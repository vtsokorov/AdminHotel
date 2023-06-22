#ifndef ITEMCELL_H
#define ITEMCELL_H

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


#endif // ITEMCELL_H
