#include "ext_qgraphicspixmapitem.h"

ext_qgraphicspixmapitem::ext_qgraphicspixmapitem(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{

}

void ext_qgraphicspixmapitem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
    qDebug() << "entered object";
    //this->setPixmap(highlightedPixmap);

}

void ext_qgraphicspixmapitem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    event->accept();
    qDebug() << "exited object";
    //this->setPixmap(defaultPixmap);
}

void ext_qgraphicspixmapitem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    qDebug() << "shlick!";
    this->setPixmap(highlightedPixmap);
}
