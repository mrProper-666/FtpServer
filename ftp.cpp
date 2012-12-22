#include "ftp.h"

Ftp::Ftp(QObject *parent) :
    QObject(parent)
{
    currentDir.setPath(tr("D:/"));
    homeDir.setPath(tr("D:/"));

    mResponseCodes.insert(110,"Restart marker reply.");
    mResponseCodes.insert(120,"Service ready in nnn minutes.");
    mResponseCodes.insert(125,"Data connection already open; transfer starting.");
    mResponseCodes.insert(150,"File status okay; about to open data connection.");
    mResponseCodes.insert(200,"Command okay.");
    mResponseCodes.insert(202,"Command not implemented, superfluous at this site.");
    mResponseCodes.insert(211,"System status, nothing to report.");
    mResponseCodes.insert(212,"Directory status.");
    mResponseCodes.insert(213,"End of status.");
    mResponseCodes.insert(214,"Help command successful.");
    mResponseCodes.insert(215,"UNIX Type: L8.");
    mResponseCodes.insert(220,"Service ready for new user.");
    mResponseCodes.insert(221,"Service closing control connection.");
    mResponseCodes.insert(225,"Data connection open; no transfer in progress.");
    mResponseCodes.insert(226,"Closing data connection.");
    mResponseCodes.insert(227,"Entering Passive Mode (h1,h2,h3,h4,p1,p2).");
    mResponseCodes.insert(230,"User logged in, proceed.");
    mResponseCodes.insert(250,"Requested file action okay, completed.");
    mResponseCodes.insert(257,"Path was created.");
    mResponseCodes.insert(331,"Password required.");
    mResponseCodes.insert(332,"Need account for login.");
    mResponseCodes.insert(350,"Requested file action pending further information.");
    mResponseCodes.insert(421,"Service not available, closing control connection.");
    mResponseCodes.insert(425,"Can't open data connection.");
    mResponseCodes.insert(426,"Connection closed; transfer aborted.");
    mResponseCodes.insert(450,"Requested file action not taken.");
    mResponseCodes.insert(451,"Requested action aborted: local error in processing.");
    mResponseCodes.insert(452,"Requested action not taken.");
    mResponseCodes.insert(500,"Syntax error, command unrecognized.");
    mResponseCodes.insert(501,"Syntax error in parameters or arguments.");
    mResponseCodes.insert(502,"Command not implemented.");
    mResponseCodes.insert(503,"Bad sequence of commands.");
    mResponseCodes.insert(504,"Command not implemented for that parameter.");
    mResponseCodes.insert(530,"Not logged in.");
    mResponseCodes.insert(532,"Need account for storing files.");
    mResponseCodes.insert(550,"Requested action not taken.");
    mResponseCodes.insert(551,"Requested action aborted: page type unknown.");
    mResponseCodes.insert(552,"Requested file action aborted.");
    mResponseCodes.insert(553,"Requested action not taken.");
    mResponseCodes.insert(550,"The filename, directory name, or volume label syntax is incorrect.");

    month.insert(1, "Jan");
    month.insert(2, "Feb");
    month.insert(3, "Mar");
    month.insert(4, "Apr");
    month.insert(5, "May");
    month.insert(6, "Jun");
    month.insert(7, "Jul");
    month.insert(8, "Aug");
    month.insert(9, "Sep");
    month.insert(10, "Oct");
    month.insert(11, "Nov");
    month.insert(12, "Dec");

    server = new QTcpServer(this);
    hash = new QCryptographicHash(QCryptographicHash::Md5);

    connect(server, SIGNAL(newConnection()), this, SLOT(incomingConnection()));
}

void Ftp::startServer(QHostAddress addr, qint16 port){
    eSTATUS = STARTING;
    if (server->listen(addr, port)) {
        eSTATUS = STARTED;
        emit onStarted(tr("Сервер успешно запущен"));
    } else {
        eSTATUS = STOPED;
        emit onError(server->errorString());
    }
}

STATUS Ftp::status(){
    return eSTATUS;
}

void Ftp::stopServer(){
    server->close();
    eSTATUS = STOPED;
    emit onClose(tr("Сервер остановлен"));
}

void Ftp::incomingConnection(){
    mSocket = server->nextPendingConnection();

    connect(mSocket, SIGNAL(readyRead()), this, SLOT(readData()));

    emit onEvent(tr("Новое соединение с %1").arg(mSocket->localAddress().toString()));
    sendResponse(getResponse(220));
}

void Ftp::sendResponse(QString str){
    emit onEvent(tr("Сервер: %1").arg(str));
    mSocket->write(str.toAscii() + "\r\n");
}

QString Ftp::getResponse(int i, QString str){
    if (str.isEmpty()) {
        return QString("%1 %2").arg(i).arg(mResponseCodes.value(i));
    } else {
        return QString("%1 %2").arg(i).arg(str);
    }
}

void Ftp::readData(){
    QByteArray array = mSocket->readAll();
    qDebug() << array;

    if (array.contains("USER")) {
        QRegExp rx("^USER\\s(.*)\r\n");
        rx.indexIn(array);
        user = rx.cap(1);
        int id = Users::instance()->id(user);
        QString pass = Users::instance()->load(tr("%1/%2").arg(id).arg(user), "empty").toString();
        if (Users::instance()->isUser(user)) {
            if (pass == "empty") {
                sendResponse(getResponse(331));
            } else {
                sendResponse(getResponse(230));
            }
        } else {
            sendResponse(getResponse(530));
        }
        return;
    }

    if (array.contains("PASS")) {
        QRegExp rx("^PASS\\s(.*)\r\n");
        rx.indexIn(array);
        int id = Users::instance()->id(user);
        QString pass = Users::instance()->load(tr("%1/pass").arg(id), "empty").toString();
        QByteArray arr;
        arr.append(rx.cap(1));
        hash->addData(arr);
        qDebug() << hash->result() + "\n" << pass;
        if (pass == QString(hash->result())) {
            sendResponse(getResponse(230));
        } else {
            sendResponse(getResponse(530));
        }
        hash->reset();
        return;
    }

    if (array.contains("SYST")) {
        sendResponse(getResponse(215));
        return;
    }

    if (array.contains("FEAT")) {
        sendResponse(getResponse(211));
        return;
    }

    if (array.contains("PWD")) {
        QString path;
        if (currentDir.path() == homeDir.path()) {
            path = "/";
        } else {
            path = currentDir.path();
            path.remove(homeDir.path());
            path.prepend("/");
        }
        sendResponse(getResponse(257, QString("\"%1\" is your current dir").arg(path)));
        return;
    }

    if (array.contains("TYPE")) {
        sendResponse(getResponse(200, tr("TYPE is now 8-bit binary")));
        return;
    }

    if (array.contains("PASV")) {
        dataSocket = new QTcpServer(this);
        dataSocket->listen(QHostAddress::Any, 6666);
        connect(dataSocket, SIGNAL(newConnection()), this, SLOT(readDataAtSock()));
        QString addr = tr("Entering Passive Mode (") + currentIp();
        addr.replace('.', ',');
        addr.append(",26,10)");
        sendResponse(getResponse(227, addr));
        eMODE = PASSIVE;
        return;
    }

    if (array.contains("LIST")) {
        sendResponse(getResponse(150));
        return;
    }

    if (array.contains("CWD")) {
        QRegExp rx("^CWD\\s(.*)\\r\\n");
        rx.indexIn(array);
        QDir dir;
        if (rx.cap(1) != "/") {
            if (currentDir.dirName() == rx.cap(1)) {
                currentDir.cdUp();
            } else {
                currentDir.cd(rx.cap(1));
            }
        } else {
            currentDir = homeDir;
        }
        sendResponse(getResponse(150));
        QStringList list = getDirList(dir);
        foreach (QString str, list) {
            readDataSock->write(qPrintable(str));
        }
        readDataSock->write("\r\n");
        readDataSock->close();
        dataSocket->close();
        sendResponse(getResponse(226));
        return;
    }

    if (array.contains("CDUP")) {
        currentDir.cdUp();
        if (homeDir.absolutePath().length() > currentDir.absolutePath().length()) {
            sendResponse(getResponse(550));
            return;
        }
        sendResponse(getResponse(150));
        QStringList list = getDirList(currentDir);
        foreach (QString str, list) {
            readDataSock->write(qPrintable(str));
        }
        readDataSock->write("\r\n");
        readDataSock->close();
        dataSocket->close();
        sendResponse(getResponse(226));
        return;
    }

    if (array.contains("SIZE")) {
        QFileInfo fi(currentDir, array.remove(0, 6));
        sendResponse(getResponse(200, tr("%1").arg(fi.size())));
        return;
    }

    if (array.contains("MDTM")) {
        QFileInfo fi(currentDir, array.remove(0, 6));
        QString time = QString("%1 %2 %3").arg(month.value(fi.lastModified().date().month()) + " ")
                .arg(fi.lastModified().date().day())
                .arg(fi.lastModified().date().year());
        sendResponse(getResponse(200, time));
        return;
    }

    if (array.contains("RETR")) {
        QRegExp rx("^RETR\\s(.*)\\r\\n");
        rx.indexIn(array);
        QFile file (currentDir.path() + rx.cap(1));

        if(!file.open(QFile::ReadOnly))
        {
            sendResponse(getResponse(550));
            return;
        }

        while (!file.atEnd()) {
            QByteArray buffer = file.read(1024 * 8);
            readDataSock->write(buffer);
        }
        readDataSock->close();
        dataSocket->close();
        return;
    }

    if (array.contains("STOR")) {
        QRegExp rx("^STOR\\s(.*)\\r\\n");
        rx.indexIn(array);
        fileName = rx.cap(1);
        return;
    }

    if (array.contains("HELP")) {
        sendResponse(getResponse(214));
        return;
    }

    if (array.contains("OPTS")) {
        sendResponse(getResponse(200, "Ok"));
        return;
    }
}

void Ftp::uploadFile(){
    QFile file(fileName.remove(0,1));

    if(!file.open(QFile::Append))
    {
        sendResponse(getResponse(550));
        return;
    }
    QByteArray buffer = readDataSock->readAll();
    readDataSock->close();
    dataSocket->close();
    file.write(buffer);
    file.close();
}

void Ftp::readDataAtSock(){
    readDataSock = dataSocket->nextPendingConnection();
    connect(readDataSock, SIGNAL(readyRead()), this, SLOT(uploadFile()));

    QStringList list = getDirList(currentDir);

    foreach (QString str, list) {
        readDataSock->write(qPrintable(str));
    }
    readDataSock->write("\r\n");
    readDataSock->close();
    dataSocket->close();
    sendResponse(getResponse(226));
}

QStringList Ftp::getDirList(QDir dir){
    QStringList list;

    foreach (QFileInfo fi, dir.entryInfoList()) {
        QString pattern("%1 %2 %3 %4 %5 %6 %7\r\n");
        QString rights;

        if (fi.isDir()) {
            rights.append("d");
        } else {
            rights.append("-");
        }

        if (fi.isReadable()) {
            rights.append("r");
        } else {
            rights.append("-");
        }

        if (fi.isWritable()) {
            rights.append("w");
        } else {
            rights.append("-");
        }

        rights.append("-r--r--");

        QString links("1");

        QString owner = fi.owner();

        QString group = fi.group();

        QString size;
        if (fi.isDir()) {
            size = "0";
        } else {
            size = QString("%1").arg(fi.size());
        }

        QString time;

        if (fi.lastModified().daysTo(QDateTime::currentDateTime()) > 180) {
            time = QString("%1 %2 %3").arg(month.value(fi.lastModified().date().month()) + " ")
                                      .arg(fi.lastModified().date().day())
                                      .arg(fi.lastModified().date().year());
        } else {
            time = QString("%1 %2 %3:%4").arg(month.value(fi.lastModified().date().month()) + " ")
                                         .arg(fi.lastModified().date().day())
                                         .arg(fi.lastModified().time().hour())
                                         .arg(fi.lastModified().time().minute());
        }
        QString str = pattern.arg(rights)
               .arg(links)
               .arg(owner)
               .arg(group)
               .arg(size)
               .arg(time)
               .arg(fi.fileName());
        if (fi.fileName() != ".." && fi.fileName() != ".")
            list << str;
    }
    return list;
}

QString Ftp::currentIp(){
    QString address;
    QList< QHostAddress > addresses = QHostInfo::fromName( QHostInfo::localHostName() ).addresses();
    foreach ( const QHostAddress & a, addresses ){
        QString protocol = "???";
            switch ( a.protocol() ){
                case QAbstractSocket::IPv4Protocol:
                    protocol = "IPv4";
                break;
                case QAbstractSocket::IPv6Protocol:
                    protocol = "IPv6";
                break;
                case QAbstractSocket::UnknownNetworkLayerProtocol:
                    protocol = "err";
                break;
            }
            if(protocol == "IPv4"){
                address = a.toString();
                break;
            }
    }
    return address;
}
