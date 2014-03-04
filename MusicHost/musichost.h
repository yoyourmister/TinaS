#ifndef MUSICHOST_H
#define MUSICHOST_H

#include <QWidget>
#include <QtMultimedia>
#include <QFileInfoList>
#include <QListWidget>

#include "musicserver.h"

namespace Ui {
class MusicHost;
}

class MusicHost : public QWidget
{
    Q_OBJECT
    
public:
    explicit MusicHost(QWidget *parent = 0);
    ~MusicHost();


private slots:
    void on_but_host_clicked();

    void on_but_sendList_clicked();

    void on_but_sendSound_clicked();

    void on_but_addFolder_clicked();

    void on_but_addToList_clicked();

    void on_listWidget_playlist_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MusicHost *ui;
    MusicServer *musicserver;

    void addMusicFiles(QString dir);
};

#endif // MUSICHOST_H
