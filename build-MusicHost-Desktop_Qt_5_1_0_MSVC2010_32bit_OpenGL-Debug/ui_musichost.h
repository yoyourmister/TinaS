/********************************************************************************
** Form generated from reading UI file 'musichost.ui'
**
** Created by: Qt User Interface Compiler version 5.1.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MUSICHOST_H
#define UI_MUSICHOST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MusicHost
{
public:
    QWidget *widget;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QPushButton *but_addFolder;
    QPushButton *but_addToList;
    QVBoxLayout *verticalLayout;
    QPushButton *but_host;
    QPushButton *but_sendList;
    QPushButton *but_sendSound;
    QVBoxLayout *verticalLayout_3;
    QListWidget *listWidget_playlist;
    QListWidget *listWidget_allSongs;

    void setupUi(QWidget *MusicHost)
    {
        if (MusicHost->objectName().isEmpty())
            MusicHost->setObjectName(QStringLiteral("MusicHost"));
        MusicHost->resize(459, 430);
        widget = new QWidget(MusicHost);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 10, 343, 394));
        gridLayout = new QGridLayout(widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        but_addFolder = new QPushButton(widget);
        but_addFolder->setObjectName(QStringLiteral("but_addFolder"));

        verticalLayout_2->addWidget(but_addFolder);

        but_addToList = new QPushButton(widget);
        but_addToList->setObjectName(QStringLiteral("but_addToList"));

        verticalLayout_2->addWidget(but_addToList);


        gridLayout->addLayout(verticalLayout_2, 1, 1, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        but_host = new QPushButton(widget);
        but_host->setObjectName(QStringLiteral("but_host"));

        verticalLayout->addWidget(but_host);

        but_sendList = new QPushButton(widget);
        but_sendList->setObjectName(QStringLiteral("but_sendList"));

        verticalLayout->addWidget(but_sendList);

        but_sendSound = new QPushButton(widget);
        but_sendSound->setObjectName(QStringLiteral("but_sendSound"));

        verticalLayout->addWidget(but_sendSound);


        gridLayout->addLayout(verticalLayout, 0, 1, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        listWidget_playlist = new QListWidget(widget);
        listWidget_playlist->setObjectName(QStringLiteral("listWidget_playlist"));

        verticalLayout_3->addWidget(listWidget_playlist);

        listWidget_allSongs = new QListWidget(widget);
        listWidget_allSongs->setObjectName(QStringLiteral("listWidget_allSongs"));
        listWidget_allSongs->setSelectionMode(QAbstractItemView::MultiSelection);

        verticalLayout_3->addWidget(listWidget_allSongs);


        gridLayout->addLayout(verticalLayout_3, 0, 0, 2, 1);


        retranslateUi(MusicHost);

        QMetaObject::connectSlotsByName(MusicHost);
    } // setupUi

    void retranslateUi(QWidget *MusicHost)
    {
        MusicHost->setWindowTitle(QApplication::translate("MusicHost", "MusicHost", 0));
        but_addFolder->setText(QApplication::translate("MusicHost", "Add Folder", 0));
        but_addToList->setText(QApplication::translate("MusicHost", "Add to List", 0));
        but_host->setText(QApplication::translate("MusicHost", "Host", 0));
        but_sendList->setText(QApplication::translate("MusicHost", "Send List", 0));
        but_sendSound->setText(QApplication::translate("MusicHost", "Send Sound", 0));
    } // retranslateUi

};

namespace Ui {
    class MusicHost: public Ui_MusicHost {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MUSICHOST_H
