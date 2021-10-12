//
// Created by hamlet on 2021/10/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <filesystem>
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

void MainWindow::addFile() {
    QStringList files = QFileDialog::getOpenFileNames(this, "添加图片", ".", "图片格式 (*.png *.jpg)");
    for (const auto &file: files) {
        std::filesystem::path file_path{file.toStdString()};
        if (!exists(file_path))
            continue;
        auto *item = new QListWidgetItem(QIcon(file), QString::fromStdString(file_path.filename()), ui->listWidget);
        item->setToolTip(file);
        ui->listWidget->addItem(item);
    }
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
    QString file = QFileDialog::getSaveFileName(this, "保存PDF", ".", "PDF格式 (*.pdf)");
    if (file.isEmpty()) return;
    std::string out_command{"convert \""};
    for (int i = 0; i < length; ++i) {
        out_command.append(ui->listWidget->item(i)->toolTip().toStdString());
        out_command.append("\" \"");
    }
    out_command.append(file.toStdString());
    out_command.append("\"");
    system(out_command.c_str());
    QDesktopServices::openUrl(QUrl(QString::fromStdString(R"(file:)" + file.toStdString()), QUrl::TolerantMode));
}

