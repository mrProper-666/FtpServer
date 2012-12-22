#include "pages.h"

 AccountPage::AccountPage(QWidget *parent)
     : QWidget(parent)
 {
    table = new QTableWidget(this);

    updateTable();
    addUserWidget();

    QPushButton *addBtn = new QPushButton("Добавить", this);
    QPushButton *rmBtn = new QPushButton("Удалить", this);

    connect(addBtn,SIGNAL(clicked()), widgetUser, SLOT(show()));
    connect(rmBtn, SIGNAL(clicked()), this, SLOT(removeUser()));

    QHBoxLayout *hBox = new QHBoxLayout;
    hBox->addStretch(1);
    hBox->addWidget(addBtn);
    hBox->addWidget(rmBtn);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(table);
    mainLayout->addLayout(hBox);
    setLayout(mainLayout);

    hash = new QCryptographicHash(QCryptographicHash::Md5);
 }

void AccountPage::updateTable(){
    table->clear();
    table->setRowCount(0);
    table->setColumnCount(2);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList list;
    list << tr("Имя") << tr("Пароль");
    table->setHorizontalHeaderLabels(list);

    foreach (QString str, Users::instance()->allKeys()){
        table->insertRow(table->rowCount());
        QTableWidgetItem *item = new QTableWidgetItem(Users::instance()->load(str + "/name", "").toString());
        table->setItem(table->rowCount() - 1, 0, item);
        QTableWidgetItem *item2 = new QTableWidgetItem(Users::instance()->load(str + "/pass", "").toString());
        table->setItem(table->rowCount() - 1, 1, item2);
    }
 }

void AccountPage::addUserWidget(){
    widgetUser = new QWidget();
    widgetUser->setWindowModality(Qt::ApplicationModal);
    widgetUser->setWindowFlags(Qt::WindowCloseButtonHint);
    widgetUser->setWindowTitle(qApp->applicationName() + " | Добавить пользователя");

    QVBoxLayout *vBoxLay = new QVBoxLayout(widgetUser);
    QHBoxLayout *hBoxName = new QHBoxLayout(vBoxLay->widget());
    QHBoxLayout *hBoxPas = new QHBoxLayout(vBoxLay->widget());
    QHBoxLayout *hBoxBtns = new QHBoxLayout(vBoxLay->widget());

    QLabel *nameLbl = new QLabel(tr("Имя :"), hBoxName->widget());
    QLabel *pasLbl = new QLabel(tr("Пароль :"), hBoxPas->widget());

    nameEdit = new QLineEdit(hBoxName->widget());
    pasEdit = new QLineEdit(hBoxPas->widget());

    QPushButton *okBtn = new QPushButton(tr("Добавить"), hBoxBtns->widget());
    QPushButton *cancelBtn = new QPushButton(tr("Отменить"), hBoxBtns->widget());

    hBoxBtns->addStretch(1);
    hBoxBtns->addWidget(okBtn);
    hBoxBtns->addWidget(cancelBtn);

    hBoxPas->addWidget(pasLbl);
    hBoxPas->addWidget(pasEdit);

    hBoxName->addWidget(nameLbl);
    hBoxName->addWidget(nameEdit);

    vBoxLay->addLayout(hBoxName);
    vBoxLay->addLayout(hBoxPas);
    vBoxLay->addStretch(1);
    vBoxLay->addLayout(hBoxBtns);

    connect(cancelBtn, SIGNAL(clicked()), widgetUser, SLOT(close()));
    connect(okBtn, SIGNAL(clicked()), this, SLOT(applyAdd()));
}

void AccountPage::applyAdd(){
    Users::instance()->save(tr("%1/name").arg(table->rowCount() - 1), nameEdit->text());
    QByteArray arr;
    arr.append(pasEdit->text());
    hash->addData(arr);
    Users::instance()->save(tr("%1/pass").arg(table->rowCount() - 1), QString(hash->result()));
    hash->reset();
    updateTable();
    widgetUser->close();
}

void AccountPage::removeUser(){
    if (!table->selectedItems().isEmpty()) {
        Users::instance()->remove(
                    Users::instance()->allKeys().at(table->selectedItems().first()->row()));
        updateTable();
    }
}

FolderPage::FolderPage(QWidget *parent)
     : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout(mainLayout->widget());

    QSettings set(qApp->organizationName(),qApp->applicationName());
    QString path = set.value(tr("path"), tr("C:/")).toString();
    edit = new QLineEdit(path, hBox->widget());
    edit->setReadOnly(true);
    QPushButton *btn = new QPushButton(tr("Выбрать"), hBox->widget());
    connect(btn, SIGNAL(clicked()), this, SLOT(setPath()));

    hBox->addWidget(edit);
    hBox->addWidget(btn);

    mainLayout->addLayout(hBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void FolderPage::setPath(){
    QString dir = QFileDialog::getExistingDirectory(this, qApp->applicationName() + " | Выбрать папку",
                                                    tr("C:/"));
    edit->setText(dir);
    QSettings set(qApp->organizationName(),qApp->applicationName());
    set.setValue(tr("path"), dir);
}

LogPage::LogPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    button = new QPushButton(tr("Очистить"));
    hLayout->addStretch(1);
    hLayout->addWidget(button);
    edit = new QTextEdit;
    edit->setReadOnly(true);
    vLayout->addWidget(edit);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
}

AboutPage::AboutPage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *lblPic = new QLabel(mainLayout->widget());
    lblPic->setPixmap(QPixmap(":/resources/logo.png"));
    lblPic->setAlignment(Qt::AlignHCenter);

    QLabel *lblText = new QLabel(tr("Ftp сервер разработан в ходе выполнения\n"
                                    "лабораторной работы №4 по КИС"),
                                 mainLayout->widget());
    lblText->setFont(QFont(tr("Arial"), 15, 70));
    lblText->setAlignment(Qt::AlignHCenter);

    mainLayout->addWidget(lblPic);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(lblText);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}
