#include "paymentlisteditdialog.h"
#include "ui_paymentlisteditdialog.h"

#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>

PaymentListEditDialog::PaymentListEditDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent), ui(new Ui::PaymentListEditDialog), model(model), mapper(new QDataWidgetMapper(parent)),
    currentRow(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint |
                   Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->pbCancel, &QAbstractButton::clicked, this, &PaymentListEditDialog::close);
    connect(ui->pbOk, &QAbstractButton::clicked, this, &PaymentListEditDialog::submit);

    ui->leField3->setValidator(new QRegularExpressionValidator(QRegularExpression("[.0-9]+"), this));
    ui->leField4->setValidator(new QRegularExpressionValidator(QRegularExpression("[.0-9]+"), this));

    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->addMapping(ui->label_1, 0, "text");

    ui->cbField1->setModel(model->relationModel(1));
    ui->cbField1->setModelColumn(4);
    mapper->addMapping(ui->cbField1, 1);

    ui->cbField2->setModel(model->relationModel(2));
    ui->cbField2->setModelColumn(1);
    mapper->addMapping(ui->cbField2, 2);

    mapper->addMapping(ui->leField3, 3);
    mapper->addMapping(ui->leField4, 4);

    mapper->addMapping(ui->deField5, 5);
}

PaymentListEditDialog::~PaymentListEditDialog()
{
    delete ui;
}

void PaymentListEditDialog::manualMapping()
{
   ui->leField3->setText(QString::number(ui->leField3->text().toDouble(), 'f', 2));
   ui->leField4->setText(QString::number(ui->leField4->text().toDouble(), 'f', 2));
}

int PaymentListEditDialog::exec(const QModelIndex &index)
{
     modelIndex = index;
     currentRow = modelIndex.row();
     mapper->setCurrentIndex(currentRow);
     manualMapping();
     return QDialog::exec();
}

int PaymentListEditDialog::exec()
{
    this->clearWindow();
    currentRow = 0;
    mapper->setCurrentIndex(-1);
    return QDialog::exec();
}

void PaymentListEditDialog::clearWindow()
{
    ui->label_1->clear();

    ui->cbField1->setCurrentIndex(-1);
    ui->cbField2->setCurrentIndex(-1);

    ui->leField3->clear();
    ui->leField4->clear();
    ui->deField5->clear();

    ui->deField5->setDate(QDate::currentDate());
}

int PaymentListEditDialog::relationIndex(QComboBox *widget)
{
    QSqlTableModel *m = qobject_cast<QSqlTableModel *>(widget->model());
    return m->data(m->index(widget->currentIndex(), 0)).toInt();
}

void PaymentListEditDialog::refresh()
{
    QString filter = model->filter();
    model->setFilter(QString());
    model->select();
    model->setFilter(filter);
}

void PaymentListEditDialog::submit()
{
    if(ui->cbField1->currentIndex() == -1 || ui->cbField2->currentIndex() == -1){
        QMessageBox::warning(this, "Ошибка", "Не указана связь между главной и дочерней таблицей");
        return;
    }

    if(!modelIndex.isValid())
        if(!model->insertRow(currentRow))
            return;

    model->setData(model->index(currentRow, 1), relationIndex(ui->cbField1));
    model->setData(model->index(currentRow, 2), relationIndex(ui->cbField2));
    model->setData(model->index(currentRow, 3), ui->leField3->text());
    model->setData(model->index(currentRow, 4), ui->leField4->text());
    model->setData(model->index(currentRow, 5), QDate::fromString(ui->deField5->text(), "dd.MM.yyyy"));


    if(model->submitAll())
        accept();
    else{
        QMessageBox::warning(this, "Ошибка", "Не удается сохранить данные.\n"
                                             "* Возможно введенная сумма превышает возможную.");
         refresh();
    }

}


