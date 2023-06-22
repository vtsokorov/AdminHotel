#ifndef APPLICATIONMODEL_H
#define APPLICATIONMODEL_H

#include <QObject>
#include <QModelIndex>

class QSqlDatabase;
class QSqlTableModel;
class QSqlRelationalTableModel;

class QStackedWidget;
class QTableView;
class QLineEdit;
class QToolBar;
class QDateEdit;
class QComboBox;
class QProgressDialog;

namespace LimeReport {
class ReportEngine;
}

class AbstractApplicationModel : public QObject
{
    Q_OBJECT

public:

    AbstractApplicationModel(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);

    virtual ~AbstractApplicationModel();
    virtual QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) = 0;
    virtual void showDialog(const QModelIndex& index = QModelIndex(), QWidget *parent = 0);
    virtual void showReport(int index = 0, const QString& param = QString());
    virtual void execQuery(int index = 0, const QString& query = QString());

    virtual void setFilter(const QString& filer = QString());
    virtual QString setCurrentHotel(const QString &hotel);

    virtual void setPlaceHolderText(QLineEdit *widget);
    virtual void deleteRecord();
    virtual void refresh();
    virtual void toArchive();

    void clearFilter();

    virtual QString name() { return QString(); }

    void setReportDateWidgets(QDateEdit *widget1, QDateEdit *widget2);

protected:

    void showAllColumn(int columnsSize);
    void selectTable(const QString &tableName);

    QDate inputDate(const QString &title, bool &isOk);
    QString inputData(const QString &title, bool &isOk);
    QString selectData(const QString &title, const QStringList &items, bool &isOk);

protected:

    QSqlTableModel *hotelModel;
    QSqlRelationalTableModel *model;

    QDateEdit *rptFirstDate;
    QDateEdit *rptSeconDate;
    QComboBox *hotelSelect;
    QTableView *table;
    QStackedWidget *stackedWidget;

    QString currentHotel;

    LimeReport::ReportEngine *report;
    QProgressDialog* m_progressDialog;
    int m_currentPage;

private slots:

    void renderStarted();
    void renderPageFinished(int renderedPageCount);
    void renderFinished();
};


class ReferenceObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    ReferenceObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
};

class ArchiveObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    ArchiveObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
};

class HotelsObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    HotelsObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~HotelsObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void showDialog(const QModelIndex& index = QModelIndex(), QWidget *parent = 0) override;
    void setFilter(const QString& filter = QString()) override;
    void setPlaceHolderText(QLineEdit *widget) override;
};

class NumberedStockObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    NumberedStockObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~NumberedStockObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void showDialog(const QModelIndex& index = QModelIndex(), QWidget *parent = 0) override;
    void setFilter(const QString& filter = QString()) override;
    void setPlaceHolderText(QLineEdit *widget) override;
};

class RegistrationObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    RegistrationObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~RegistrationObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void showDialog(const QModelIndex& index = QModelIndex(), QWidget *parent = 0) override;
    void setFilter(const QString& filter = QString()) override;
    void setPlaceHolderText(QLineEdit *widget) override;
    void toArchive() override;
    void showReport(int index = 0, const QString& param = QString()) override;
};

class PaymentListObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    PaymentListObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~PaymentListObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void showDialog(const QModelIndex& index = QModelIndex(), QWidget *parent = 0) override;
    void setFilter(const QString& filter = QString()) override;
    QString setCurrentHotel(const QString& hotel);
    void setPlaceHolderText(QLineEdit *widget) override;
};

class ArchiveRegistrationObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    ArchiveRegistrationObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~ArchiveRegistrationObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void setFilter(const QString& filter = QString()) override;
    void setPlaceHolderText(QLineEdit *widget) override;
};

class ArchivePaymentListObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    ArchivePaymentListObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~ArchivePaymentListObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void setFilter(const QString& filter = QString()) override;
    QString setCurrentHotel(const QString& hotel) override;
    void setPlaceHolderText(QLineEdit *widget) override;
};

class PaymentTypeObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    PaymentTypeObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~PaymentTypeObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void showDialog(const QModelIndex& index = QModelIndex(), QWidget *parent = 0) override;
    void setPlaceHolderText(QLineEdit *widget) override;
};

class StatusRoomObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    StatusRoomObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~StatusRoomObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void showDialog(const QModelIndex& index = QModelIndex(), QWidget *parent = 0) override;
    void setPlaceHolderText(QLineEdit *widget) override;
};

class NumberedTableObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    NumberedTableObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void setPlaceHolderText(QLineEdit *widget) override;
    virtual QString name() override { return QString("FORGENTABLE"); }
};

class ReportObject : public AbstractApplicationModel
{
    Q_OBJECT

public:

    ReportObject(QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget,
                             QSqlDatabase *db, QObject *parent);
    ~ReportObject() {    }
    QSqlRelationalTableModel* viewAdapter(QToolBar *toolBar) override;
    void showReport(int index = 0, const QString& param = QString()) override;

private:

    typedef void (ReportObject::*ExecReport)();
    QList<ExecReport> listReports;

private:

    void showReport1();
    void showReport2();
    void showReport3();

};

namespace CreateModel {

    typedef QSharedPointer<AbstractApplicationModel> (*ObjectCalling)
                (QComboBox *comboBox, QTableView *table, QStackedWidget *stackedWidget, QSqlDatabase *db, QObject *parent);
    extern const ObjectCalling CallingMenu[];

}



#endif // APPLICATIONMODEL_H
