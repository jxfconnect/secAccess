#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class SocketServer : public QObject
{
    Q_OBJECT
public:
    explicit SocketServer(QObject *parent = nullptr);
    bool StartServer();
    bool SendData(QString data);
    void DumpDataToFile(QByteArray buf);

private:
    QTcpServer *mTcpServer;
    QTcpSocket *mCurrClient;

signals:
    void readyToDisplay(QString, int, QByteArray);
    void newClient(QTcpSocket *);

private slots:
    void OnNewConnection();
    void OnReceive();

};

#endif // SOCKETSERVER_H
