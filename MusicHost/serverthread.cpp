#include "serverthread.h"

//#include "musicserver.h"

ServerThread::ServerThread(int ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor=ID;

    qDebug()<<"Create Socket";

    socket=new QTcpSocket();

    if (!socket->setSocketDescriptor(this->socketDescriptor))
    {
        qDebug()<<"Error setSocketDescriptor";
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()),    this, SLOT(readyRead()),    Qt::QueuedConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::QueuedConnection);
    //TODO connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)), Qt::QueuedConnection);
    //connect(this, SIGNAL(sendSignal(QString)), this, SLOT(sendData(QString)), Qt::DirectConnection);

    qDebug()<<"Socket Signals connected" << socketDescriptor;

}

void ServerThread::run()
{
    qDebug()<<"Starting thread" << socketDescriptor;
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
    if (!socket || socket->state()!=QAbstractSocket::ConnectedState)
    {
        return;
    }
    qDebug()<<"writing to " << socketDescriptor << ": " << data;
    qDebug()<<"written bytes:" << socket->write(data.toUtf8());
    socket->waitForBytesWritten();
}
