#include "users.h"

Users *Users::aInstance = 0;

Users *Users::instance()
{
    if (!aInstance) aInstance = new Users();
    return aInstance;
}

void Users::destroy() {
   if (aInstance)
      delete aInstance, aInstance = 0;
}

Users::Users(){
    home = qApp->applicationDirPath();
    if (!QFile::exists(home + "/users.ini")) {
        QFile file(home + "/users.ini");
        file.open(QFile::ReadWrite);
        file.close();
    }
    qDebug() << "save Users to " << home + "/users.ini";
}

void Users::save(const QString &key, const QVariant &value){
    QSettings settings(home+"/users.ini", QSettings::IniFormat);
    settings.setValue(key, value);
}

QVariant Users::load(const QString &key, const QVariant &defaultValue){
    QSettings settings(home+"/users.ini", QSettings::IniFormat);
    return settings.value(key, defaultValue);
}

int Users::count(){
    QSettings settings(home+"/users.ini", QSettings::IniFormat);
    return settings.allKeys().count();
}

QStringList Users::allKeys(){
    QSettings settings(home+"/users.ini", QSettings::IniFormat);
    return settings.childGroups();
}

void Users::remove(const QString &key){
    QSettings settings(home+"/users.ini", QSettings::IniFormat);
    settings.remove(key);
}

bool Users::isUser(const QString &user){
    QStringList list = allKeys();

    foreach (QString str, list) {
        QString temp = load(tr("%1/name").arg(str), "empty").toString();

        if (temp == user) {
            return true;
        }
    }
    return false;
}

int Users::id(const QString &user){
    QStringList list = allKeys();

    foreach (QString str, list) {
        QString temp = load(tr("%1/name").arg(str), "empty").toString();

        if (temp == user) {
            return str.toInt();
        }
    }
    return -1;
}
