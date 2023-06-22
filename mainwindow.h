#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

class QSqlRelationalTableModel;
class AbstractApplicationModel;
class QPushButton;
class TableModel;

class QPrinter;
class QTableView;
class QLineEdit;
class QComboBox;

namespace Ui {
class MainWindow;
}

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

    virtual void closeEvent(QCloseEvent *);

public slots:

    void show();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    QLabel *lwUser;
    QLabel *lwHost;
    QLabel *lwBase;
    QLabel *lwPort;

    QSqlRelationalTableModel *model;
    QSharedPointer<AbstractApplicationModel> item;

    TableModel *stockModel;

    QWidget *spacerWidget;
    QLineEdit *lineFilter;
    QComboBox *cbTypeFiler;

    QAction *actFilter;
    QAction *actClear;

private:

    void setupWidget();

private slots:

    void selectedItem(const QModelIndex &index);
    void selectHotel(const QString& value);

    void selectedQuery(const QModelIndex &index);
    void selectedReports(const QModelIndex &index);
    void openEditDialog(const QModelIndex &index);
    void showNumberedTable();
    void showStatus(bool value);
    void printNumTable();
    void billPrint();
    void showAboutDialog();

    void showToolBar(bool value);
    void showTreeView(bool value);

    void editRecord();
    void addRecord();
    void deleteRecord();
    void refresh();
    void toArchive();

    void filterOn();
    void filterOff();


};

#endif // MAINWINDOW_H
