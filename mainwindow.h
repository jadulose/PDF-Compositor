//
// Created by hamlet on 2021/10/12.
//

#ifndef PDF_COMPOSITOR_MAINWINDOW_H
#define PDF_COMPOSITOR_MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:

    void addFile();

    void removeSelected();

    void clearAll();

    void composite();

private:
    Ui::MainWindow *ui;
    QString last_path{"."};
};


#endif //PDF_COMPOSITOR_MAINWINDOW_H
