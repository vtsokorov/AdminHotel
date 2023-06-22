#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QDate>
#include <QList>
#include <QString>
#include <QObject>
#include <QAbstractTableModel>
#include <QSqlDatabase>

struct ItemCell
{
    ItemCell(int room, const QDate &date, const QString data):
       number(room), when(date), cell(data){}

    int number;
    QDate when;
    QString cell;
};

using DataColumn = QList<ItemCell>;
using DataRow    = QList<DataColumn>;


class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit TableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool load(const QString &hotelName, const QDate &first, const QDate &second);
    void clear();

    void showLabel(bool value);

private:

    DataRow dataArray;

    QSqlDatabase dbase;
    QDate firstDate;
    QDate secondDate;

    int rows;
    int columns;
    bool label;
};

#endif // TABLEMODEL_H
