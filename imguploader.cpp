#include "imguploader.h"

imgUploader::imgUploader(QObject *parent) :
    QObject(parent)
{
    UserAgent = "Fotomatis";
    Uploading = false;
}


void imgUploader::uploadImage(QString FileName, QString PhpUploadURL, QString FieldName)
{

    QUrl serviceUrl = QUrl(PhpUploadURL);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QNetworkRequest request(serviceUrl);
    request.setRawHeader("User-Agent", UserAgent.toAscii());

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + FieldName + "\"; filename=\"" + FileName +  "\""));

    QFile *file = new QFile(FileName);
    file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart
    file->open(QIODevice::ReadOnly);

    imagePart.setBodyDevice(file);
    multiPart->append(imagePart);

    // Call the webservice
    //QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    currentUpload =  manager.post(request, multiPart);

    connect(currentUpload,SIGNAL(uploadProgress(qint64,qint64)),this, SLOT(onUploadProgress(qint64,qint64)));
    connect(currentUpload, SIGNAL(finished()),this,SLOT(onUploadFinished()));

    Uploading = true;
    //reply->deleteLater();
}

bool imgUploader::isUploading()
{
    return Uploading;
}

void imgUploader::onUploadFinished()
{
    //qDebug() << "c:upload finished";
    Uploading = false;
    currentUpload->deleteLater();
    emit uploadFinished();
}

void imgUploader::onUploadProgress(qint64 uploaded, qint64 total)
{

 emit uploadProgress( uploaded,  total);
}
