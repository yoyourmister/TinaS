#include "musichost.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MusicHost w;
    w.show();
    
    return a.exec();
}
