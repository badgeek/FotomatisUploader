#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDrag>
#include <QQueue>
#include <QDragEnterEvent>
#include <QtSql>
#include <QSqlRecord>
#include <QUdpSocket>
#include <QDebug>
#include <QFile>

#include <imguploader.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QUdpSocket* server;
    QSqlTableModel* modQueue;

    void createConnection();
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    imgUploader Uploader;
    void addQueue(QString Filename);
    QString nextQueue();
    void deQueue();
public slots:
    void readPendingDatagrams();


private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_progressUpload(qint64 uploaded, qint64 total);
    void on_uploadFinished();
    void on_uploadQueueFinished();
    void on_cmdUploadAll_clicked();


    void on_cmdDeleteSelected_clicked();

    void on_cmdCreateDB_clicked();

private:
    Ui::MainWindow *ui;
    int CurrentUploadRowIndex;
    bool queueUploading;
    //QQueue uploadQueue;
};

#endif // MAINWINDOW_H
