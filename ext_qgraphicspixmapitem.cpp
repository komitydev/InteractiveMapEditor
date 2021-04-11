#include "ext_qgraphicspixmapitem.h"
#include <compositemap.h>

ext_qgraphicspixmapitem::ext_qgraphicspixmapitem(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    this->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    this->setAcceptHoverEvents(true);
    this->setCursor(Qt::PointingHandCursor);
    this->setVisible(true);

    this->setPixmap(defaultPixmap); // delete this somehow

    if (table != nullptr)
    {
        delete table;
        table = nullptr; // all qtablewidgets wont be destoyed(or will they?)
    }
    table = new QTableWidget();
    table->setRowCount(1);
    table->setColumnCount(2);
    //---------------------------
    QTableWidgetItem *firstCol = new QTableWidgetItem();
    QFont forCaptions("MS Shell Dlg", 10, QFont::Bold);
    firstCol->setText("Название");
    firstCol->setFont(forCaptions);
    table->setItem(0, 0, firstCol);
    //---
    QTableWidgetItem *secondCol = new QTableWidgetItem("");
    table->setItem(0, 1, secondCol);
    //---------------------------
    table->setColumnWidth(0, 165);
    table->setColumnWidth(1, 215);
    table->setAlternatingRowColors(true);
    table->setCornerButtonEnabled(false);
    table->setSortingEnabled(false);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    table->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    table->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->setDragEnabled(false);
    table->setWordWrap(true);
    table->setTextElideMode(Qt::ElideNone);
    table->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    table->setEditTriggers(QAbstractItemView::DoubleClicked);
    return;
}

ext_qgraphicspixmapitem::ext_qgraphicspixmapitem(const ext_qgraphicspixmapitem &a, QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    this->id = a.id;
    this->defaultPixmap = a.defaultPixmap;
    this->highlightedPixmap = a.highlightedPixmap;
    this->defaultColor = a.defaultColor;
    this->size = a.size;

    this->setPixmap(defaultPixmap); // delete this somehow (set it in another function)

    this->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    this->setAcceptHoverEvents(true);
    this->setCursor(Qt::PointingHandCursor);
    this->setVisible(true);

    if (table != nullptr)
    {
        delete table;
        table = nullptr; // all qtablewidgets wont be destoyed(or will they?)
    }
    table = new QTableWidget();
    table->setRowCount(1);
    table->setColumnCount(2);
    //---------------------------
    QTableWidgetItem *firstCol = new QTableWidgetItem();
    QFont forCaptions("MS Shell Dlg", 10, QFont::Bold);
    firstCol->setText("Название");
    firstCol->setFont(forCaptions);
    table->setItem(0, 0, firstCol);
    //---
    QTableWidgetItem *secondCol = new QTableWidgetItem("");
    table->setItem(0, 1, secondCol);
    //---------------------------
    table->setColumnWidth(0, 165);
    table->setColumnWidth(1, 215);
    table->setAlternatingRowColors(true);
    table->setCornerButtonEnabled(false);
    table->setSortingEnabled(false);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    table->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    table->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->setDragEnabled(false);
    table->setWordWrap(true);
    table->setTextElideMode(Qt::ElideNone);
    table->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    table->setEditTriggers(QAbstractItemView::DoubleClicked);
    return;
}

void ext_qgraphicspixmapitem::insertFlagLabel(QString name, QPixmap imgToLabel)
{
    table->insertRow(1);
    QTableWidgetItem *firstCol = new QTableWidgetItem();
    QFont forCaptions("MS Shell Dlg", 10, QFont::Bold);
    firstCol->setText(name);
    firstCol->setFont(forCaptions);
    table->setItem(1, 0, firstCol);
    //--------------------------------------------------
    QLabel *flagLabel = new QLabel();
    flagLabel->setPixmap(imgToLabel);
    table->setCellWidget(1, 1, flagLabel);
    return;
}

void ext_qgraphicspixmapitem::setProps(int id, QPixmap defaultPixmap, QPixmap highlightedPixmap, QColor defaultColor, int size)
{
    this->id = id;
    this->defaultPixmap = defaultPixmap;
    this->highlightedPixmap = highlightedPixmap;
    this->defaultColor = defaultColor;
    this->size = size;
}

void ext_qgraphicspixmapitem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (compositeMap::getItemSelected() == true)
    {
        event->accept();
        return;
    }
    event->accept();
    this->setPixmap(highlightedPixmap);
    qDebug() << "entered";
}

void ext_qgraphicspixmapitem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (compositeMap::getItemSelected() == true)
    {
        event->accept();
        return;
    }
    event->accept();
    this->setPixmap(defaultPixmap);
    qDebug() << "exited";
}

void ext_qgraphicspixmapitem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (compositeMap::getItemSelected() == true)
    {
        if (compositeMap::getSelectedRegion() == this)
        {
            qDebug() << "same one";
            event->accept();
            return;
        }
        else
        {
            compositeMap::getSelectedRegion()->resetHighlighting(); // сперва уберем выделение с предыдущего выбранного региона
            compositeMap::setSelectedRegion(this); // запоминаем указатель на выбранный регион
        }
    }
    else
    {
        compositeMap::setSelectedRegion(this); // запоминаем указатель на выбранный регион
        compositeMap::setItemSelected(true); // меняем флаг выбора региона
    }

    QColor c1;
    c1.setNamedColor("#ffffd700");
    QBitmap cmask = defaultPixmap.mask(); // nice!
    defaultPixmap.fill(c1);
    defaultPixmap.setMask(cmask);
    //QImage test = new QImage(defaultPixmap);
    /*for (int i = 0; i < defaultPixmap.width(); i++)
    {
        for (int j = 0; j < defaultPixmap.height(); j++)
        {3
            defaultPixmap.pixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex) != blackC;
        }
    }*/
    //this->defaultPixmap.fill(c1);
    this->setPixmap(defaultPixmap); // yes
    event->accept();
    sgnl.performMessaging(id);
}

void ext_qgraphicspixmapitem::resetHighlighting()
{
//    QColor c1;
//    c1.setNamedColor("#ffffffff");
    table->close();
    QBitmap cmask = defaultPixmap.mask(); // nice!
    defaultPixmap.fill(defaultColor);
    defaultPixmap.setMask(cmask);

    this->setPixmap(defaultPixmap); // yes

    this->sgnl.resetCurrentItem(); // при отмене выделения посылаем сигнал на изменение текущего айтема
}

void ext_qgraphicspixmapitem::saveItem(QDataStream *dstream)
{
    (*dstream) << id;

    (*dstream) << this->x();
    (*dstream) << this->y();

    (*dstream) << table->columnCount();
    (*dstream) << table->rowCount();

    for (int i = 0; i < table->rowCount(); i++)
    {
        for (int j = 0; j < table->columnCount(); j++)
        {
            if (table->item(i, j) == nullptr) // try takeItem instead - doesn't save text from default widgets(or just create all of them manually)
            {
                //(*dstream) << "imekd_no_item_in_table";
                (*dstream) << (QString) "";
                continue;
            }
            if ((table->item(i, j)->text() == "Флаг") && (j == 0)) // если "флаг" в первом столбце (то есть название категории и справа флаг, а не фейк)
            {
                // но по-хорошему, это очень ненадежный костыль
                QLabel *lbl = (QLabel *) table->cellWidget(i, j+1);

                QPixmap pix = lbl->pixmap(Qt::ReturnByValue);
                (*dstream) << table->item(i, j)->text();
                (*dstream) << pix;
                break;
            }

            (*dstream) << table->item(i, j)->text();
            //qDebug() << table->item(i, j)->text();
        }
    }

    (*dstream) << showOnTheMap;
    (*dstream) << defaultPixmap;
    (*dstream) << highlightedPixmap;
    (*dstream) << grpTag;
    (*dstream) << defaultColor;
    (*dstream) << size;
    (*dstream) << groupMates;
    (*dstream) << isMainInGroup;

    return;
}

void ext_qgraphicspixmapitem::loadItem(QDataStream *dstream, int r_id)
{
    //(*dstream) >> id;
    this->id = r_id;

    qreal qrealBuf;
    (*dstream) >> qrealBuf; // x
    this->setX(qrealBuf);
    (*dstream) >> qrealBuf; // y
    this->setY(qrealBuf);

    int buf;

    (*dstream) >> buf; // table columncount
    this->table->setColumnCount(buf);
    (*dstream) >> buf; // table rowcount
    this->table->setRowCount(buf);

    QString emptyCellCheck = "imekd_no_item_in_table";
    QString bufStr;
    QFont forCaptionsOnOpening("MS Shell Dlg", 10, QFont::Bold);

    for (int i = 0; i < table->rowCount(); i++)
    {
        for (int j = 0; j < table->columnCount(); j++)
        {
            (*dstream) >> bufStr;
            if (bufStr == "Флаг" && j == 0) // костыль, но сроки поджимают
            {
                QPixmap pix;
                (*dstream) >> pix;
                this->insertFlagLabel(bufStr, pix);
                this->table->setRowCount(this->table->rowCount() - 1);
                break;
            }
//            if (bufStr == emptyCellCheck)
//                continue;
            QTableWidgetItem *bufItem = new QTableWidgetItem(bufStr);
            if (j == 0) // первый столбец всегда другим шрифтом
                bufItem->setFont(forCaptionsOnOpening);
            table->setItem(i, j, bufItem);
        }
    }

    (*dstream) >> showOnTheMap;
    (*dstream) >> defaultPixmap;
    (*dstream) >> highlightedPixmap;
    (*dstream) >> grpTag;
    (*dstream) >> defaultColor;
    (*dstream) >> size;
    (*dstream) >> groupMates;
    (*dstream) >> isMainInGroup;

    this->setPixmap(defaultPixmap); // какой же мусор, а не код

    return;
}

void ext_qgraphicspixmapitem::addRowToTable(QString left = "", QString right = "")
{
    int rowCount = table->rowCount();
    table->setRowCount(rowCount + 1);

    QTableWidgetItem *leftCol = new QTableWidgetItem();
    leftCol->setText(left);
    QTableWidgetItem *rightCol = new QTableWidgetItem();
    rightCol->setText(right);

    QFont forCaptions("MS Shell Dlg", 10, QFont::Bold);
    leftCol->setFont(forCaptions);

    table->setItem(rowCount, 0, leftCol);
    table->setItem(rowCount, 1, rightCol);

    return;
}

void ext_qgraphicspixmapitem::deleteLastRowFromTable()
{
    if(table->rowCount() <= 1)
        return;
    table->setRowCount(table->rowCount() - 1);
    return;
}

void ext_qgraphicspixmapitem::hideItem()
{
    this->setAcceptHoverEvents(false);
    this->setAcceptedMouseButtons(Qt::NoButton);
    this->unsetCursor();

    compositeMap::setSelectedRegion(nullptr); // запоминаем указатель на выбранный регион
    compositeMap::setItemSelected(false); // меняем флаг выбора региона
    showOnTheMap = false;
    return;
}

void ext_qgraphicspixmapitem::showItem()
{
    this->setAcceptHoverEvents(true);
    this->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    this->setCursor(Qt::PointingHandCursor);
    showOnTheMap = true;
    return;
}

void ext_qgraphicspixmapitem::setNewDefaultColor(QColor newColor)
{
    // сперва поработать с группами
    defaultColor = newColor;
    return;
}
