//
// Created by hamlet on 2021/10/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <cstdlib>
#include <QDesktopServices>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->move(this->screen()->availableGeometry().center() - this->rect().center());
    connect(ui->pushButton_Add_File, SIGNAL(clicked(bool)), this, SLOT(addFile()));
    connect(ui->pushButton_Remove_Selected, SIGNAL(clicked(bool)), this, SLOT(removeSelected()));
    connect(ui->pushButton_Clear_All, SIGNAL(clicked(bool)), this, SLOT(clearAll()));
    connect(ui->pushButton_Composite, SIGNAL(clicked(bool)), this, SLOT(composite()));
}

MainWindow::~MainWindow() {
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

void MainWindow::addFile() {
    QStringList files = QFileDialog::getOpenFileNames(this, "添加图片", last_path,
                                                      "图片格式 (*.png *.jpg *.gif *.bmp *.svg);;全部文件 (*.*)");
    if (files.isEmpty()) return;
    for (const auto &file: files) {
        auto *item = new QListWidgetItem(QIcon(file), getFileName(file), ui->listWidget);
        item->setToolTip(file);
        ui->listWidget->addItem(item);
    }
    last_path = getDirectory(files[0]);
}

void MainWindow::removeSelected() {
    for (const auto &item: ui->listWidget->selectedItems()) {
        ui->listWidget->removeItemWidget(item);
        delete item;
    }
}

void MainWindow::clearAll() {
    ui->listWidget->clear();
}

void MainWindow::composite() {
    int length = ui->listWidget->count();
    if (!length) return;
    QString file = QFileDialog::getSaveFileName(this, "保存PDF", last_path, "PDF格式 (*.pdf)");
    if (file.isEmpty()) return;
    std::string out_command{"convert \""};
    for (int i = 0; i < length; ++i) {
        out_command.append(ui->listWidget->item(i)->toolTip().toLocal8Bit());
        out_command.append("\" \"");
    }
    out_command.append(file.toLocal8Bit());
    out_command.append("\"");
    system(out_command.c_str());
    QDesktopServices::openUrl(QUrl(QString("file:").append(file), QUrl::TolerantMode));
}
