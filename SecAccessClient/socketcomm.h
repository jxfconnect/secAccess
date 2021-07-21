#ifndef SOCKETCOMM_H
#define SOCKETCOMM_H

#include <QObject>
#include <QTcpSocket>

class SocketComm : public QObject
{
    Q_OBJECT
public:
    explicit SocketComm(QObject *parent = nullptr);
    bool connectToServer(QString servName, int port);
    bool close();
    bool write(QString request);
    bool writeBytes(QByteArray bytes);
    QString getServerName();
    int getPort();
    QString getStatus();
    bool isOpen();
    bool isConnected();
    void DumpDataToFile(QByteArray buf);
    QString getHostIpAddress();
    void getInterfaces();
    QString getLocalMACfromConnection();
    QString getLocalIPfromConnection();

private:
    QTcpSocket *s;

    QString mServerName;
    int mPort;
    QHostAddress *localAddr;

signals:
    void readyToPump(QByteArray);

private slots:
    void read();
};

#endif // SOCKETCOMM_H
