#include "virtualfile.h"
#include <QDebug>

VirtualFile::VirtualFile(QObject *parent)
    :   QIODevice(parent)
        ,m_pos(0)
{
}

VirtualFile::~VirtualFile() {

}

void VirtualFile::start() {
    m_buffer.clear();
    open(QIODevice::ReadWrite);
}

void VirtualFile::stop() {
    m_pos=0;
    close();
}


qint64 VirtualFile::readData(char *data, qint64 maxlen) {
    //qDebug()<<"read VF "<<maxlen;
    qint64 total = 0;
    while (maxlen - total > 0) {
        const qint64 chunk = qMin((m_buffer.size() - m_pos), maxlen - total);
        memcpy(data + total, m_buffer.constData() + m_pos, chunk);
        m_pos = (m_pos + chunk);// % m_buffer.size();
        total += chunk;
    }
    return total;
}

qint64 VirtualFile::writeData(const char *data, qint64 len) {
    //qDebug()<<"write Data";
    //m_buffer.resize(m_buffer.length()+len);
    m_buffer.append(data,len);
    return 0;
}

qint64 VirtualFile::bytesAvailable() const {
    //qDebug()<<"available";
    return m_buffer.size() + QIODevice::bytesAvailable();
}

void VirtualFile::clear() {
    m_buffer.clear();
    m_pos=0;
}
