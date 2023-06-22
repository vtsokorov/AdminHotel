#ifndef PRINTEROBJECT_H
#define PRINTEROBJECT_H

#include <QObject>
#include <QString>

class QPrinter;
class QTableView;

class PrinterObject : public QObject
{
    Q_OBJECT

public:

    explicit PrinterObject(QObject *parent = 0);

    void printTable(const QString& title, QTableView* tableView);

protected:
    void render(const QString& title, QTableView* tableView, QPrinter* printer);
};

#endif // PRINTEROBJECT_H
