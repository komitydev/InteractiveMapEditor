#include "ext_qgraphicspixmapitem.h"

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
    table->setRowCount(30);
    table->setColumnCount(2);

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
    table->setRowCount(30);
    table->setColumnCount(2);

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
    event->accept();
    this->setPixmap(highlightedPixmap);
}

void ext_qgraphicspixmapitem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
    this->setPixmap(defaultPixmap);
}

void ext_qgraphicspixmapitem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QColor c1;
    c1.setNamedColor("#ffffffff");
    QBitmap cmask = defaultPixmap.mask(); // nice!
    defaultPixmap.fill(c1);
    defaultPixmap.setMask(cmask);
    //QImage test = new QImage(defaultPixmap);
    /*for (int i = 0; i < defaultPixmap.width(); i++)
    {
        for (int j = 0; j < defaultPixmap.height(); j++)
        {
            defaultPixmap.pixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex) != blackC;
        }
    }*/
    //this->defaultPixmap.fill(c1);
    event->accept();
    sgnl.performMessaging(id);
}
