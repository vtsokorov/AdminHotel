#ifndef NUMBEREDSTOCKEDITDIALOG_H
#define NUMBEREDSTOCKEDITDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class NumberedStockEditDialog;
}


class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QComboBox;

class NumberedStockEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NumberedStockEditDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~NumberedStockEditDialog();

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
    Ui::NumberedStockEditDialog *ui;
    QSqlRelationalTableModel *model;
    QDataWidgetMapper *mapper;
    QModelIndex modelIndex;
    int currentRow;
};

#endif // NUMBEREDSTOCKEDITDIALOG_H
