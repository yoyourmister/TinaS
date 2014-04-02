#ifndef VIRTUALFILE_H
#define VIRTUALFILE_H

#include <QIODevice>
#include <QAudioOutput>
#include <QByteArray>
#include <QObject>

class VirtualFile : public QIODevice
{

    Q_OBJECT

public:
    VirtualFile(QObject *parent);
    ~VirtualFile();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

    void clear();

private:

    qint64 m_pos;
    QByteArray m_buffer;
};

#endif // VIRTUALFILE_H
