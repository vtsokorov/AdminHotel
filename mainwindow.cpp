#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "global.h"
#include "aboutdialog.h"
#include "applicationmodel.h"
#include "logindialog.h"
#include "tablemodel.h"
#include "printerobject.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

#include <QDebug>
#include <QMessageBox>
#include <QtMath>

#include <QSqlRelationalTableModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), stockModel(nullptr),
    spacerWidget(new QWidget(this)), lineFilter(new QLineEdit(this)),
    cbTypeFiler(new QComboBox(this)), actFilter(new QAction(this)),
    actClear(new QAction(this))

{
    ui->setupUi(this);

    setupWidget();
}

MainWindow::~MainWindow()
{
    delete stockModel;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
   if(db.isOpen()) db.close();
}

void MainWindow::setupWidget()
{
    lwUser = new QLabel(this);
    lwUser->setAlignment(Qt::AlignHCenter);
    lwUser->setMinimumSize(lwUser->sizeHint());

    lwHost = new QLabel(this);
    lwHost->setAlignment(Qt::AlignHCenter);
    lwHost->setMinimumSize(lwHost->sizeHint());

    lwBase = new QLabel(this);
    lwBase->setAlignment(Qt::AlignLeft);
    lwBase->setMinimumSize(lwBase->sizeHint());

    lwPort = new QLabel(this);
    lwPort->setAlignment(Qt::AlignHCenter);
    lwPort->setMinimumSize(lwPort->sizeHint());

    statusBar()->addWidget(lwUser);
    statusBar()->addWidget(lwHost);
    statusBar()->addWidget(lwPort);
    statusBar()->addWidget(lwBase, 1);

    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->mainToolBar->addAction(ui->actAdd);
        ui->mainToolBar->addAction(ui->actEdit);
        ui->mainToolBar->addAction(ui->actUpdate);
        ui->mainToolBar->addAction(ui->actDelete);
        ui->mainToolBar->addAction(ui->actToArchive);
        ui->mainToolBar->addAction(ui->actBillPrint);

        ui->mainToolBar->addSeparator();
        cbTypeFiler->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        cbTypeFiler->setFixedWidth(150);
        cbTypeFiler->setToolTip("Выбор отеля");
        ui->mainToolBar->addWidget(cbTypeFiler);
        actClear->setIcon(QIcon(":/img/find.png"));
        actClear->setText("Показать все");
        ui->mainToolBar->addAction(actClear);

    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);
        ui->mainToolBar->addSeparator();
        ui->mainToolBar->addWidget(spacerWidget);
        ui->mainToolBar->addSeparator();

    lineFilter->setMaximumWidth(250);
    lineFilter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    lineFilter->setPlaceholderText("Фильтр");
    lineFilter->setFont(QFont("Tahoma", 10));
        ui->mainToolBar->addWidget(lineFilter);
        actFilter->setIcon(QIcon(":/img/filter.png"));
        actFilter->setText("Отфильтровать");
        ui->mainToolBar->addAction(actFilter);
    ui->mainToolBar->setHidden(false);

    ui->treeWidget->expandAll();
    ui->stackedWidget->setCurrentIndex(0);
    ui->cbStatus->setChecked(true);



    connect(cbTypeFiler, &QComboBox::currentTextChanged, this, &MainWindow::selectHotel);
    connect(actFilter, &QAction::triggered, this, &MainWindow::filterOn);
    connect(actClear, &QAction::triggered, this, &MainWindow::filterOff);
    connect(ui->actAdd, &QAction::triggered, this, &MainWindow::addRecord);
    connect(ui->actEdit, &QAction::triggered, this, &MainWindow::editRecord);
    connect(ui->actDelete, &QAction::triggered, this, &MainWindow::deleteRecord);
    connect(ui->actUpdate, &QAction::triggered, this, &MainWindow::refresh);
    connect(ui->actToArchive, &QAction::triggered, this, &MainWindow::toArchive);
    connect(ui->actBillPrint, &QAction::triggered, this, &MainWindow::billPrint);
    connect(ui->actAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    connect(ui->actActionPanel, &QAction::triggered, this, &MainWindow::showToolBar);
    connect(ui->mainToolBar, &QToolBar::visibilityChanged, ui->actActionPanel, &QAction::setChecked);
    connect(ui->actTreeView, &QAction::triggered, this, &MainWindow::showTreeView);

    connect(ui->actExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->pbShowTable, &QAbstractButton::clicked, this, &MainWindow::showNumberedTable);
    connect(ui->pbPrintNumTable, &QAbstractButton::clicked, this, &MainWindow::printNumTable);
    connect(ui->cbStatus, &QCheckBox::clicked, this, &MainWindow::showStatus);

    connect(ui->treeWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(selectedItem(QModelIndex)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openEditDialog(QModelIndex)));
    connect(ui->listWidget, &QListWidget::doubleClicked, this, &MainWindow::selectedReports);

    selectedItem(ui->treeWidget->model()->index(0,0));
}

void MainWindow::show()
{
    LoginDialog dlg(&db, this);
    while(!db.isOpen()){
        dlg.exec();
    }

    lwUser->setText(" Пользователь: " + db.userName() + " ");
    lwHost->setText(" Сервер: " + db.hostName() + " ");
    lwPort->setText(" Порт: "+QString::number(db.port()) + " ");
    lwBase->setText(" Размещение БД: " + db.databaseName() + " ");

    stockModel = new TableModel(this, db);

    auto callableObject = CreateModel::CallingMenu[ArrayIndex::indexes[1][0]];
    item = callableObject(cbTypeFiler, ui->tableView, ui->stackedWidget, &db, this);

    model = item->viewAdapter(ui->mainToolBar);
    item->setPlaceHolderText(lineFilter);

    QMainWindow::show();
}

void MainWindow::selectedItem(const QModelIndex &index)
{
    int i = qAbs(ui->treeWidget->indexBelow(index).row()+index.parent().row());
    int j = qAbs(index.parent().row()+index.row()+1);

    qDebug()<<"i = "<<i<<"j = "<<j;

    auto callableObject = CreateModel::CallingMenu[ArrayIndex::indexes[i][j]];
    item = callableObject(cbTypeFiler, ui->tableView, ui->stackedWidget, &db, this);

    model = item->viewAdapter(ui->mainToolBar);
    item->setPlaceHolderText(lineFilter);
    item->setReportDateWidgets(ui->deRptFirstDate, ui->deRptSeconDate);
}

void MainWindow::openEditDialog(const QModelIndex &index)
{
    item->showDialog(index, this);
}

void MainWindow::editRecord()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if(select->hasSelection()){
        item->showDialog(select->selectedRows().first(), this);
    }
    else
        QMessageBox::information(this, "Инфо", "Укажите запись!");
}

void MainWindow::addRecord()
{
    item->showDialog(QModelIndex(), this);
}

void MainWindow::deleteRecord()
{
    item->deleteRecord();
}
void MainWindow::refresh()
{
    item->refresh();
}

void MainWindow::toArchive()
{
    item->toArchive();
}

void MainWindow::selectedQuery(const QModelIndex &index)
{
//    item->execQuery(index.row());
}

void MainWindow::selectedReports(const QModelIndex &index)
{
    item->showReport(index.row());
}

void MainWindow::filterOn()
{
   item->setFilter(lineFilter->text());
}

void MainWindow::selectHotel(const QString& value)
{
   item->setCurrentHotel(value);
   lineFilter->clear();
}

void MainWindow::filterOff()
{
   item->clearFilter();
   lineFilter->clear();
}

void MainWindow::showToolBar(bool value)
{
    ui->mainToolBar->setHidden(!value);
}

void MainWindow::showTreeView(bool value)
{
    ui->treeWidget->setHidden(!value);
}

void MainWindow::showStatus(bool value)
{
    stockModel->showLabel(value);
}

void MainWindow::billPrint()
{
    item->showReport(0);
}

void MainWindow::printNumTable()
{
    QString title = "Номерной фонд "+ui->deDateFirst->date().toString("dd.MM.yyyy")
                             +" - "
                             +ui->deDateSecond->date().toString("dd.MM.yyyy");
    PrinterObject obj;
    obj.printTable(title, ui->stockTable);
}

void MainWindow::showNumberedTable()
{
    if(stockModel->load(cbTypeFiler->currentText(),
                        ui->deDateFirst->date(),
                        ui->deDateSecond->date())){
        ui->stockTable->clearSpans();
        ui->stockTable->setSpan(stockModel->rowCount()-1, 0, 1, 2);
        ui->stockTable->setModel(stockModel);
        ui->stockTable->resizeRowsToContents();
        ui->stockTable->resizeColumnsToContents();
    }
    else ui->stockTable->setModel(0);
}

void MainWindow::showAboutDialog()
{
   AboutDialog dlg(this);
   dlg.exec();
}

