#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(int ID, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();
    void sendData(QString data);

private:
    QTcpSocket *socket;
public:
    int socketDescriptor;

};

#endif // SERVERTHREAD_H
