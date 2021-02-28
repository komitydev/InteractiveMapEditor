#ifndef EXT_QGRAPHICSVIEW_H
#define EXT_QGRAPHICSVIEW_H

#include <QWidget>
#include <QScrollBar>
#include <QGraphicsView>
#include <QWheelEvent>

class ext_qgraphicsview : public QGraphicsView
{
    Q_OBJECT
public:
    ext_qgraphicsview(QWidget *parent = nullptr);
    ~ext_qgraphicsview();

private:
    QScrollBar *horSB, *verSB;
    double viewScale;

    void wheelEvent(QWheelEvent *wevent);
};

#endif // EXT_QGRAPHICSVIEW_H
