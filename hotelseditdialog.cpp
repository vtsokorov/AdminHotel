#include "hotelseditdialog.h"
#include "ui_hotelseditdialog.h"

#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>

HotelsEditDialog::HotelsEditDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent), ui(new Ui::HotelsEditDialog), mapper(new QDataWidgetMapper(parent)),
    model(model), currentRow(0)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint |
                   Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->pbCancel, &QAbstractButton::clicked, this, &HotelsEditDialog::close);
    connect(ui->pbOk, &QAbstractButton::clicked, this, &HotelsEditDialog::submit);

    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->addMapping(ui->label_1, 0, "text");
    mapper->addMapping(ui->leField1, 1);
    mapper->addMapping(ui->leField2, 2);
    mapper->addMapping(ui->leField3, 3);
    mapper->addMapping(ui->leField4, 4);
    mapper->addMapping(ui->sbField5, 5);
}

HotelsEditDialog::~HotelsEditDialog()
{
    delete ui;
}

int HotelsEditDialog::exec(const QModelIndex &index)
{
     modelIndex = index;
     currentRow = modelIndex.row();
     mapper->setCurrentIndex(currentRow);
     return QDialog::exec();
}

int HotelsEditDialog::exec()
{
    this->clearWindow();
    currentRow = 0;
    mapper->setCurrentIndex(-1);
    return QDialog::exec();
}

void HotelsEditDialog::clearWindow()
{
    ui->label_1->clear();
    ui->leField1->clear();
    ui->leField2->clear();
    ui->leField3->clear();
    ui->leField4->clear();
    ui->sbField5->clear();
}

void HotelsEditDialog::refresh()
{
    QString filter = model->filter();
    model->setFilter(QString());
    model->select();
    model->setFilter(filter);
}

void HotelsEditDialog::submit()
{
    if(ui->leField1->text().isEmpty()){
       QMessageBox::warning(this, "Ошибка", "Необходимо заполнить обязательные поля (*)");
       return;
    }

    if(!modelIndex.isValid())
        if(!model->insertRow(currentRow))
            return;

    model->setData(model->index(currentRow, 1), ui->leField1->text());
    model->setData(model->index(currentRow, 2), ui->leField2->text());
    model->setData(model->index(currentRow, 3), ui->leField3->text());
    model->setData(model->index(currentRow, 4), ui->leField4->text());
    model->setData(model->index(currentRow, 5), ui->sbField5->text());

    if(model->submitAll())
        accept();
    else
    {
        QMessageBox::warning(this, "Ошибка", "Не удается сохранить данные");
        refresh();
    }

}

