#ifndef EXT_QGRAPHICSPIXMAPITEM_H
#define EXT_QGRAPHICSPIXMAPITEM_H

#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

// класс, в котором храним для каждого объекта, который будет использоваться в карте, несколько масок;
// например территории стран, выделенные разным цветом под одной и той же по форме маске, но, которые будут меняться
// в зависимости от того, выбраны они или нет;

class region2D;

class ext_qgraphicspixmapitem : public QGraphicsPixmapItem
{
public:
    ext_qgraphicspixmapitem(QGraphicsItem *parent = nullptr);
    ext_qgraphicspixmapitem(const ext_qgraphicspixmapitem &a, QGraphicsItem *parent = nullptr);
    ~ext_qgraphicspixmapitem() { /*qDebug() << "deleted";*/; };
    void setProps(int id, QPixmap defaultPixmap, QPixmap highlightedPixmap, /*QPixmap editPixmap,*/ QColor defaultColor, int size);
    int getID() { return id; };
    int getSize() { return size; };

    region2D regPos(); // region position, width and height

private:
    int id;

    bool showOnTheMap = true;   // if region was automatically claimed as a territory, but user doesn't want it to be shown as a region - "false"
    QPixmap *currentPixmap = &defaultPixmap;
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

class region2D
{
public:
    region2D();
    ~region2D();
    void setRegion(int l, int r, int t, int b)
    {
        left = l;
        right = r;
        top = t;
        bottom = b;

        width = right - left;
        height = bottom - top;
        return;
    }
    int getWidth() { return width; }
    int getHeight() { return height; }
    int getYPos() { return top; }
    int getXPos() { return left; }

private:
    int left;
    int right;
    int top;
    int bottom;
    int width;
    int height;
};

#endif // EXT_QGRAPHICSPIXMAPITEM_H
