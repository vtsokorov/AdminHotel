#include "applicationmodel.h"
#include "lrreportengine.h"
#include "querys.h"
#include "reports.h"
#include "generalreportmodel.h"

#include "hotelseditdialog.h"
#include "numberedstockeditdialog.h"
#include "registrationeditdialog.h"
#include "paymentlisteditdialog.h"
#include "referenceeditdialog.h"

#include <QLineEdit>
#include <QTableView>
#include <QStackedWidget>
#include <QToolBar>
#include <QDateEdit>

#include <QDate>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlQuery>
#include <QDebug>

#include <QMessageBox>
#include <QInputDialog>
#include <QProgressDialog>
#include <QPushButton>
#include <QApplication>


AbstractApplicationModel::AbstractApplicationModel(QComboBox *comboBox, QTableView *table,
                                                   QStackedWidget *stackedWidget, QSqlDatabase *db,
                                                   QObject *parent):
    QObject(parent), hotelSelect(comboBox), table(table), stackedWidget(stackedWidget),
    report(new LimeReport::ReportEngine(parent)),m_progressDialog(nullptr),
    m_currentPage(0), currentHotel(QString())
{
    model = new QSqlRelationalTableModel(parent, *db);
    hotelModel = new QSqlTableModel(parent, *db);
    hotelModel->setTable("HOTELS");
    hotelModel->select();
    int index = hotelSelect->currentIndex();
    hotelSelect->setModel(hotelModel);
    hotelSelect->setModelColumn(1);
    hotelSelect->setCurrentIndex(index);
}

AbstractApplicationModel::~AbstractApplicationModel()
{
    delete hotelModel;
    delete model;
}

void AbstractApplicationModel::showDialog(const QModelIndex& index, QWidget *parent)
{
   qDebug() <<"do nothing";
}

void AbstractApplicationModel::showReport(int index, const QString& param)
{
    qDebug() <<"do nothing";
}

void AbstractApplicationModel::execQuery(int index, const QString& query)
{
    qDebug() <<"do nothing";
}

void AbstractApplicationModel::setFilter(const QString& filer)
{
    qDebug() <<"do nothing";
}

QString AbstractApplicationModel::setCurrentHotel(const QString &hotel)
{
    currentHotel = hotel;
    QString select = QString("HOTEL_NAME LIKE '%%0%'").arg(currentHotel);
    model->setFilter(select);
    return select;
}

void AbstractApplicationModel::setPlaceHolderText(QLineEdit *widget)
{
    widget->setPlaceholderText(QString());
}

void AbstractApplicationModel::deleteRecord()
{
    QItemSelectionModel *select = table->selectionModel();
    if(select->hasSelection()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(0, "Вопрос", "Вы действительно хотите удалить запись?", QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            model->removeRow(select->selectedRows().first().row());
            model->submitAll();
            refresh();
        }
    }
    else
       QMessageBox::information(0, "Инфо", "Укажите запись!");
}

void AbstractApplicationModel::refresh()
{
    QString currentFilter = model->filter();
    model->setFilter(QString());
    model->select();
    model->setFilter(currentFilter);
}

void AbstractApplicationModel::toArchive()
{
     qDebug() <<"do nothing";
}

void AbstractApplicationModel::clearFilter()
{
    model->setFilter(QString());
    hotelSelect->setCurrentIndex(-1);
    currentHotel = QString();
}

void AbstractApplicationModel::setReportDateWidgets(QDateEdit *widget1, QDateEdit *widget2)
{
    rptFirstDate = widget1;
    rptSeconDate = widget2;
}

void AbstractApplicationModel::showAllColumn(int columnsSize)
{
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    for(int i = 0; i < columnsSize; ++i)
        table->showColumn(i);
}

void AbstractApplicationModel::selectTable(const QString &tableName)
{
    model->setTable(tableName);
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
}

QDate AbstractApplicationModel::inputDate(const QString &title, bool &isOk)
{
  return QDate::fromString(QInputDialog::getText(table, tr("Ввод даты"),
            QString("%0 (дата в формате dd.MM.yyyy):").arg(title), QLineEdit::Normal,
              "01.01.2017", &isOk), "dd.MM.yyyy");
}

QString AbstractApplicationModel::inputData(const QString &title, bool &isOk)
{
    return QInputDialog::getText(table, tr("Ввод параметра запроса"),
              title, QLineEdit::Normal, "", &isOk);
}

QString AbstractApplicationModel::selectData(const QString &title, const QStringList &items, bool &isOk)
{
    return QInputDialog::getItem(table, tr("Ввод параметра запроса"),
                                             title, items, 0, false, &isOk);
}

void AbstractApplicationModel::renderStarted()
{
    m_currentPage = 0;
    m_progressDialog = new QProgressDialog(tr("Start render"),tr("Cancel"), 0, 0, 0);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    connect(m_progressDialog, SIGNAL(canceled()), report, SLOT(cancelRender()));
    m_progressDialog->show();
    QApplication::processEvents();
}

void AbstractApplicationModel::renderPageFinished(int renderedPageCount)
{
    if (m_progressDialog){
        m_progressDialog->setLabelText(QString::number(renderedPageCount)+tr(" page rendered"));
        m_progressDialog->setValue(renderedPageCount);
    }
}

void AbstractApplicationModel::renderFinished()
{
    if (m_progressDialog){
        m_progressDialog->close();
        delete m_progressDialog;
    }
    m_progressDialog = 0;
}

////-------------------------

ReferenceObject::ReferenceObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* ReferenceObject::viewAdapter(QToolBar *toolBar)
{
    table->setModel(model);
    stackedWidget->setCurrentIndex(1);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(true);
    toolBar->actions().at(1)->setDisabled(true);
    toolBar->actions().at(2)->setDisabled(true);
    toolBar->actions().at(3)->setDisabled(true);
    toolBar->actions().at(4)->setDisabled(true); 
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(true);
    toolBar->actions().at(8)->setDisabled(true);

    toolBar->actions().at(12)->setDisabled(true);
    toolBar->actions().at(13)->setDisabled(true);

    return model;
}

////-------------------------

ArchiveObject::ArchiveObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* ArchiveObject::viewAdapter(QToolBar *toolBar)
{
    table->setModel(model);
    stackedWidget->setCurrentIndex(2);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(true);
    toolBar->actions().at(1)->setDisabled(true);
    toolBar->actions().at(2)->setDisabled(true);
    toolBar->actions().at(3)->setDisabled(true);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(true);
    toolBar->actions().at(8)->setDisabled(true);

    toolBar->actions().at(12)->setDisabled(true);
    toolBar->actions().at(13)->setDisabled(true);

    return model;
}

////-------------------------

HotelsObject::HotelsObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* HotelsObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("HOTELS");
    model->select();
    showAllColumn(model->columnCount());
    table->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, "№");
    model->setHeaderData(1, Qt::Horizontal, "Название отеля");
    model->setHeaderData(2, Qt::Horizontal, "Адрес");
    model->setHeaderData(3, Qt::Horizontal, "Телефон");
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 250);
    table->setColumnWidth(2, 200);
    table->setColumnWidth(3, 110);
    table->hideColumn(4);
    table->hideColumn(5);
    stackedWidget->setCurrentIndex(0);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(false);
    toolBar->actions().at(1)->setDisabled(false);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(false);

    toolBar->actions().at(12)->setDisabled(false);
    toolBar->actions().at(13)->setDisabled(false);

    return model;
}

void HotelsObject::showDialog(const QModelIndex& index, QWidget *parent)
{
    HotelsEditDialog dlg(model, parent);
    if(index.isValid()){
        dlg.exec(index);
        hotelModel->select();
    }
    else
        dlg.exec();
}

void HotelsObject::setFilter(const QString& filter)
{
    if(filter.isEmpty()){
        setCurrentHotel(currentHotel);
        return;
    }

    QString address = QString("ADDRESS LIKE '%%0%'").arg(filter);
    if(!currentHotel.isEmpty())
    {
        QString name = QString("HOTEL_NAME LIKE '%%0%'").arg(currentHotel);
        QString result = name + " and " + address;
        model->setFilter(result);
    }
    else
        model->setFilter(address);
}

void HotelsObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->show();
   widget->setPlaceholderText("Фильтер по адресу отеля");
}

////-------------------------

NumberedStockObject::NumberedStockObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* NumberedStockObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("NUMBERED_STOCK");
    model->setRelation(1, QSqlRelation("HOTELS", "P_ID_HOTEL", "HOTEL_NAME"));
    model->select();
    showAllColumn(model->columnCount());
    table->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, "№");
    model->setHeaderData(1, Qt::Horizontal, "Отель");
    model->setHeaderData(2, Qt::Horizontal, "Номер комнаты");
    model->setHeaderData(3, Qt::Horizontal, "Количество комнат");
    model->setHeaderData(4, Qt::Horizontal, "Цена");
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 200);
    table->setColumnWidth(2, 120);
    table->setColumnWidth(3, 120);
    table->setColumnWidth(4, 75);
    table->hideColumn(5);
    stackedWidget->setCurrentIndex(0);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(false);
    toolBar->actions().at(1)->setDisabled(false);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(false);

    toolBar->actions().at(12)->setDisabled(false);
    toolBar->actions().at(13)->setDisabled(false);

    return model;
}

void NumberedStockObject::showDialog(const QModelIndex& index, QWidget *parent)
{
    NumberedStockEditDialog dlg(model, parent);
    if(index.isValid())
        dlg.exec(index);
    else
        dlg.exec();
}

void NumberedStockObject::setFilter(const QString& filter)
{
    if(filter.isEmpty()){
        setCurrentHotel(currentHotel);
        return;
    }

   QString amount = QString("ROOM_AMOUNT LIKE '%%0%'").arg(filter);
   if(!currentHotel.isEmpty())
   {
       QString name = QString("HOTEL_NAME LIKE '%%0%'").arg(currentHotel);
       QString result = name + " and " + amount;
       model->setFilter(result);
   }
   else
       model->setFilter(amount);
}

void NumberedStockObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->show();
   widget->setPlaceholderText("Фильтер по количеству комнат");
}

////-------------------------

RegistrationObject::RegistrationObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* RegistrationObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("REGISTRATION");
    model->setRelation(1, QSqlRelation("HOTELS", "P_ID_HOTEL", "HOTEL_NAME"));
    model->setRelation(2, QSqlRelation("NUMBERED_STOCK", "P_ID_NUMBERED_STOCK", "ROOM_NUMBER"));
    model->setRelation(3, QSqlRelation("STATUS_ROOM", "P_ID_STATUS_ROOM", "STATUS"));
    model->select();
    showAllColumn(model->columnCount());
    table->setModel(model);

    model->setHeaderData(0, Qt::Horizontal, "№");
    model->setHeaderData(1, Qt::Horizontal, "Отель");
    model->setHeaderData(2, Qt::Horizontal, "Номер комнаты");
    model->setHeaderData(3, Qt::Horizontal, "Статус");
    model->setHeaderData(4, Qt::Horizontal, "ФИО");
    model->setHeaderData(7, Qt::Horizontal, "Дата приезда");
    model->setHeaderData(8, Qt::Horizontal, "Дата отьезда");
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 120);
    table->setColumnWidth(2, 120);
    table->setColumnWidth(3, 100);
    table->setColumnWidth(4, 150);
    table->setColumnWidth(7, 120);
    table->setColumnWidth(8, 120);
    table->hideColumn(5);
    table->hideColumn(6);
    table->hideColumn(9);
    stackedWidget->setCurrentIndex(0);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(false);
    toolBar->actions().at(1)->setDisabled(false);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);
    toolBar->actions().at(4)->setDisabled(false);
    toolBar->actions().at(5)->setDisabled(false);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(false);

    toolBar->actions().at(12)->setDisabled(false);
    toolBar->actions().at(13)->setDisabled(false);

    return model;
}

void RegistrationObject::showDialog(const QModelIndex& index, QWidget *parent)
{
    RegistrationEditDialog dlg(model, parent);
    if(index.isValid())
        dlg.exec(index);
    else
        dlg.exec();
}

void RegistrationObject::setFilter(const QString& filter)
{  
    if(filter.isEmpty()){
        setCurrentHotel(currentHotel);
        return;
    }

    QString resident = QString("RESIDENT_NAME LIKE '%%0%'").arg(filter);
    if(!currentHotel.isEmpty())
    {
        QString name = QString("HOTEL_NAME LIKE '%%0%'").arg(currentHotel);
        QString result = name + " and " + resident;
        model->setFilter(result);
    }
    else
        model->setFilter(resident);
}

void RegistrationObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->show();
   widget->setPlaceholderText("Фильтер по ФИО резедента");
}

void RegistrationObject::toArchive()
{
    QItemSelectionModel *select = table->selectionModel();
    if(select->hasSelection()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(0, "Вопрос", "Вы действительно хотите переместить запись в архив?", QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            int row = select->selectedRows().first().row();

            QString query(Querys::insertArchiveRegistration
                        .arg(model->data(model->index(row, 0)).toString())
                        .arg(model->data(model->index(row, 1)).toString())
                        .arg(model->data(model->index(row, 2)).toString())
                        .arg(model->data(model->index(row, 3)).toString())
                        .arg(model->data(model->index(row, 4)).toString())
                        .arg(model->data(model->index(row, 5)).toString())
                        .arg(model->data(model->index(row, 6)).toString())
                        .arg(model->data(model->index(row, 7)).toDateTime().toString("dd.MM.yyyy"))
                        .arg(model->data(model->index(row, 8)).toDateTime().toString("dd.MM.yyyy"))
                        .arg(model->data(model->index(row, 9)).toString()));

            QSqlQuery sql(model->database());
            if(sql.exec(query))
            {
                if(sql.exec(Querys::selectPaymentList.arg(model->data(model->index(row, 0)).toString())))
                {
                     while (sql.next())
                     {
                          QSqlQuery q(model->database());
                          QString e(Querys::insertArchivePaymentList.arg(sql.value(0).toInt())
                                        .arg(sql.value(1).toString())
                                        .arg(sql.value(2).toString())
                                        .arg(sql.value(3).toString())
                                        .arg(sql.value(4).toString())
                                        .arg(sql.value(5).toDate().toString("dd.MM.yyyy")));
                          q.exec(e);
                     }
                }
            }
            model->removeRow(select->selectedRows().first().row());
            model->submitAll();
        }
    }
    else
       QMessageBox::information(0, "Инфо", "Укажите запись!");
}

void RegistrationObject::showReport(int index, const QString& param)
{
    QItemSelectionModel *select = table->selectionModel();
    if(select->hasSelection())
    {
        QSqlQuery query(Querys::numberBill, model->database());
        query.next();
        int number = query.value(0).toInt();
        int id     = model->data(select->selectedRows().first()).toInt();

        QSqlQueryModel *qmodel = new QSqlQueryModel(this);
        qmodel->setQuery(Querys::billList.arg(id), model->database());
        report->dataManager()->addModel("master", qmodel, true);
        if(report->loadFromFile(Reports::billList)){
            report->dataManager()->setReportVariable("BillNumber", number);
            report->previewReport();
            //report->designReport();
        }
    }
    else
       QMessageBox::information(0, "Инфо", "Укажите запись!");
}

////-------------------------

PaymentListObject::PaymentListObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* PaymentListObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("PAYMENT_LIST");
    model->setRelation(1, QSqlRelation("REGISTRATION", "P_ID_REGISTRATION", "RESIDENT_NAME"));
    model->setRelation(2, QSqlRelation("PAYMENT_TYPE", "P_ID_PAYMENT_TYPE", "PAY_TYPE"));
    model->select();
    showAllColumn(model->columnCount());

    table->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, "№");
    model->setHeaderData(1, Qt::Horizontal, "ФИО");
    model->setHeaderData(2, Qt::Horizontal, "Тип платежа");
    model->setHeaderData(3, Qt::Horizontal, "Оплачено");
    model->setHeaderData(4, Qt::Horizontal, "Доплата");
    model->setHeaderData(5, Qt::Horizontal, "Дата");
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 200);
    table->setColumnWidth(2, 100);
    table->setColumnWidth(3, 75);
    table->setColumnWidth(4, 75);
    table->setColumnWidth(5, 100);
    stackedWidget->setCurrentIndex(0);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(false);
    toolBar->actions().at(1)->setDisabled(false);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(false);

    toolBar->actions().at(12)->setDisabled(false);
    toolBar->actions().at(13)->setDisabled(false);


    return model;
}

void PaymentListObject::showDialog(const QModelIndex& index, QWidget *parent)
{
    PaymentListEditDialog dlg(model, parent);
    if(index.isValid())
        dlg.exec(index);
    else
        dlg.exec();
}

void PaymentListObject::setFilter(const QString& filter)
{
    if(filter.isEmpty()){
        setCurrentHotel(currentHotel);
        return;
    }
    QString resident = QString("RESIDENT_NAME LIKE '%%0%'").arg(filter);
    model->setFilter(resident);
}

QString PaymentListObject::setCurrentHotel(const QString& hotel)
{
    currentHotel = hotel;
    if(currentHotel.isEmpty()) {
        model->setFilter(currentHotel);
        return currentHotel;
    }

    QSqlQuery sql(Querys::hotelId.arg(currentHotel), model->database());
    QString select;
    QString param = QString("F_ID_REGISTRATION = '%0'");
    int i = 0;
    while(sql.next()) {
        if(i > 0)
            select += " or ";
        select += param.arg(sql.value(0).toInt());
        i++;
    }
    if(!select.isEmpty()){
        model->setFilter(select);
        return select;
    }
    else{
        model->setFilter(param.arg(-1));
        return param.arg(-1);
    }
}

void PaymentListObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->show();
   widget->setPlaceholderText("Фильтер по ФИО резедента");
}

////-------------------------

ArchiveRegistrationObject::ArchiveRegistrationObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* ArchiveRegistrationObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("ARCHIVE_REGISTRATION");
    model->select();
    showAllColumn(model->columnCount());
    table->setModel(model);

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "№");
    model->setHeaderData(2, Qt::Horizontal, "Отель");
    model->setHeaderData(3, Qt::Horizontal, "Номер комнаты");
    model->setHeaderData(4, Qt::Horizontal, "Статус");
    model->setHeaderData(5, Qt::Horizontal, "ФИО");
    model->setHeaderData(6, Qt::Horizontal, "Телефон");
    model->setHeaderData(7, Qt::Horizontal, "Организация");
    model->setHeaderData(8, Qt::Horizontal, "Дата приезда");
    model->setHeaderData(9, Qt::Horizontal, "Дата отьезда");
    model->setHeaderData(10, Qt::Horizontal, "Примечание");

    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 50);

    stackedWidget->setCurrentIndex(0);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(true);
    toolBar->actions().at(1)->setDisabled(true);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(false);

    toolBar->actions().at(12)->setDisabled(false);
    toolBar->actions().at(13)->setDisabled(false);

    return model;
}

void ArchiveRegistrationObject::setFilter(const QString& filter)
{
    if(filter.isEmpty()){
        setCurrentHotel(currentHotel);
        return;
    }

    QString resident = QString("RESIDENT_NAME LIKE '%%0%'").arg(filter);
    if(!currentHotel.isEmpty())
    {
        QString name = QString("HOTEL_NAME LIKE '%%0%'").arg(currentHotel);
        QString result = name + " and " + resident;
        model->setFilter(result);
    }
    else
        model->setFilter(resident);
}

void ArchiveRegistrationObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->show();
   widget->setPlaceholderText("Фильтер по ФИО резедента");
}

////-------------------------

ArchivePaymentListObject::ArchivePaymentListObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* ArchivePaymentListObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("ARCHIVE_PAYMENT_LIST");
    model->setRelation(2, QSqlRelation("ARCHIVE_REGISTRATION", "P_ID_REGISTRATION", "RESIDENT_NAME"));
    model->select();
    showAllColumn(model->columnCount());

    table->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "№");
    model->setHeaderData(2, Qt::Horizontal, "ФИО");
    model->setHeaderData(3, Qt::Horizontal, "Тип платежа");
    model->setHeaderData(4, Qt::Horizontal, "Оплачено");
    model->setHeaderData(5, Qt::Horizontal, "Доплата");
    model->setHeaderData(6, Qt::Horizontal, "Дата");

    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 50);

    stackedWidget->setCurrentIndex(0);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(true);
    toolBar->actions().at(1)->setDisabled(true);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(false);

    toolBar->actions().at(12)->setDisabled(false);
    toolBar->actions().at(13)->setDisabled(false);

    return model;
}

void ArchivePaymentListObject::setFilter(const QString& filter)
{
    if(filter.isEmpty()){
        setCurrentHotel(currentHotel);
        return;
    }

    QString resident = QString("RESIDENT_NAME LIKE '%%0%'").arg(filter);
    model->setFilter(resident);
}

QString ArchivePaymentListObject::setCurrentHotel(const QString& hotel)
{
    currentHotel = hotel;
    if(currentHotel.isEmpty()) {
        model->setFilter(currentHotel);
        return currentHotel;
    }

    QSqlQuery sql(Querys::hotelIdArchive.arg(currentHotel), model->database());
    QString select = QString();
    QString param = QString("F_ID_REGISTRATION = '%0'");
    int i = 0;
    while(sql.next()) {
        if(i > 0)
            select += " or ";
        select += param.arg(sql.value(0).toInt());
        i++;
    }
    model->setFilter(select);

    if(!select.isEmpty()){
        model->setFilter(select);
        return select;
    }
    else{
        model->setFilter(param.arg(-1));
        return param.arg(-1);
    }
}

void ArchivePaymentListObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->show();
   widget->setPlaceholderText("Фильтер по ФИО проживающего");
}

////-------------------------

PaymentTypeObject::PaymentTypeObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* PaymentTypeObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("PAYMENT_TYPE");
    model->select();
    showAllColumn(model->columnCount());
    table->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, "№");
    model->setHeaderData(1, Qt::Horizontal, "Вид платежа");
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 150);
    stackedWidget->setCurrentIndex(0);

    //setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(false);
    toolBar->actions().at(1)->setDisabled(false);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);

    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(true);
    toolBar->actions().at(8)->setDisabled(true);

    toolBar->actions().at(12)->setDisabled(true);
    toolBar->actions().at(13)->setDisabled(true);

    return model;
}

void PaymentTypeObject::showDialog(const QModelIndex& index, QWidget *parent)
{
    ReferenceEditDialog dlg(model, parent);
    if(index.isValid())
        dlg.exec(index);
    else
        dlg.exec();
}

void PaymentTypeObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->setPlaceholderText("");
   widget->hide();
}

////-------------------------

StatusRoomObject::StatusRoomObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* StatusRoomObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("STATUS_ROOM");
    model->select();
    table->setModel(model);
    showAllColumn(model->columnCount());

    model->setHeaderData(0, Qt::Horizontal, "№");
    model->setHeaderData(1, Qt::Horizontal, "Статус комнаты");
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 150);
    stackedWidget->setCurrentIndex(0);

    //setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(false);
    toolBar->actions().at(1)->setDisabled(false);
    toolBar->actions().at(2)->setDisabled(false);
    toolBar->actions().at(3)->setDisabled(false);

    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(true);
    toolBar->actions().at(8)->setDisabled(true);

    toolBar->actions().at(12)->setDisabled(true);
    toolBar->actions().at(13)->setDisabled(true);

    return model;
}

void StatusRoomObject::showDialog(const QModelIndex& index, QWidget *parent)
{
    ReferenceEditDialog dlg(model, parent);
    if(index.isValid())
        dlg.exec(index);
    else
        dlg.exec();
}

void StatusRoomObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->setPlaceholderText("");
   widget->hide();
}

////-------------------------

NumberedTableObject::NumberedTableObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent) { }

QSqlRelationalTableModel* NumberedTableObject::viewAdapter(QToolBar *toolBar)
{
    selectTable("HOTELS");
    model->select();
    showAllColumn(model->columnCount());
    table->setModel(model);
    stackedWidget->setCurrentIndex(3);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(true);
    toolBar->actions().at(1)->setDisabled(true);
    toolBar->actions().at(2)->setDisabled(true);
    toolBar->actions().at(3)->setDisabled(true);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(true);

    toolBar->actions().at(12)->setDisabled(true);
    toolBar->actions().at(13)->setDisabled(true);

    return model;
}

void NumberedTableObject::setPlaceHolderText(QLineEdit *widget)
{
   widget->clear();
   widget->setPlaceholderText("");
   widget->hide();
}

////-------------------------

ReportObject::ReportObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                         QSqlDatabase *db, QObject *parent):
    AbstractApplicationModel(comboBox, table, stackedWidget, db, parent)
{
    listReports.append(&ReportObject::showReport1);
    listReports.append(&ReportObject::showReport2);
    listReports.append(&ReportObject::showReport3);
}

QSqlRelationalTableModel* ReportObject::viewAdapter(QToolBar *toolBar)
{
    table->setModel(model);
    stackedWidget->setCurrentIndex(4);

    setCurrentHotel(hotelSelect->currentText());

    toolBar->actions().at(0)->setDisabled(true);
    toolBar->actions().at(1)->setDisabled(true);
    toolBar->actions().at(2)->setDisabled(true);
    toolBar->actions().at(3)->setDisabled(true);
    toolBar->actions().at(4)->setDisabled(true);
    toolBar->actions().at(5)->setDisabled(true);

    toolBar->actions().at(7)->setDisabled(false);
    toolBar->actions().at(8)->setDisabled(true);

    toolBar->actions().at(12)->setDisabled(true);
    toolBar->actions().at(13)->setDisabled(true);

    return model;
}

void ReportObject::showReport(int index, const QString& param)
{
    (this->*listReports[index])();
}

void ReportObject::showReport1()
{
    if(!hotelSelect->currentText().isEmpty())
    {
        QDate first  = rptFirstDate->date();
        QDate second = rptSeconDate->date();

        GeneralReportModel *gmodel = new GeneralReportModel(0, model->database());
        gmodel->load(hotelSelect->currentText(), first, second);
        report->dataManager()->addModel("master", gmodel, true);
        if(report->loadFromFile(Reports::generalReport)){
            report->dataManager()->setReportVariable("hotel", hotelSelect->currentText());
            report->dataManager()->setReportVariable("firstDate", first.toString("dd.MM.yyyy"));
            report->dataManager()->setReportVariable("secondDate", second.toString("dd.MM.yyyy"));
            report->previewReport();
            //report->designReport();
        }
        delete gmodel;
    }
}

void ReportObject::showReport2()
{
    if(!hotelSelect->currentText().isEmpty())
    {
        QDate first  = rptFirstDate->date();
        QDate second = rptSeconDate->date();

        QSqlQueryModel *qmodel = new QSqlQueryModel(this);
        qmodel->setQuery(Querys::logRegistration.arg(hotelSelect->currentText())
                         .arg(first.toString("dd.MM.yyyy"))
                         .arg(second.toString("dd.MM.yyyy")),
                         model->database());

        report->dataManager()->addModel("master", qmodel, true);
        if(report->loadFromFile(Reports::logRegistration)){
            report->dataManager()->setReportVariable("hotel", hotelSelect->currentText());
            report->dataManager()->setReportVariable("firstDate", first.toString("dd.MM.yyyy"));
            report->dataManager()->setReportVariable("secondDate", second.toString("dd.MM.yyyy"));
            report->previewReport();
            //report->designReport();
        }
    }
}

void ReportObject::showReport3()
{
    if(!hotelSelect->currentText().isEmpty())
    {
        QDate first  = rptFirstDate->date();
        QDate second = rptSeconDate->date();

        QSqlQueryModel *qmodel = new QSqlQueryModel(this);
        qmodel->setQuery(Querys::archiveLogRegistration.arg(hotelSelect->currentText())
                         .arg(first.toString("dd.MM.yyyy"))
                         .arg(second.toString("dd.MM.yyyy")),
                         model->database());

        report->dataManager()->addModel("master", qmodel, true);
        if(report->loadFromFile(Reports::archiveLogRegistration)){
            report->dataManager()->setReportVariable("hotel", hotelSelect->currentText());
            report->dataManager()->setReportVariable("firstDate", first.toString("dd.MM.yyyy"));
            report->dataManager()->setReportVariable("secondDate", second.toString("dd.MM.yyyy"));
            report->previewReport();
            //report->designReport();
        }
    }
}



////-------------------------

namespace CreateModel
{
    using AbstractObjectPtr = QSharedPointer<AbstractApplicationModel>;


    template<class Type = AbstractApplicationModel,
             class = typename std::enable_if<std::is_base_of
                                             <AbstractApplicationModel, Type>::value>::type>
    AbstractObjectPtr makeItemObject(QComboBox *comboBox,
                                     QTableView *table,
                                     QStackedWidget *stackedWidget,
                                     QSqlDatabase *db,
                                     QObject *parent = Q_NULLPTR)
    {
        return AbstractObjectPtr(new Type(comboBox, table, stackedWidget, db, parent),
                                 [](AbstractApplicationModel *object)
                                 {object->deleteLater();});
    }


    const ObjectCalling CallingMenu[] = { &makeItemObject<HotelsObject>,
                                          &makeItemObject<NumberedStockObject>,
                                          &makeItemObject<RegistrationObject>,
                                          &makeItemObject<PaymentListObject>,
                                          &makeItemObject<ArchiveObject>,
                                          &makeItemObject<ArchiveRegistrationObject>,
                                          &makeItemObject<ArchivePaymentListObject>,
                                          &makeItemObject<ReferenceObject>,
                                          &makeItemObject<PaymentTypeObject>,
                                          &makeItemObject<StatusRoomObject>,
                                          &makeItemObject<NumberedTableObject>,
                                          &makeItemObject<ReportObject>
                                        };
}






