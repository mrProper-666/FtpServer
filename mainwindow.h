#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QListWidgetItem>
#include <QtDebug>
#include <QHostAddress>

#include "pages.h"
#include "log.h"
#include "ftp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    void createLPanel();
    Log *cLog;
    Ftp *cFtp;

    AccountPage *accountWidget;
    FolderPage *folderWidget;
    LogPage *logWidget;
    AboutPage *aboutWidget;

private slots:
    void changePage(QListWidgetItem *, QListWidgetItem *);
    void on_startServerAct_triggered();
    void on_stopServerAct_triggered();

public slots:
    void eventHandler(QString str);
};

#endif // MAINWINDOW_H
