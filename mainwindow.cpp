#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stopServerAct->setEnabled(false);

    setWindowTitle(qApp->applicationName() + ' ' + qApp->applicationVersion());
    setContextMenuPolicy(Qt::NoContextMenu);
    createLPanel();

    cLog = new Log;
    connect(cLog, SIGNAL(appendLog(QString)), logWidget->edit, SLOT(append(QString)));
    connect(logWidget->button, SIGNAL(clicked()), logWidget->edit, SLOT(clear()));

    cFtp = new Ftp;
    connect(cFtp, SIGNAL(onError(QString)), this, SLOT(eventHandler(QString)));
    connect(cFtp, SIGNAL(onStarted(QString)), this, SLOT(eventHandler(QString)));
    connect(cFtp, SIGNAL(onClose(QString)), this, SLOT(eventHandler(QString)));
    connect(cFtp, SIGNAL(onEvent(QString)), this, SLOT(eventHandler(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createLPanel(){
    accountWidget = new AccountPage;
    folderWidget = new FolderPage;
    logWidget = new LogPage;
    aboutWidget = new AboutPage;

    ui->lPanelWidget->setViewMode(QListView::IconMode);
    ui->lPanelWidget->setMovement(QListView::Static);
    ui->lPanelWidget->setSpacing(8);

    ui->rPanelWidget->removeWidget(ui->rPanelWidget->widget(1));
    ui->rPanelWidget->removeWidget(ui->rPanelWidget->widget(0));
    ui->rPanelWidget->addWidget(accountWidget);
    ui->rPanelWidget->addWidget(folderWidget);
    ui->rPanelWidget->addWidget(logWidget);
    ui->rPanelWidget->addWidget(aboutWidget);

    QListWidgetItem *account = new QListWidgetItem(ui->lPanelWidget);
    account->setIcon(QIcon(":/resources/account.png"));
    account->setText(tr("Пользователи"));
    account->setTextAlignment(Qt::AlignHCenter);
    account->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *folder = new QListWidgetItem(ui->lPanelWidget);
    folder->setIcon(QIcon(":/resources/folder.png"));
    folder->setText(tr("Расшаренная \nпапка"));
    folder->setTextAlignment(Qt::AlignHCenter);
    folder->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *logging = new QListWidgetItem(ui->lPanelWidget);
    logging->setIcon(QIcon(":/resources/log.png"));
    logging->setText(tr("Логирование"));
    logging->setTextAlignment(Qt::AlignHCenter);
    logging->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *about = new QListWidgetItem(ui->lPanelWidget);
    about->setIcon(QIcon(":/resources/info.png"));
    about->setText(tr("О программе..."));
    about->setTextAlignment(Qt::AlignHCenter);
    about->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(ui->lPanelWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

void MainWindow::changePage(QListWidgetItem *current, QListWidgetItem *previous){
    if (!current)
        current = previous;

    ui->rPanelWidget->setCurrentIndex(ui->lPanelWidget->row(current));
}

void MainWindow::on_startServerAct_triggered()
{
    cFtp->startServer(QHostAddress::Any, 21);
    ui->startServerAct->setEnabled(false);
    ui->stopServerAct->setEnabled(true);
}

void MainWindow::on_stopServerAct_triggered()
{
    if (cFtp->status() != STOPED) {
        cFtp->stopServer();
        ui->stopServerAct->setEnabled(false);
        ui->startServerAct->setEnabled(true);
    }
}

void MainWindow::eventHandler(QString str){
    logWidget->edit->append(str);
}
