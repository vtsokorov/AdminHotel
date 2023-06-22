#include "generalreportmodel.h"

#include <QSqlQuery>
#include <QSize>
#include <QColor>
#include <QFont>
#include <QDate>
#include <QFile>
#include <QDebug>


namespace
{

const QString generalReport =
         "SELECT COUNT( STATUS_ROOM.STATUS ) FROM HOTELS "
         "INNER JOIN REGISTRATION ON (HOTELS.P_ID_HOTEL = REGISTRATION.F_ID_HOTELS) "
         "INNER JOIN STATUS_ROOM ON (REGISTRATION.F_ID_STATUS_ROOM = STATUS_ROOM.P_ID_STATUS_ROOM) "
         "WHERE (HOTELS.HOTEL_NAME = '%0' AND STATUS_ROOM.STATUS = 'Бронь' AND (SELECT * FROM INTERSECTIONDATE(REGISTRATION.F_ID_NUMBERED_STOCK, '%1', '%2')) > 0) "
             "UNION ALL "
         "SELECT COUNT( STATUS_ROOM.STATUS ) FROM HOTELS "
         "INNER JOIN REGISTRATION ON (HOTELS.P_ID_HOTEL = REGISTRATION.F_ID_HOTELS) "
         "INNER JOIN STATUS_ROOM ON (REGISTRATION.F_ID_STATUS_ROOM = STATUS_ROOM.P_ID_STATUS_ROOM) "
         "WHERE (HOTELS.HOTEL_NAME = '%0' AND STATUS_ROOM.STATUS = 'Оплачено' AND (SELECT * FROM INTERSECTIONDATE(REGISTRATION.F_ID_NUMBERED_STOCK, '%1', '%2')) > 0) "
             "UNION ALL "
         "SELECT COUNT( STATUS_ROOM.STATUS ) FROM HOTELS "
         "INNER JOIN REGISTRATION ON (HOTELS.P_ID_HOTEL = REGISTRATION.F_ID_HOTELS) "
         "INNER JOIN STATUS_ROOM ON (REGISTRATION.F_ID_STATUS_ROOM = STATUS_ROOM.P_ID_STATUS_ROOM) "
         "WHERE (HOTELS.HOTEL_NAME = '%0' AND STATUS_ROOM.STATUS = 'Занято' AND (SELECT * FROM INTERSECTIONDATE(REGISTRATION.F_ID_NUMBERED_STOCK, '%1', '%2')) > 0) "
             "UNION ALL "
         "SELECT  COUNT( NUMBERED_STOCK.ROOM_NUMBER ) - "
         "(SELECT COL2 FROM ( SELECT SUM(COL1) AS COL2 FROM "
         "( "
             "SELECT COUNT( STATUS_ROOM.STATUS ) AS COL1 "
             "FROM HOTELS "
             "INNER JOIN REGISTRATION ON (HOTELS.P_ID_HOTEL = REGISTRATION.F_ID_HOTELS) "
             "INNER JOIN STATUS_ROOM ON (REGISTRATION.F_ID_STATUS_ROOM = STATUS_ROOM.P_ID_STATUS_ROOM) "
             "WHERE (HOTELS.HOTEL_NAME = '%0' AND STATUS_ROOM.STATUS = 'Бронь' AND (SELECT * FROM INTERSECTIONDATE(REGISTRATION.F_ID_NUMBERED_STOCK, '%1', '%2')) > 0) "
                "UNION ALL "
             "SELECT COUNT( STATUS_ROOM.STATUS )  AS COL1 "
             "FROM HOTELS "
             "INNER JOIN REGISTRATION ON (HOTELS.P_ID_HOTEL = REGISTRATION.F_ID_HOTELS) "
             "INNER JOIN STATUS_ROOM ON (REGISTRATION.F_ID_STATUS_ROOM = STATUS_ROOM.P_ID_STATUS_ROOM) "
             "WHERE (HOTELS.HOTEL_NAME = '%0' AND STATUS_ROOM.STATUS = 'Оплачено' AND (SELECT * FROM INTERSECTIONDATE(REGISTRATION.F_ID_NUMBERED_STOCK, '%1', '%2')) > 0) "
                 "UNION ALL "
             "SELECT COUNT( STATUS_ROOM.STATUS )  AS COL1 "
             "FROM HOTELS "
             "INNER JOIN REGISTRATION ON (HOTELS.P_ID_HOTEL = REGISTRATION.F_ID_HOTELS) "
             "INNER JOIN STATUS_ROOM ON (REGISTRATION.F_ID_STATUS_ROOM = STATUS_ROOM.P_ID_STATUS_ROOM) "
             "WHERE (HOTELS.HOTEL_NAME = '%0' AND STATUS_ROOM.STATUS = 'Занято' AND (SELECT * FROM INTERSECTIONDATE(REGISTRATION.F_ID_NUMBERED_STOCK, '%1', '%2')) > 0) "
         ") T1 ) T2 ) "
         "FROM HOTELS "
            "INNER JOIN NUMBERED_STOCK ON (HOTELS.P_ID_HOTEL = NUMBERED_STOCK.F_ID_HOTEL) "
         "WHERE ((HOTELS.HOTEL_NAME = '%0'))";
}


GeneralReportModel::GeneralReportModel(QObject *parent, QSqlDatabase db) :
    QAbstractTableModel(parent), dbase(db), rows(0), columns(0)
{

}

Qt::ItemFlags GeneralReportModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    return theFlags;
}


QVariant GeneralReportModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !dataArray.count())
        return QVariant();

    ReportRow record = dataArray.at(index.row());

    if(role == Qt::DisplayRole || Qt::DisplayRole == Qt::EditRole){
            if(index.column() == 0)
               return record.when;
            else if(index.column() == 1)
               return record.reserv;
            else if(index.column() == 2)
               return record.paid;
            else if(index.column() == 3)
               return record.busy;
            else if(index.column() == 4)
               return record.free;
            else
                return QVariant();
    }

    return QVariant();
}


QVariant GeneralReportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if(section == 0)
            return QString("Дата");
        if(section == 1)
            return QString("Бронь");
        if(section == 2)
            return QString("Оплачено");
        if(section == 3)
            return QString("Занято");
        if(section == 4)
            return QString("Свободно");
    }
    return section + 1;
}


int GeneralReportModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : rows;
}

int GeneralReportModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : columns;
}

void GeneralReportModel::clear()
{
    dataArray.clear();
    columns = rows = 0;
    beginResetModel();
    endResetModel();
}

bool GeneralReportModel::load(const QString &hotelName, const QDate &first, const QDate &second)
{
    if(first > second || hotelName.isEmpty()) {
        rows = columns = 0;
        return false;
    }

    dataArray.clear();
    rows    = first.daysTo(second)+1;
    columns = 5;

    QDate date = first;;
    for(int i = 0, j = 0; i < rows; ++i)
    {
        ReportRow data;
        date        = first.addDays(i);
        data.when   = date.toString("dd.MM.yyyy");
        QString sql = generalReport.arg(hotelName)
                .arg(date.toString("dd.MM.yyyy")).arg(second.toString("dd.MM.yyyy"));

        QSqlQuery query(sql, dbase);
        if(!query.isActive()) return false;
        while(query.next()) {
            if(j == 0) data.reserv = query.value(0).toInt();
            if(j == 1) data.paid = query.value(0).toInt();
            if(j == 2) data.busy = query.value(0).toInt();
            if(j == 3) data.free = query.value(0).toInt();
            j++;
        }j = 0;

        dataArray.append(data);
    }

    return true;
}



