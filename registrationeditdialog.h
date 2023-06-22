#ifndef REGISTRATIONEDITDIALOG_H
#define REGISTRATIONEDITDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class RegistrationEditDialog;
}

class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QComboBox;

class RegistrationEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationEditDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~RegistrationEditDialog();

public slots:

    int exec(const QModelIndex &index);
    int exec() Q_DECL_OVERRIDE;
    void clearWindow();
    void submit();
    void refresh();

    void initFilter(const QString& param);

private:

    int relationIndex(int row, int column);

private:
    Ui::RegistrationEditDialog *ui;
    QSqlRelationalTableModel *model;
    QDataWidgetMapper *mapper;
    QModelIndex modelIndex;
    int currentRow;
};

#endif // REGISTRATIONEDITDIALOG_H
