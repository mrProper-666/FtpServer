#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName( "CP-1251" );
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(codec);

    a.setApplicationName(QObject::tr("Ftp сервер"));
    a.setOrganizationName(QObject::tr("08i2"));
    a.setApplicationVersion(QObject::tr("v0.1"));

    MainWindow w;
    w.show();
    
    return a.exec();
}
