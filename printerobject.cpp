#include "printerobject.h"

#include <QPrinter>
#include <QPageSize>
#include <QPageLayout>
#include <QPainter>
#include <QInputDialog>
#include <QFileDialog>
#include <QPrintDialog>
#include <QTableView>
#include <QHeaderView>

PrinterObject::PrinterObject(QObject *parent): QObject(parent)
{

}

void PrinterObject::printTable(const QString& title, QTableView* tableView)
{
       QPrinter printer(QPrinter::HighResolution);
       printer.setPageSize(QPageSize::A4);
       printer.setColorMode(QPrinter::Color);
       printer.setPageOrientation(QPageLayout::Landscape);
       printer.setFullPage(true);

       QString modePrinter = tr("Принтер");
       QString modePDF = tr("Файл PDF");
       QStringList items;
       items << modePDF << modePrinter;
       bool ok;
       QString mode = QInputDialog::getItem(
           0, tr("Печать"),
           tr("Тип печати:"), items, 0, false, &ok);
       if (!ok || mode.isEmpty()) {
           return;
       }

       QString fileName = QString();
       bool f;
       if (mode == modePDF){
           printer.setOutputFormat(QPrinter::PdfFormat);
           fileName = QFileDialog::getSaveFileName(
               0, tr("Выбор файла"),
               QString(),
               tr("Adobe PDF (*.pdf);;Все файлы (*.*)"));
           f = fileName.isEmpty();
       }else{
           printer.setOutputFormat(QPrinter::NativeFormat);
           QPrintDialog dlg(&printer, 0);
           f = (dlg.exec() != QDialog::Accepted);
       }
       if ( f )
           return;

       if (!fileName.isEmpty()){
           printer.setOutputFileName(fileName);
           int dpi = QInputDialog::getInt(
           0, tr("Выбор разрешения"),
           tr("Разрешение:"), 75, 75, 999999, 1, &ok);
           if ( ok )
               printer.setResolution(dpi);
       }else{
           QList<int> res = printer.supportedResolutions();
           items.clear();
           foreach (int i, res)
               items.append(QString("%1").arg(i));

           mode = QInputDialog::getItem(
               0, tr("Выбор разрешения"),
               tr("Разрешение:"), items, 0, false, &ok);
           if ( ok ) {
            bool flag;
            int dpi = mode.toInt(&flag);
               if ( flag )
                printer.setResolution(dpi);
           }
       }
    render(title, tableView, &printer);
}

void PrinterObject::render(const QString& title, QTableView* tableView, QPrinter* printer)
{
  if (tableView) {
    QAbstractItemModel* qModel = tableView->model();
    QHeaderView* hHeader = tableView->horizontalHeader();
    QHeaderView* vHeader = tableView->verticalHeader();

    if (qModel) {
      QPainter painter(printer);

      if (!painter.isActive())
        return;

      const int addtoColumnWidth = 35;
      int pageLeft = 25;
      int pageTop  = 100;
      int pageHeight = painter.device()->height();
      int pageWidth = painter.device()->width();
      int x = pageLeft;
      int y = pageTop;
      QList<QVector<QLine> > linePages;
      QVector<QLine> verticalLines;
      QList<QList<int> > sectionPages;
      QList<int> sections;
      QList<QList<int> > widthPages;
      QList<int> widths;
      QList<int> pageRights;
      QList<QStringList> pageLabels;
      QStringList labels;
      int logicalIndex;

      QFont font("Times", 16, QFont::Bold);
      painter.setFont(font);
      painter.drawText(70, 70, title);
      font.setBold(false);
      font.setPixelSize(14);
      painter.setFont(font);
      painter.setRenderHint( QPainter::Antialiasing );
      QPen pen(Qt::black);
      pen.setWidth(1);//это установит нужную толщину
      painter.setPen(pen);

      // Перечисляем столбцы
      for (int i = 0; i < hHeader->count(); ++i) {
        logicalIndex = hHeader->logicalIndex(i);

        // Если столбец не скрыт
        if (!hHeader->isSectionHidden(logicalIndex)) {
          // Если уже не помещаемся на страницу пошем новую страницу
          if (pageWidth <= x + hHeader->sectionSize(logicalIndex)+addtoColumnWidth) {
            verticalLines.append(QLine(x, pageTop, x, pageHeight));
            pageRights.append(x);
            x = pageLeft;
            sectionPages.append(sections);
            sections.clear();
            linePages.append(verticalLines);
            verticalLines.clear();
            widthPages.append(widths);
            widths.clear();
            pageLabels.append(labels);
            labels.clear();
          }

          // Добавляем, так как колонка видимая
          sections.append(logicalIndex);
          widths.append(hHeader->sectionSize(logicalIndex)+addtoColumnWidth);
          labels.append(qModel->headerData(logicalIndex, Qt::Horizontal).toString());
          verticalLines.append(QLine(x, pageTop, x, pageHeight));
          // Смещаемся правее на ширину колонки
          x += hHeader->sectionSize(logicalIndex)+addtoColumnWidth;
        }
      }

      if (!sections.isEmpty()) {
        verticalLines.append(QLine(x, pageTop, x, pageHeight));
        pageRights.append(x);
        x = pageLeft;
        sectionPages.append(sections);
        sections.clear();
        linePages.append(verticalLines);
        verticalLines.clear();
        widthPages.append(widths);
        widths.clear();
        pageLabels.append(labels);
        labels.clear();
      }

      int columnPageCount = sectionPages.count();
      int columnCount = sections.count();
      int rowCount = qModel->rowCount(tableView->rootIndex());
      int rowHeight;
      int columnWidth;
      QModelIndex currentIndex;
      int row = 0;
      int lastPageRow = 0;
      int columnPageIndex = 0;
      int rightColumn;

      while (true) {
        // Получаем колонки текущей страницы
        sections = sectionPages.at(columnPageIndex);
        widths = widthPages.at(columnPageIndex);
        verticalLines = linePages.at(columnPageIndex);
        // Количество колонок текущей страницы
        columnCount = sections.count();
        rightColumn = pageRights.at(columnPageIndex);
        labels = pageLabels.at(columnPageIndex);
        // Перемещаемся в левый верхний угол страницы
        x = pageLeft;
        y = pageTop;
        rowHeight = hHeader->height();

        // Рисуем колонки
        for (int i = 0; i < columnCount; ++i) {
          // Текущее значение
          columnWidth = widths.at(i)+10;
          painter.drawText(x, y, columnWidth, rowHeight
                           , currentIndex.data(Qt::TextAlignmentRole).toInt()
                           | Qt::TextWordWrap
                           | Qt::AlignVCenter
                           | Qt::AlignHCenter
                           , labels.at(i)
                          );
          painter.drawLine(pageLeft, y, rightColumn, y);
          // Перемащаемся правее
          x += widths.at(i);
        }

        y += rowHeight;
        x = pageLeft;

        // Пока можем рисовать на текущей странице
        while (true) {
          // Высота текущей строки
          rowHeight = vHeader->sectionSize(row);

          if (y + rowHeight > pageHeight) {
            --row;
            break;
          }

          // Рисуем колонки
          for (int i = 0; i < columnCount; ++i) {
            // Текущее значение
            currentIndex = qModel->index(row, sections.at(i), tableView->rootIndex());
            columnWidth = widths.at(i)+10;

            if (!currentIndex.data().isNull())
              painter.drawText(x, y, columnWidth, rowHeight
                               , currentIndex.data(Qt::TextAlignmentRole).toInt()
                               | Qt::TextWordWrap
                               | Qt::AlignVCenter
                               | Qt::AlignHCenter
                               , currentIndex.data().toString()
                              );

            painter.drawLine(pageLeft, y, rightColumn, y);
            // Перемащаемся правее
            x += widths.at(i);
          }

          x = pageLeft;
          // Перемещаемся ниже
          y += rowHeight;
          // Берем следующую строку
          ++row;

          // Если строки кончились
          if (row >= rowCount)
            break;
        }

        // Рисуем завершающую горизонтальную линию
        painter.drawLine(pageLeft, y, rightColumn, y);
        // Рисуем вертикальные линии
        painter.drawLines(verticalLines);
        // Вытираем лишние концы
        painter.fillRect(pageLeft-10, y + 1, pageWidth, pageHeight - (y + 1), QBrush(Qt::white));

        // Если предыдущие колонки не поместились
        if (columnPageIndex < columnPageCount - 1) {
          ++columnPageIndex;
          row = lastPageRow;
        // Предыдущие колонки поместились
        } else {
          columnPageIndex = 0;
          lastPageRow = row;

          // Строки закончились
          if (row >= rowCount)
            break;
        }

        // Создаем новую страницу
        printer->newPage();
      }
    }
  }
}

