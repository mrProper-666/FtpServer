#ifndef USERS_H
#define USERS_H

#include <QObject>
#include <QString>
#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDebug>

class Users : public QObject
{
    Q_OBJECT
public:
    static Users *instance();
    static void destroy();
    void save(const QString &key, const QVariant &value);
    QVariant load(const QString &key, const QVariant &defaultValue);
    int count();
    QStringList allKeys();
    void remove(const QString &key);
    bool isUser(const QString &user);
    int id(const QString &user);

private:
    Users();
    static Users *aInstance;

    QString home;

signals:

public slots:

};

#endif // USERS_H
