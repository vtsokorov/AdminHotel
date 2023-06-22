#include "referenceeditdialog.h"
#include "ui_referenceeditdialog.h"

#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>

ReferenceEditDialog::ReferenceEditDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent), ui(new Ui::ReferenceEditDialog), mapper(new QDataWidgetMapper(parent)),
    model(model), currentRow(0)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint |
                   Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->pbCancel, &QAbstractButton::clicked, this, &ReferenceEditDialog::close);
    connect(ui->pbOk, &QAbstractButton::clicked, this, &ReferenceEditDialog::submit);

    if(model->tableName() == "STATUS_ROOM") {
        ui->label_2->setText("* Статус комнаты:");
    }
    if(model->tableName() == "PAYMENT_TYPE") {
        ui->label_2->setText("* Вид платежа: ");
    }


    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->addMapping(ui->label_1, 0, "text");
    mapper->addMapping(ui->leField1, 1);
}

ReferenceEditDialog::~ReferenceEditDialog()
{
    delete ui;
}

int ReferenceEditDialog::exec(const QModelIndex &index)
{
     modelIndex = index;
     currentRow = modelIndex.row();
     mapper->setCurrentIndex(currentRow);
     return QDialog::exec();
}

int ReferenceEditDialog::exec()
{
    this->clearWindow();
    currentRow = 0;
    mapper->setCurrentIndex(-1);
    return QDialog::exec();
}

void ReferenceEditDialog::clearWindow()
{
    ui->label_1->clear();
    ui->leField1->clear();
}

void ReferenceEditDialog::refresh()
{
    QString filter = model->filter();
    model->setFilter(QString());
    model->select();
    model->setFilter(filter);
}

void ReferenceEditDialog::submit()
{
    if(!modelIndex.isValid())
        if(!model->insertRow(currentRow))
            return;

    model->setData(model->index(currentRow, 1), ui->leField1->text());

    if(model->submitAll())
        accept();
    else{
        QMessageBox::warning(this, "Ошибка", "Не удается сохранить данные");
        refresh();
    }

}

