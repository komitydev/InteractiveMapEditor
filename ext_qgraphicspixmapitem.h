#ifndef EXT_QGRAPHICSPIXMAPITEM_H
#define EXT_QGRAPHICSPIXMAPITEM_H

#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QDebug>
#include <QBitmap>
#include <item_chosen.h>
#include <QHeaderView>

class ext_qgraphicspixmapitem : public QGraphicsPixmapItem
{
public:
    ext_qgraphicspixmapitem(QGraphicsItem *parent = nullptr);
    ext_qgraphicspixmapitem(const ext_qgraphicspixmapitem &a, QGraphicsItem *parent = nullptr);
    ~ext_qgraphicspixmapitem();
    void setProps(int id, QPixmap defaultPixmap, QPixmap highlightedPixmap, /*QPixmap editPixmap,*/ QColor defaultColor, int size);
    int getID() { return id; };
    int getSize() { return size; };
    item_chosen sgnl;
    QTableWidget *table = nullptr;

private:
    int id;

    bool showOnTheMap = true;   // if region was automatically claimed as a territory, but user doesn't want it to be shown as a region - "false"
    QPixmap defaultPixmap;      // pixmap color that is shown by default
    QPixmap highlightedPixmap;  // pixmap color when selected or under cursor (all are supposed to be maybe golden with some kind of a border)
    QPixmap editPixmap;         // pixmap color in edit mode (all are supposed to be green)

    QColor defaultColor;
    int size;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    // need method "highlight" and something to return to non-highlighted state
};

#endif // EXT_QGRAPHICSPIXMAPITEM_H
