#ifndef PAGES_H
#define PAGES_H

#include <QWidget>
#include <QtGui>
#include <QPushButton>
#include <QTextEdit>
#include <QCryptographicHash>
#include <QFileDialog>

#include "users.h"

class AccountPage : public QWidget
{
    Q_OBJECT
public:
    AccountPage(QWidget *parent = 0);

private:
    QTableWidget *table;
    QWidget *widgetUser;
    QLineEdit *nameEdit;
    QLineEdit *pasEdit;
    QCryptographicHash *hash;

    void updateTable();
    void addUserWidget();

private slots:
     void applyAdd();
     void removeUser();
};

class FolderPage : public QWidget
{
    Q_OBJECT
public:
    FolderPage(QWidget *parent = 0);

private:
    QLineEdit *edit;

private slots:
    void setPath();
};

class LogPage : public QWidget
{
    Q_OBJECT
public:
    LogPage(QWidget *parent = 0);

    QPushButton *button;
    QTextEdit *edit;
};

class AboutPage : public QWidget
{
    Q_OBJECT
public:
    AboutPage(QWidget *parent = 0);
};

#endif
