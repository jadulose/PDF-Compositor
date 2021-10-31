//
// Created by hamlet on 2021/10/12.
//

#ifndef PDF_COMPOSITOR_MAINWINDOW_H
#define PDF_COMPOSITOR_MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class FileAdder;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void addFile(const QString &file_name);

    void setLastPath(const QString &path);

signals:

    void signNeedAddFile(QStringList *);

    void signComposite(QString *);

private slots:

    void slotNeedAddFileFinished(bool state);

    void slotCompositeFinished(QString *file);

public slots:

    void addFile();

    void removeSelected();

    void clearAll();

    void composite();

    void whetherShowToolBar();

    void previewPhoto(QListWidgetItem *);

    void showPhoto(QListWidgetItem *);

    void resizeEvent(QResizeEvent *ev) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dragLeaveEvent(QDragLeaveEvent *event) override;

    void dropEvent(QDropEvent *event) override;

private:
    Ui::MainWindow *ui;
    QString last_path{"."};

    QPixmap m_pixmap;
    QThread workerThread1, workerThread2;

    inline void showMessage(const QString &text, int timeout = 0);
};

class FileAdder : public QObject {
Q_OBJECT
public:
    explicit FileAdder(MainWindow *father);

public slots:

    void doWork(QStringList *path_list);

signals:

    void resultReady(bool state);

private:
    MainWindow *m_father;
};

class PDFCompositor : public QObject {
Q_OBJECT
public:
    explicit PDFCompositor(QListWidget *list);

public slots:

    void doWork(QString *output_path);

signals:

    void resultReady(QString *file);

private:
    QListWidget *m_fatherList;
};


#endif //PDF_COMPOSITOR_MAINWINDOW_H
