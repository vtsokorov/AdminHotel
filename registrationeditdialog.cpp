#include "registrationeditdialog.h"
#include "ui_registrationeditdialog.h"

#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>
#include <QDebug>

#include "querys.h"

RegistrationEditDialog::RegistrationEditDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent), ui(new Ui::RegistrationEditDialog), mapper(new QDataWidgetMapper(parent)),
    model(model), currentRow(0)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint |
                   Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->pbCancel, &QAbstractButton::clicked, this, &RegistrationEditDialog::close);
    connect(ui->pbOk, &QAbstractButton::clicked, this, &RegistrationEditDialog::submit);
    connect(ui->cbField1, SIGNAL(currentTextChanged(QString)), this, SLOT(initFilter(QString)));

    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->addMapping(ui->label_1, 0, "text");

    ui->cbField1->setModel(model->relationModel(1));
    ui->cbField1->setModelColumn(1);
    mapper->addMapping(ui->cbField1, 1);

    ui->cbField2->setModel(model->relationModel(2));
    ui->cbField2->setModelColumn(2);
    mapper->addMapping(ui->cbField2, 2);

    ui->cbField3->setModel(model->relationModel(3));
    ui->cbField3->setModelColumn(1);
    mapper->addMapping(ui->cbField3, 3);

    mapper->addMapping(ui->leField4, 4);
    mapper->addMapping(ui->leField5, 5);
    mapper->addMapping(ui->leField6, 6);

    mapper->addMapping(ui->deField7, 7);
    mapper->addMapping(ui->deField8, 8);
    mapper->addMapping(ui->teField9, 9);


}

RegistrationEditDialog::~RegistrationEditDialog()
{
    delete ui;
}

int RegistrationEditDialog::exec(const QModelIndex &index)
{
     modelIndex = index;
     currentRow = modelIndex.row();
     mapper->setCurrentIndex(currentRow);
     return QDialog::exec();
}

int RegistrationEditDialog::exec()
{
    this->clearWindow();
    currentRow = 0;
    mapper->setCurrentIndex(-1);
    return QDialog::exec();
}

void RegistrationEditDialog::clearWindow()
{
    ui->label_1->clear();

    ui->leField4->clear();
    ui->leField5->clear();
    ui->leField6->clear();

    ui->deField7->clear();
    ui->deField8->clear();

    ui->teField9->clear();

    ui->cbField1->setCurrentIndex(-1);
    ui->cbField2->setCurrentIndex(-1);
    ui->cbField3->setCurrentIndex(-1);

    ui->deField7->setDate(QDate::currentDate());
    ui->deField8->setDate(QDate::currentDate());
}

int RegistrationEditDialog::relationIndex(int row, int column)
{
    QSqlTableModel *m = model->relationModel(column);
    return m->data(m->index(row, 0)).toInt();
}
void RegistrationEditDialog::refresh()
{
    QString filter = model->filter();
    model->setFilter(QString());
    model->select();
    model->setFilter(filter);
}

void RegistrationEditDialog::initFilter(const QString& param)
{
    if(!param.isEmpty())
    {
        int index = relationIndex(ui->cbField1->currentIndex(), 1);
        QSqlTableModel *m = model->relationModel(2);
        m->setFilter(QString("F_ID_HOTEL LIKE '%%0%'").arg(index));
        ui->cbField2->setModel(m);
        ui->cbField2->setModelColumn(2);
    }
}

void RegistrationEditDialog::submit()
{
    if(ui->cbField1->currentIndex() == -1 ||
            ui->cbField2->currentIndex() == -1 ||
            ui->cbField3->currentIndex() == -1){
        QMessageBox::warning(this, "Ошибка", "Не указана связь между главной и дочерней таблицей");
        return;
    }

    if(ui->leField4->text().isEmpty()){
       QMessageBox::warning(this, "Ошибка", "Необходимо заполнить обязательные поля (*)");
       return;
    }

    if(ui->deField7->date() >= ui->deField8->date()){
       QMessageBox::warning(this, "Ошибка", "Некорректно заполнены обязательные (*) поля\n"
                                            "\"Дата въезда\" \\ \"Дата выезда\"");
       return;
    }

    int hotelId  = relationIndex(ui->cbField1->currentIndex(), 1);
    int stockId  = relationIndex(ui->cbField2->currentIndex(), 2);
    int statusId = relationIndex(ui->cbField3->currentIndex(), 3);
    QString query = Querys::intersection.arg(stockId)
            .arg(ui->deField7->date().toString("dd.MM.yyyy"))
            .arg(ui->deField8->date().toString("dd.MM.yyyy"));

    if(!modelIndex.isValid())
        if(!model->insertRow(currentRow))
            return;

    model->setData(model->index(currentRow, 1), hotelId);
    model->setData(model->index(currentRow, 2), stockId);
    model->setData(model->index(currentRow, 3), statusId);

    model->setData(model->index(currentRow, 4), ui->leField4->text());
    model->setData(model->index(currentRow, 5), ui->leField5->text());
    model->setData(model->index(currentRow, 6), ui->leField6->text());

    model->setData(model->index(currentRow, 7), ui->deField7->dateTime());
    model->setData(model->index(currentRow, 8), ui->deField8->dateTime());
    model->setData(model->index(currentRow, 9), ui->teField9->toPlainText());

    if(model->submitAll()) accept();
    else{
        QMessageBox::warning(this, "Ошибка", "Не удается сохранить данные.\n"
                                             "* Возможно указанная комната занята.");
        refresh();
    }
}
