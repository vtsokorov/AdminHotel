#include "numberedstockeditdialog.h"
#include "ui_numberedstockeditdialog.h"

#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>

NumberedStockEditDialog::NumberedStockEditDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent), ui(new Ui::NumberedStockEditDialog), model(model), mapper(new QDataWidgetMapper(parent)),
    currentRow(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint |
                   Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->pbCancel, &QAbstractButton::clicked, this, &NumberedStockEditDialog::close);
    connect(ui->pbOk, &QAbstractButton::clicked, this, &NumberedStockEditDialog::submit);

    ui->leField4->setValidator(new QRegularExpressionValidator(QRegularExpression("[.0-9]+"), this));

    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->addMapping(ui->label_1, 0, "text");

    ui->cbField1->setModel(model->relationModel(1));
    ui->cbField1->setModelColumn(1);
    mapper->addMapping(ui->cbField1, 1);
    mapper->addMapping(ui->leField2, 2);
    mapper->addMapping(ui->leField3, 3);
    mapper->addMapping(ui->leField4, 4);
}

NumberedStockEditDialog::~NumberedStockEditDialog()
{
    delete ui;
}

void NumberedStockEditDialog::manualMapping()
{
   ui->leField4->setText(QString::number(ui->leField4->text().toDouble(), 'f', 2));
}

int NumberedStockEditDialog::exec(const QModelIndex &index)
{
     modelIndex = index;
     currentRow = modelIndex.row();
     mapper->setCurrentIndex(currentRow);
     manualMapping();
     return QDialog::exec();
}

int NumberedStockEditDialog::exec()
{
    this->clearWindow();
    currentRow = 0;
    mapper->setCurrentIndex(-1);
    return QDialog::exec();
}

void NumberedStockEditDialog::clearWindow()
{
    ui->label_1->clear();
    ui->cbField1->setCurrentIndex(-1);
    ui->leField2->clear();
    ui->leField3->clear();
    ui->leField4->clear();
}

int NumberedStockEditDialog::relationIndex(QComboBox *widget)
{
    QSqlTableModel *m = qobject_cast<QSqlTableModel *>(widget->model());
    return m->data(m->index(widget->currentIndex(), 0)).toInt();
}

void NumberedStockEditDialog::refresh()
{
    QString filter = model->filter();
    model->setFilter(QString());
    model->select();
    model->setFilter(filter);
}

void NumberedStockEditDialog::submit()
{

    if(ui->leField4->text().toDouble() <= 0){
        QMessageBox::warning(this, "Ошибка", "Цена должна быть больше нуля");
        return;
    }

    if(ui->cbField1->currentIndex() == -1){
        QMessageBox::warning(this, "Ошибка", "Не указана связь между главной и дочерней таблицей");
        return;
    }

    if(ui->leField2->text().isEmpty() || ui->leField3->text().isEmpty()){
        QMessageBox::warning(this, "Ошибка", "Необходимо заполнить обязательные поля (*)");
        return;
     }

    if(!modelIndex.isValid())
        if(!model->insertRow(currentRow))
            return;

    model->setData(model->index(currentRow, 1), relationIndex(ui->cbField1));
    model->setData(model->index(currentRow, 2), ui->leField2->text());
    model->setData(model->index(currentRow, 3), ui->leField3->text());
    model->setData(model->index(currentRow, 4), ui->leField4->text());

    if(model->submitAll())
        accept();
    else{
        QMessageBox::warning(this, "Ошибка", "Не удается сохранить данные.\n"
                             "* Возможно введенный номер комнаты уже существует в выбранном отеле.");
         refresh();
    }

}


