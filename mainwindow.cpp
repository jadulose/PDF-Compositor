//
// Created by hamlet on 2021/10/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <cstdlib>
#include <QDesktopServices>
#include <QScreen>
#include <QDragEnterEvent>
#include <QMimeData>
#include <vector>
#include <filesystem>
#include <iostream>

static const std::vector<std::string> AVAILABLE_FORMATS{".png", ".jpg", ".gif", ".bmp", ".svg"};

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->widget->setVisible(false);
    this->move(this->screen()->availableGeometry().center() - this->rect().center());
    connect(ui->pushButton_Add_File, SIGNAL(clicked(bool)), this, SLOT(addFile()));
    connect(ui->pushButton_Remove_Selected, SIGNAL(clicked(bool)), this, SLOT(removeSelected()));
    connect(ui->pushButton_Clear_All, SIGNAL(clicked(bool)), this, SLOT(clearAll()));
    connect(ui->pushButton_Composite, SIGNAL(clicked(bool)), this, SLOT(composite()));
    connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(whetherShowToolBar()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem * )), this, SLOT(previewPhoto(QListWidgetItem * )));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem * )), this, SLOT(showPhoto(QListWidgetItem * )));
    setAcceptDrops(true);

    auto *fileAdder = new FileAdder(this);
    fileAdder->moveToThread(&workerThread1);
    connect(&workerThread1, &QThread::finished, fileAdder, &QObject::deleteLater);
    connect(this, &MainWindow::signNeedAddFile, fileAdder, &FileAdder::doWork);
    connect(fileAdder, &FileAdder::resultReady, this, &MainWindow::slotNeedAddFileFinished);
    workerThread1.start();

    auto *pdfCompositor = new PDFCompositor(ui->listWidget);
    pdfCompositor->moveToThread(&workerThread2);
    connect(&workerThread2, &QThread::finished, pdfCompositor, &PDFCompositor::deleteLater);
    connect(this, &MainWindow::signComposite, pdfCompositor, &PDFCompositor::doWork);
    connect(pdfCompositor, &PDFCompositor::resultReady, this, &MainWindow::slotCompositeFinished);
    workerThread2.start();
}

MainWindow::~MainWindow() {
    clearAll();
    workerThread1.quit();
    workerThread2.quit();
    workerThread1.wait();
    workerThread2.wait();
    delete ui;
}

QString getFileName(const QString &file_path) {
    for (auto i = file_path.length() - 1; i >= 0; --i)
        if (file_path.at(i) == QChar('/') || file_path.at(i) == QChar('\\'))
            return std::move(file_path.mid(i + 1));
    return file_path;
}

QString getDirectory(const QString &file_path) {
    for (auto i = file_path.length() - 1; i >= 0; --i)
        if (file_path.at(i) == QChar('/') || file_path.at(i) == QChar('\\'))
            return std::move(file_path.mid(0, i));
    return file_path;
}

QString getExtension(const QString &file_path) {
    for (auto i = file_path.length() - 1; i >= 0; --i)
        if (file_path.at(i) == QChar('.'))
            return std::move(file_path.mid(i));
    return file_path;
}

void MainWindow::addFile(const QString &file_name) {
    QPixmap pixmap{file_name};
    pixmap = pixmap.scaledToHeight(800);
    auto *item = new QListWidgetItem(pixmap, getFileName(file_name), ui->listWidget);
    item->setToolTip(file_name);
    ui->listWidget->addItem(item);
}

void MainWindow::addFile() {
    showMessage("添加图片中...");
    QString formats{"图片格式 ("};
    for (const auto &format: AVAILABLE_FORMATS)
        formats.append(" *").append(format.c_str());
    formats.append(");;全部文件 (*.*)");
    auto *files = new QStringList(QFileDialog::getOpenFileNames(this, "添加图片", last_path, formats));
    emit signNeedAddFile(files);
}

void MainWindow::removeSelected() {
    for (const auto &item: ui->listWidget->selectedItems()) {
        ui->listWidget->removeItemWidget(item);
        delete item;
    }
    showMessage("已删除所选图片", 1000);
}

void MainWindow::clearAll() {
    ui->listWidget->clear();
    showMessage("已清空所有图片", 1000);
}

void MainWindow::composite() {
    int length = ui->listWidget->count();
    if (!length) return;
    showMessage("开始合成PDF...");
    auto *file = new QString(QFileDialog::getSaveFileName(this, "保存PDF", last_path, "PDF格式 (*.pdf)"));
    ui->pushButton_Composite->setEnabled(false);
    emit signComposite(file);
}

void MainWindow::whetherShowToolBar() {
    ui->widget->setVisible(ui->listWidget->selectedItems().length() == 1);
}

void MainWindow::previewPhoto(QListWidgetItem *item) {
    if (!ui->widget->isVisible()) return;
    m_pixmap = QPixmap{item->toolTip()};
    ui->widget->setVisible(true);
    ui->widget->setFixedWidth((int) ((ui->centralwidget->width() - 18) * 0.618));
    ui->label_title->setText(item->text());
    ui->labe_pic->setPixmap(
            m_pixmap.scaled(ui->widget->width() - 12, ui->labe_pic->height(), Qt::KeepAspectRatio,
                            Qt::SmoothTransformation));
    showMessage(item->toolTip(), 2000);
}

void MainWindow::showPhoto(QListWidgetItem *item) {
    showMessage(QString("打开图片：").append(item->text()), 5000);
    QDesktopServices::openUrl(QUrl(QString("file:").append(item->toolTip()), QUrl::TolerantMode));
}

void MainWindow::resizeEvent(QResizeEvent *ev) {
    ui->widget->setFixedWidth((int) ((ui->centralwidget->width() - 18) * 0.618));
    if (ui->widget->isVisible()) {
        ui->labe_pic->setPixmap(
                m_pixmap.scaled(ui->widget->width() - 22, ui->labe_pic->height() - 10, Qt::KeepAspectRatio,
                                Qt::SmoothTransformation));
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    ui->stackedWidget->setCurrentIndex(1);
    ui->statusbar->hide();
    // 窗口显示在最前面
//    this->setWindowState(Qt::WindowActive);
//    this->activateWindow();
    event->acceptProposedAction();
}

inline bool formats_contains(const std::string &str) {
    auto result = std::find(AVAILABLE_FORMATS.begin(), AVAILABLE_FORMATS.end(), str);
    return !(result == AVAILABLE_FORMATS.end());
}

void MainWindow::dropEvent(QDropEvent *event) {
    ui->statusbar->show();
    showMessage("丢入图片中...");
    ui->stackedWidget->setCurrentIndex(0);

//    const QMimeData *mimeData = event->mimeData();
//    if (mimeData->hasImage()) {
//        qDebug() << "图片类型MimeData";
//    } else if (mimeData->hasFormat(QLatin1String("text/markdown"))) {
//        qDebug() << "text/markdown: " << QString::fromUtf8(mimeData->data(QLatin1String("text/markdown")));
//    } else if (mimeData->hasHtml()) {
//        qDebug() << "html类型MimeData";
//    } else if (mimeData->hasText()) {
//        qDebug() << "text类型：" << mimeData->text();
//    } else if (mimeData->hasUrls()) {
//        qDebug() << "url类型";
//    } else {
//        qDebug() << "无法显示类型";
//    }
//    qDebug() <<  mimeData->formats();

    auto *path_list = new QStringList();
    for (const auto &url: event->mimeData()->urls())
        path_list->append(url.path());
    emit signNeedAddFile(path_list);
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event) {
    ui->statusbar->show();
    showMessage("取消丢入图片", 1000);
    event->accept();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setLastPath(const QString &path) {
    last_path = path;
}

inline void MainWindow::showMessage(const QString &text, int timeout) {
    ui->statusbar->showMessage(text, timeout);
}

void MainWindow::slotNeedAddFileFinished(bool state) {
    showMessage(state ? "成功导入图片" : "取消导入图片", 1000);
}

void MainWindow::slotCompositeFinished(QString *file) {
    if (file->isEmpty())
        showMessage("取消导出PDF", 1000);
    else
        showMessage(QString("成功导出PDF：").append(*file), 5000);
    ui->pushButton_Composite->setEnabled(true);
    QDesktopServices::openUrl(QUrl(QString("file:").append(*file), QUrl::TolerantMode));
    delete file;
}

FileAdder::FileAdder(MainWindow *father) : m_father{father} {
}

void FileAdder::doWork(QStringList *path_list) {
    bool state = !path_list->isEmpty();
    for (const auto &path: *path_list) {
//        std::cout << path.toStdString() << std::endl;
//        std::cout << std::string(path.toLocal8Bit()) << std::endl;
//        std::cout << p.string() << std::endl;
        // 当前这种方法不支持网络图片
#ifdef __GNUC__
        std::filesystem::path p{path.toStdString()};
        if (!std::filesystem::exists(p)) continue;
        if (std::filesystem::is_regular_file(p) && formats_contains(p.extension().string())) {
            m_father->addFile(path);
            m_father->setLastPath(QString::fromStdString(p.parent_path().string()));
        } else if (std::filesystem::is_directory(p)) {
            for (const auto &pp: std::filesystem::directory_iterator(p)) {
                // 并不是recursive操作，只能添加一个文件夹
                if (std::filesystem::is_regular_file(pp) && formats_contains(pp.path().extension().string())) {
                    m_father->addFile(QString::fromStdString(pp.path().string()));
                }
            }
            m_father->setLastPath(QString::fromStdString(p.string()));
        }
#else
        if (formats_contains(getExtension(path.mid(1)).toStdString())) {
            m_father->addFile(path.mid(1));
            m_father->setLastPath(path.mid(1));
        }
#endif
    }
    delete path_list;
    emit resultReady(state);
}

PDFCompositor::PDFCompositor(QListWidget *list) : m_fatherList(list) {
}

void PDFCompositor::doWork(QString *output_path) {
    if (!output_path->isEmpty()) {
        int length = m_fatherList->count();
        std::string out_command{"convert \""};
        for (int i = 0; i < length; ++i) {
            out_command.append(m_fatherList->item(i)->toolTip().toLocal8Bit());
            out_command.append("\" \"");
        }
        out_command.append(output_path->toLocal8Bit());
        out_command.append("\"");
        std::cout << out_command << std::endl;
        system(out_command.c_str());
    }
    emit resultReady(output_path);
}
