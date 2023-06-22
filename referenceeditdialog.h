#ifndef REFERENCEEDITDIALOG_H
#define REFERENCEEDITDIALOG_H

#include <QDialog>
#include <QModelIndex>

class QDataWidgetMapper;
class QSqlRelationalTableModel;

namespace Ui {
class ReferenceEditDialog;
}

class ReferenceEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReferenceEditDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~ReferenceEditDialog();

public slots:

    int exec(const QModelIndex &index);
    int exec() Q_DECL_OVERRIDE;
    void clearWindow();
    void submit();
    void refresh();

private:

    Ui::ReferenceEditDialog *ui;
    QDataWidgetMapper *mapper;
    QSqlRelationalTableModel *model;
    QModelIndex modelIndex;
    int currentRow;
};

#endif // REFERENCEEDITDIALOG_H
