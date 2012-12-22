#ifndef FTP_H
#define FTP_H

#include <QObject>
#include <QTcpServer>
#include <QHostAddress>
#include <QHash>
#include <QTcpSocket>
#include <QRegExp>
#include <QDir>
#include <QHostInfo>
#include <QDateTime>
#include <QCryptographicHash>

#include "log.h"
#include "common.h"
#include "users.h"

class Ftp : public QObject
{
    Q_OBJECT
public:
    explicit Ftp(QObject *parent = 0);

    void startServer(QHostAddress addr, qint16 port);
    STATUS status();
    void stopServer();

    void sendResponse(QString str);
    QString getResponse(int i, QString str = "");

private:
    QTcpServer *server;
    QTcpSocket *readDataSock;
    QTcpServer *dataSocket;
    STATUS eSTATUS;
    MODE eMODE;
    QTcpSocket *mSocket;
    QString user;
    QCryptographicHash *hash;
    QString fileName;

    QHash<int, QString> mResponseCodes;
    QDir currentDir;
    QDir homeDir;
    QHash<int, QString> month;

    QString currentIp();
    QStringList getDirList(QDir dir);
    
signals:
    void onStarted(QString);
    void onError(QString);
    void onClose(QString);
    void onEvent(QString);

private slots:
    void incomingConnection();
    void readData();
    void readDataAtSock();
    void uploadFile();
    
public slots:
    
};

#endif // FTP_H
