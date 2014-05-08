#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createConnection();

    server = new QUdpSocket(this);
    server->bind(QHostAddress::LocalHost, 9090);

    connect(server,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));

    modQueue = new QSqlTableModel(this);
    modQueue->setTable("photos");
    modQueue->select();

    ui->tabQueue->setModel(modQueue);
    ui->tabQueue->setColumnHidden(0, true);
    ui->tabQueue->setColumnWidth(1,600);

    queueUploading = false;

    connect(&Uploader,SIGNAL(uploadFinished()), this, SLOT(on_uploadQueueFinished()));
    connect(&Uploader,SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(on_progressUpload(qint64,qint64)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createConnection()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("photos.dat");
    db.open();

    //QSqlQuery query;
    //query.exec("DROP TABLE photos");
    //query.exec("CREATE TABLE photos (id INTEGER PRIMARY KEY AUTOINCREMENT, filename VARCHAR(40) NOT NULL, status INTEGER NOT NULL)");

}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            QString droppedFile =  urlList.at(i).toLocalFile();
            addQueue(droppedFile);
        }
    }

    //event->acceptProposedAction();
}

void MainWindow::readPendingDatagrams()
{
    while(server->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(server->pendingDatagramSize());
        server->readDatagram(datagram.data(),datagram.size());
        QString test(datagram);
        addQueue(test);

        if (Uploader.isUploading() == false){
            on_cmdUploadAll_clicked();
        }

    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QModelIndex idx = ui->tabQueue->currentIndex();
    modQueue->removeRow(idx.row());
}

void MainWindow::on_pushButton_clicked()
{
  /*  if (Uploader.isUploading())
    {
        ui->txtDebugConsole->appendPlainText("Wait for Upload finished");\
        return;
    }

    QModelIndex idx     =   ui->tabQueue->currentIndex();

    QString FileName    =   modQueue->record(idx.row()).value("filename").toString();
    QString ServiceURL  =   ui->txtServiceURL->text();
    QString FormName    =   "userfile";

    CurrentUploadRowIndex = idx.row();
    Uploader.uploadImage(FileName, ServiceURL, FormName);

    //connect(&Uploader,SIGNAL(uploadFinished()), this, SLOT(on_uploadFinished()));
    //connect(&Uploader,SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(on_progressUpload(qint64,qint64)));

    ui->txtDebugConsole->appendPlainText("Uploading " + FileName );
    */
}

void MainWindow::on_progressUpload(qint64 uploaded, qint64 total)
{

    ui->progressUpload->setMaximum(total);
    ui->progressUpload->setValue(uploaded);

}

void MainWindow::on_uploadFinished()
{
    ui->progressUpload->setValue(0);
    ui->txtDebugConsole->appendPlainText("Upload Finished");
}

void MainWindow::on_uploadQueueFinished()
{
    deQueue();
    on_cmdUploadAll_clicked();
}


void MainWindow::deQueue()
{
    QSqlQuery SqlQueueQuery;
    SqlQueueQuery.prepare("UPDATE photos SET status=:status WHERE id==:id");
    SqlQueueQuery.bindValue(":id", CurrentUploadRowIndex);
    SqlQueueQuery.bindValue(":status", 1);
    SqlQueueQuery.exec();
    modQueue->select();
}

void MainWindow::addQueue(QString Filename)
{
    QSqlRecord newrecord;
    newrecord.append(QSqlField( "filename", QVariant::String));
    newrecord.append(QSqlField( "status", QVariant::Int));
    newrecord.setValue(0,Filename);
    newrecord.setValue(1,0);
    modQueue->insertRecord(-1, newrecord);
}

QString MainWindow::nextQueue()
{

    QSqlQueryModel queue;
    QString NextQueue;
    queue.setQuery("SELECT * FROM photos WHERE status == 0");
    NextQueue = queue.record(0).value("filename").toString();

    if(queue.record().count() == 0)
    {
        return "";
    }

    CurrentUploadRowIndex = queue.record(0).value("id").toInt();
    ui->txtDebugConsole->appendPlainText(NextQueue);

    return NextQueue;
}

void MainWindow::on_cmdUploadAll_clicked()
{
    if (Uploader.isUploading())
    {
        //ui->txtDebugConsole->appendPlainText("");\
        return;
    }

    queueUploading = true;

    QString FileName    =   nextQueue();

    if (FileName == "")
    {
        ui->txtDebugConsole->appendPlainText("Queue Finished");
        return;
    }

    QString ServiceURL  =   ui->txtServiceURL->text();
    QString FormName    =   "userfile";

    Uploader.uploadImage(FileName, ServiceURL, FormName);
    ui->txtDebugConsole->appendPlainText("Queue Upload: " + FileName );
}

void MainWindow::on_cmdDeleteSelected_clicked()
{
    QModelIndex idx = ui->tabQueue->currentIndex();
    modQueue->removeRow(idx.row());

}

void MainWindow::on_cmdCreateDB_clicked()
{
    //QSqlDatabase db;
    //db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("photos.dat");
    //db.open();

    QSqlQuery query;
    query.exec("DROP TABLE photos");
    query.exec("CREATE TABLE photos (id INTEGER PRIMARY KEY AUTOINCREMENT, filename VARCHAR(40) NOT NULL, status INTEGER NOT NULL)");

}
