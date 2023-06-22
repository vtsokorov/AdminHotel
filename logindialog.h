#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class QSqlDatabase;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:

    explicit LoginDialog(QSqlDatabase *db, QWidget *parent = 0);
    ~LoginDialog();

protected:

    virtual void closeEvent(QCloseEvent *event);

private slots:

    void connection();

public slots:

    int exec() Q_DECL_OVERRIDE;

private:

    Ui::LoginDialog *ui;
    QSqlDatabase *db;

    QString host;
    QString base;
    QString user;
    QString charset;
    QString role;
    QString option;
    int port;

};

#endif // LOGINDIALOG_H
