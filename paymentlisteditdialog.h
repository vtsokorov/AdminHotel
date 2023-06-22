#ifndef EQUIPMENTINPUTDIALOG_H
#define EQUIPMENTINPUTDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class PaymentListEditDialog;
}


class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QComboBox;

class PaymentListEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentListEditDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~PaymentListEditDialog();

public slots:

    int exec(const QModelIndex &index);
    int exec() Q_DECL_OVERRIDE;
    void clearWindow();
    void submit();
    void refresh();

private:

    void manualMapping();
    int relationIndex(QComboBox *widget);

private:
    Ui::PaymentListEditDialog *ui;
    QSqlRelationalTableModel *model;
    QDataWidgetMapper *mapper;
    QModelIndex modelIndex;
    int currentRow;
};

#endif // EQUIPMENTINPUTDIALOG_H
