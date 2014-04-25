#include "serverthread.h"

//#include "musicserver.h"

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
        qDebug()<<"Error setSocketDescriptor";
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    //connect(this, SIGNAL(sendSignal(QString)), this, SLOT(sendData(QString)), Qt::DirectConnection);

    qDebug()<<"Socket connected" << socketDescriptor;

    exec();
}

void ServerThread::readyRead()
{
    QByteArray data=socket->readAll();

    qDebug() << "Data in" << socketDescriptor << data;
    //((MusicServer)(QObject::parent())).gotData(this->socketDescriptor, data);
    //server->gotData(this->socketDescriptor, data);
    //(parent)->gotData(this->socketDescriptor,data);

    //socket->write(data);
    emit dataReady(socketDescriptor, data);
}

void ServerThread::disconnected()
{
    qDebug() << "Disconnect" << socketDescriptor;

    //((MusicServer)(QObject::parent())).disconnectConnection(socketDescriptor);
    //server->disconnectConnection(socketDescriptor);

    emit disconnect(socketDescriptor);

    socket->deleteLater();
    exit(0);
}

void ServerThread::sendData(QString data)
{
    if (!socket)
    {
        return;
    }
    qDebug()<<"writing to " << socketDescriptor << ": " << data;
    socket->write(data.toUtf8());
    qDebug()<<"written!";
}
