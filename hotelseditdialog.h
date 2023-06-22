#ifndef HOTELSEDITDIALOG_H
#define HOTELSEDITDIALOG_H

#include <QDialog>
#include <QModelIndex>

class QDataWidgetMapper;
class QSqlRelationalTableModel;

namespace Ui {
    class HotelsEditDialog;
}

class HotelsEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HotelsEditDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~HotelsEditDialog();

public slots:

    int exec(const QModelIndex &index);
    int exec() Q_DECL_OVERRIDE;
    void clearWindow();
    void submit();
    void refresh();

private:
    Ui::HotelsEditDialog *ui;
    QDataWidgetMapper *mapper;
    QSqlRelationalTableModel *model;
    QModelIndex modelIndex;
    int currentRow;
};

#endif // HOTELSEDITDIALOG_H
