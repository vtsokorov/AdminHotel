#ifndef GENERALREPORTMODEL_H
#define GENERALREPORTMODEL_H

#include <QList>
#include <QString>
#include <QObject>
#include <QAbstractTableModel>
#include <QSqlDatabase>

struct ReportRow
{
    ReportRow() : when(QString()), reserv(0), paid(0), busy(0), free(0)
    {}

    ReportRow(const QString &date, int reservation, int paid, int busy, int free):
       when(date), reserv(reservation), paid(paid), busy(busy), free(free)
    {}

    QString when;
    int reserv;
    int paid;
    int busy;
    int free;
};

using ReportData = QList<ReportRow>;


class GeneralReportModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit GeneralReportModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool load(const QString &hotelName, const QDate &first, const QDate &second);
    void clear();

private:

    ReportData dataArray;
    QSqlDatabase dbase;

    int rows;
    int columns;
};

#endif // GENERALREPORTMODEL_H
