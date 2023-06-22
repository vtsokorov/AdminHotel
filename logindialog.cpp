#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QCloseEvent>
#include <QSqlDatabase>
#include <QSqlError>

LoginDialog::LoginDialog(QSqlDatabase *db, QWidget *parent) :
    QDialog(parent), ui(new Ui::LoginDialog), db(db)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint |
                     Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->pbOk, &QAbstractButton::clicked, this, &LoginDialog::connection);
    connect(ui->pbCencel, &QAbstractButton::clicked, [](){exit(0);});

}

void LoginDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    exit(0);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

int LoginDialog::exec()
{
    if(!QFileInfo::exists("settings.ini")) {
        QMessageBox::critical(this, QObject::tr("База данных"),
                              QObject::tr("Файл настроек не найден."));
        exit(0);
    }
    else {

        QSettings settings("settings.ini", QSettings::IniFormat);
        settings.beginGroup("SETTINGS");
            host    = settings.value("host").toString();
            base    = settings.value("database").toString();
            port    = settings.value("port").toInt();
            user    = settings.value("user").toString();
            charset = settings.value("charset").toString();
            role    = settings.value("role").toString();
            option  = "CHARSET="+charset+";"+"ROLE="+role;
        settings.endGroup();
        ui->leUser->setText(user);
        //ui->lePassword->setText("masterkey");
    }
     return QDialog::exec();
}

void LoginDialog::connection()
{
    *db = QSqlDatabase::addDatabase("QIBASE");
    db->setHostName(host);
    db->setDatabaseName(base);
    db->setUserName(ui->leUser->text());
    db->setPassword(ui->lePassword->text());
    db->setPort(port);
    db->setConnectOptions(option);
    if(db->open()) accept();
    else
        QMessageBox::warning(this, QObject::tr("Ошибка подключения к базе данных"),
                             db->lastError().text());

}


