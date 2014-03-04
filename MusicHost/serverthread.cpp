#include "serverthread.h"

ServerThread::ServerThread(int ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor=ID;
}

void ServerThread::run()
{
    qDebug()<<"Starting thread" << socketDescriptor;
    socket=new QTcpSocket();

    if (!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);

    qDebug()<<"Socket connected" << socketDescriptor;

    exec();
}

void ServerThread::readyRead()
{
    QByteArray data=socket->readAll();

    qDebug() << "Data in" << socketDescriptor << data;

    //socket->write(data);
}

void ServerThread::disconnected()
{
    qDebug() << "Disconnect" << socketDescriptor;

    socket->deleteLater();
    exit(0);
}

void ServerThread::sendData(QString data)
{
    if (!socket)
    {
        return;
    }
    qDebug()<<"writing..."<<data;
    socket->write(data.toUtf8());
    qDebug()<<"written!";
}
