#include "ext_qgraphicspixmapitem.h"

ext_qgraphicspixmapitem::ext_qgraphicspixmapitem(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    this->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    this->setAcceptHoverEvents(true);
    this->setCursor(Qt::PointingHandCursor);
    this->setVisible(true);
    this->setPixmap(*currentPixmap);
}

ext_qgraphicspixmapitem::ext_qgraphicspixmapitem(const ext_qgraphicspixmapitem &a, QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    this->id = a.id;
    this->defaultPixmap = a.defaultPixmap;
    this->highlightedPixmap = a.highlightedPixmap;
    this->editPixmap = a.editPixmap;
    this->defaultColor = a.defaultColor;
    this->size = a.size;

    this->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    this->setAcceptHoverEvents(true);
    this->setCursor(Qt::PointingHandCursor);
    this->setVisible(true);
    this->setPixmap(*currentPixmap);
}

void ext_qgraphicspixmapitem::setProps(int id, QPixmap defaultPixmap, QPixmap highlightedPixmap, /*QPixmap editPixmap,*/ QColor defaultColor, int size)
{
    this->id = id;
    this->defaultPixmap = defaultPixmap;
    this->highlightedPixmap = highlightedPixmap;
    this->editPixmap = editPixmap;
    this->defaultColor = defaultColor;
    this->size = size;
}

void ext_qgraphicspixmapitem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
    qDebug() << "entered object";
    this->setPixmap(highlightedPixmap);

}

void ext_qgraphicspixmapitem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
    qDebug() << "exited object";
    this->setPixmap(defaultPixmap);
}

void ext_qgraphicspixmapitem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    qDebug() << "shlick!";
    //this->setPixmap(highlightedPixmap);
}
