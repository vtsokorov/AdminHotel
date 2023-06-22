#include "tablemodel.h"
#include <QVariant>
#include <QLocale>

#include <QSqlQuery>
#include <QSize>
#include <QColor>
#include <QFont>

#include <QDebug>


namespace
{
    const QString countStock =
        "SELECT COUNT(*) FROM NUMBERED_STOCK "
        "INNER JOIN HOTELS ON (NUMBERED_STOCK.F_ID_HOTEL = HOTELS.P_ID_HOTEL) "
        "WHERE HOTELS.HOTEL_NAME = '%0'";

    const QString hotelStock =
        "SELECT NUMBERED_STOCK.ROOM_NUMBER, NUMBERED_STOCK.ROOM_AMOUNT "
        "FROM HOTELS "
        "INNER JOIN NUMBERED_STOCK ON (HOTELS.P_ID_HOTEL = NUMBERED_STOCK.F_ID_HOTEL) "
        "WHERE ((HOTELS.HOTEL_NAME = '%0')) ORDER BY NUMBERED_STOCK.ROOM_NUMBER";

    const QString hotelLoad =
        "SELECT HOTELS.HOTEL_NAME, NUMBERED_STOCK.ROOM_NUMBER, STATUS_ROOM.STATUS, "
                "REGISTRATION.ARRIVAL, REGISTRATION.DEPARTURE "
        "FROM REGISTRATION "
               "INNER JOIN NUMBERED_STOCK ON (REGISTRATION.F_ID_NUMBERED_STOCK = NUMBERED_STOCK.P_ID_NUMBERED_STOCK) "
               "INNER JOIN STATUS_ROOM ON (REGISTRATION.F_ID_STATUS_ROOM = STATUS_ROOM.P_ID_STATUS_ROOM) "
               "INNER JOIN HOTELS ON (REGISTRATION.F_ID_HOTELS = HOTELS.P_ID_HOTEL) "
        "WHERE ((HOTELS.HOTEL_NAME = '%0') AND "
               "(SELECT * FROM INTERSECTIONDATE(NUMBERED_STOCK.P_ID_NUMBERED_STOCK, '%1', '%2')) > 0) "
        "ORDER BY REGISTRATION.ARRIVAL";

    const QString reservation = "Бронь";
    const QString paid        = "Оплачено";
    const QString busy        = "Занято";
}

TableModel::TableModel(QObject *parent, QSqlDatabase db) :
    QAbstractTableModel(parent), dbase(db), rows(0), columns(0),
    label(true)
{

}


Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    return theFlags;
}


QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !dataArray.count())
        return QVariant();

    DataColumn record = dataArray.at(index.row());

    switch(role)
    {
        case Qt::DisplayRole: // Данные для отображения
        case Qt::EditRole:
            if(index.row() <= rows-1 && index.column() <= 1)
               return record.at(index.column()).cell;
            else if(index.row() == rows-1)
               return record.at(index.column()).cell;
            else if(label)
               return record.at(index.column()).cell;
            else
                return QVariant();
        case Qt::ForegroundRole: // Цвет текста
            if(index.column() == 0 || index.column() == 1)
               return QVariant::fromValue(QColor(Qt::yellow));
            else
               return QVariant();
        case Qt::TextAlignmentRole: // Выравнивание
             if(index.column() >= 0 && index.column() <= columns)
                return int(Qt::AlignHCenter | Qt::AlignVCenter);
             else
                return int(Qt::AlignLeft | Qt::AlignVCenter);
        case Qt::FontRole: // Шрифт
             if(index.column() == 0 || index.column() == 1) {
                 QFont font = QFont("Helvetica", 10, QFont::Bold);
                 return QVariant::fromValue(font);
            }else
                 return QVariant();
        case Qt::BackgroundRole: {  // Цвет фона
            QString value = record.at(index.column()).cell;
            if(value == reservation)
                 return QVariant::fromValue(QColor(Qt::yellow));
            else if(value == paid)
                return QVariant::fromValue(QColor(Qt::red));
            else if(value == busy)
                return QVariant::fromValue(QColor(Qt::green));
            else if(index.column() <= 1)
                return QVariant::fromValue(QColor(Qt::lightGray));
            else
                return QVariant();
            }
        case Qt::SizeHintRole:  // Размер ячейки ???
               return QSize(30, 12);
    }

    return QVariant();
}


QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if(section == 0) return QObject::tr("№ ком.");
        else if(section == 1) return QObject::tr("Мест");
        else {
            QLocale locale(QLocale("ru_RU"));
            for(int i = 2; i < columns; ++i) {
                if(section == i){
                    QDate date = firstDate.addDays(i-2);
                    return date.toString("dd.MM")+" "+
                            locale.dayName(date.dayOfWeek(), QLocale::ShortFormat);
                }
            }
        }
    }
    return section + 1;
}


int TableModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : rows;
}

int TableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : columns;
}

void TableModel::showLabel(bool value)
{
    beginResetModel();
    label = value;
    endResetModel();
}

void TableModel::clear()
{
    dataArray.clear();
    columns = rows = 0;
    beginResetModel();
    endResetModel();
}

bool TableModel::load(const QString &hotelName, const QDate &first, const QDate &second)
{
    if(first > second || hotelName.isEmpty()) {
        rows = columns = 0;
        return false;
    }

    firstDate  = first;
    secondDate = second;

    dataArray.clear();

    QSqlQuery sql(countStock.arg(hotelName), dbase); sql.next();

    rows    = sql.value(0).toInt();
    columns = first.daysTo(second);

    sql.clear();

    if(rows <= 0 || columns <= 0) {
        rows = columns = 0;
        return false;
    }

    columns = columns + 3;
    rows    = rows + 1;

    dataArray.reserve(rows);
    foreach (DataColumn item, dataArray)
        item.reserve(columns);

    sql.exec(hotelStock.arg(hotelName));

    QSqlQuery query(hotelLoad.arg(hotelName)
                    .arg(first.toString("dd.MM.yyyy"))
                    .arg(second.toString("dd.MM.yyyy")), dbase);

    for(int i = 0; i < rows-1; ++i)
    {
        DataColumn item; sql.next();
        for(int j = 0; j < columns; ++j)
        {
            if(!sql.isNull(j))
            {
                ItemCell cell = { sql.value(j).toInt(),
                                  QDate(),
                                  sql.value(j).toString()};
                item.append(cell);
            }
            else {

                ItemCell cell = { item.first().number,
                                  firstDate.addDays(j-2),
                                  QString()};
                while(query.next()) {
                    int number      = query.value(1).toInt();
                    QString status  = query.value(2).toString();
                    QDate arrival   = query.value(3).toDateTime().date();
                    QDate departure = query.value(4).toDateTime().date();
                    int daysTo      = arrival.daysTo(departure)+1;
                    for(int i = 0; i < daysTo; ++i)
                    {
                        bool flag  = false;
                        QDate date = arrival.addDays(i);
                        if(cell.when == date && cell.number == number){
                            cell.cell = status;
                            flag = true; break;
                        }
                        if(flag) break;
                    }
                }
                query.seek(-1);

                item.append(cell);
            }
        }
        dataArray.append(item);
    }

    DataColumn item;
    for(int i = 0; i < columns; ++i) {
         int count = 0;
         for(int j = 0; j < rows-1; ++j) {
             if(!dataArray[j][i].cell.isEmpty())
                 count++;
         }
         ItemCell cell = {-1, QDate(), i > 1 ?
                          QString::number(rows-1-count) :
                          i == 0 ? QString("Свободно:") : QString()};
         item.append(cell);
    }
    dataArray.append(item);

    beginResetModel();
    endResetModel();

    return true;
}


/*
using:

void MainWindow::showNumberedTable()
{
    stockModel->clear();
    stockModel->showLabel(true);

    stockModel->load(ui->cbHotels->currentText(),
                     ui->deDateFirst->date(),
                     ui->deDateSecond->date());

    ui->stockTable->clearSpans();
    ui->stockTable->setSpan(stockModel->rowCount()-1, 0, 1, 2);
    ui->stockTable->setModel(stockModel);
    ui->stockTable->resizeRowsToContents();
    ui->stockTable->resizeColumnsToContents();  
}
*/





