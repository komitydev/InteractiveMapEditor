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
#include <cheatClass.h>
#include <QHeaderView>
#include <QSizePolicy>
#include <QKeyEvent>

class ext_qgraphicspixmapitem : public QGraphicsPixmapItem
{
public:
    ext_qgraphicspixmapitem(QGraphicsItem *parent = nullptr);
    ext_qgraphicspixmapitem(const ext_qgraphicspixmapitem &a, QGraphicsItem *parent = nullptr);
    void setProps(int id, QPixmap defaultPixmap, QPixmap highlightedPixmap, /*QPixmap editPixmap,*/ QColor defaultColor, int size);
    int getID() { return id; };
    int getSize() { return size; };
    cheatClass sgnl;
    QTableWidget *table = nullptr;

    void resetHighlighting();

    void saveItem(QDataStream *dstream);
    void loadItem(QDataStream *dstream, int r_id);

    bool getShowOnTheMap() { return showOnTheMap; }
    void hideItem();
    void showItem();

    QColor getDefaultColor() { return defaultColor; }

    void addRowToTable(QString left, QString right);
    void deleteLastRowFromTable();
    void setNewDefaultColor(QColor newColor);

    QString getGroupTag() { return grpTag; }
    void setGroupTag(QString newGroup) { grpTag = newGroup; }

    void setIsMainInGroup(bool isMain) { isMainInGroup = isMain; }

private:
    // we save all these parameters into file + table
    int id;
    bool showOnTheMap = true;   // if region was automatically claimed as a territory, but user doesn't want it to be shown as a region - "false"
    QPixmap defaultPixmap;      // pixmap color that is shown by default
    QPixmap highlightedPixmap;  // pixmap color when selected or under cursor (all are supposed to be maybe golden with some kind of a border)
    QString grpTag = "no_group";             // if some of the regions needed to be connected to some other regions - this will be used
    QColor defaultColor;
    int size;
    QList<int> groupMates;
//    std::list<int> groupMates;
    bool isMainInGroup = true;
    // we save all these parameters into file

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    // need method "highlight" and something to return to non-highlighted state
};

#endif // EXT_QGRAPHICSPIXMAPITEM_H
