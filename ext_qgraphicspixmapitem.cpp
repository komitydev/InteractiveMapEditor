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
    table->setItem(1,1,firstCol);
    //table->item(1,1)->te
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
    table->setItem(0,0,firstCol);
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
            (*dstream) << table->item(i, j)->text();
            //qDebug() << table->item(i, j)->text();
        }
    }

    (*dstream) << showOnTheMap;
    (*dstream) << defaultPixmap;
    (*dstream) << highlightedPixmap;
    (*dstream) << grpId;
    (*dstream) << defaultColor;
    (*dstream) << size;

    return;
}

void ext_qgraphicspixmapitem::loadItem(QDataStream *dstream)
{
    (*dstream) >> id;

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

    for (int i = 0; i < table->rowCount(); i++)
    {
        for (int j = 0; j < table->columnCount(); j++)
        {
            (*dstream) >> bufStr;
//            if (bufStr == emptyCellCheck)
//                continue;
            QTableWidgetItem *bufItem = new QTableWidgetItem(bufStr);
            table->setItem(i, j, bufItem);
        }
    }

    (*dstream) >> showOnTheMap;
    (*dstream) >> defaultPixmap;
    (*dstream) >> highlightedPixmap;
    (*dstream) >> grpId;
    (*dstream) >> defaultColor;
    (*dstream) >> size;

    return;
}
