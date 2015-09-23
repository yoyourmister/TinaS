#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

//#include "musicserver.h"

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(int ID, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void disconnect(int ID);
    void dataReady(int ID, QByteArray data);

    //void sendSignal(QString data);

public slots:
    void readyRead();
    void disconnected();
    void sendData(QString data);
    //TODO void bytesWritten(qint64 size);

private:
    QTcpSocket *socket;

public:
    int socketDescriptor;

};

#endif // SERVERTHREAD_H
