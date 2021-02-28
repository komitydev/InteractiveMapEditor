#ifndef EXT_QGRAPHICSPIXMAPITEM_H
#define EXT_QGRAPHICSPIXMAPITEM_H

#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

class ext_qgraphicspixmapitem : public QGraphicsPixmapItem
{
public:
    ext_qgraphicspixmapitem(QGraphicsItem *parent = nullptr);
    ~ext_qgraphicspixmapitem() {;};

private:
    QPixmap defaultPixmap;
    QPixmap highlightedPixmap;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

};

#endif // EXT_QGRAPHICSPIXMAPITEM_H
