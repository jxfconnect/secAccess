#include "socketserver.h"

#include <QJsonObject>
#include <QJsonDocument>

#include <QFile>

SocketServer::SocketServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, SIGNAL(newConnection()),this, SLOT(OnNewConnection()));
    StartServer();
}

bool SocketServer::StartServer()
{
    if(mTcpServer->listen(QHostAddress::AnyIPv4, 3456)) {
        qDebug() << "Start listening.";
        return true;
    }
    return false;
}

void SocketServer::OnNewConnection()
{
    mCurrClient = mTcpServer->nextPendingConnection();
    QJsonObject pingMessage;
    pingMessage.insert("CMD", "hello");
    QJsonDocument doc(pingMessage);
    QByteArray bytes = doc.toJson();
    mCurrClient->write(bytes);

//    mCurrClient->write(QString("hello there.").toUtf8());
//    connect(mCurrClient, SIGNAL(readyRead()), this, SLOT(OnReceive()));

    emit newClient(mCurrClient);
}

bool SocketServer::SendData(QString data)
{
    if(mCurrClient->state() == QAbstractSocket::ConnectedState) {
        mCurrClient->write(data.toUtf8());
        return true;
    }
    return false;
}

void SocketServer::OnReceive()
{
    QByteArray bytes = mCurrClient->readAll();
//    QString msg = QString::fromUtf8(bytes);
    QString ip = mCurrClient->peerAddress().toString();

    qDebug() << bytes;
    DumpDataToFile(bytes);
    emit readyToDisplay(ip, 0, bytes);
//    SendData("message received.");
}

void SocketServer::DumpDataToFile(QByteArray buf)
{
    QString filePath = "client_buffer.dmp";
    QFile dmpFile(filePath);
    if(!dmpFile.open( QIODevice::Append | QIODevice::Text )) {
        qDebug() << QString("fail open log file %1").arg(filePath);
    } else {
        QTextStream ts(&dmpFile);
        ts << buf;
        dmpFile.close();
    }
}
