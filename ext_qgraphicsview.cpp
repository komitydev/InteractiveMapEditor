#include "ext_qgraphicsview.h"

ext_qgraphicsview::ext_qgraphicsview(QWidget *parent) : QGraphicsView(parent)
{
    verSB = new QScrollBar(Qt::Vertical);
    horSB = new QScrollBar(Qt::Horizontal);
    this->setHorizontalScrollBar(horSB);
    this->setVerticalScrollBar(verSB);
}

ext_qgraphicsview::~ext_qgraphicsview()
{
    delete(horSB);
    delete(verSB);
}

void ext_qgraphicsview::wheelEvent(QWheelEvent *wevent)
{
    if (wevent->angleDelta().y() == 0)
    {
        wevent->ignore();
        return;
    }

    int sign = (wevent->angleDelta().y() < 0) ? (1):(-1); // wheel up or wheel down (should've been the opposite, but now as in standard version)

    if (wevent->modifiers() == Qt::ControlModifier)
    {
        if (sign < 0) // we zoom in, so viewScale will be greater than 1 now
        {
            if (this->transform().m11() * 1.1 > 10) // current value - default equals 1
                viewScale = 1;
            else
                viewScale = 1.1;
        }
        else
        {
            if (this->transform().m11() * 0.9 < 0.1)
                viewScale = 1;
            else
                viewScale = 0.9;
            // посмотреть дефолтный размер сцены, которая загружается в вью или сам активный размер вью и сравнить это как-то с тем
            // какой размер карты загружается во вью - относительно этого динамически задавать ограничения масштаба
            // иначе будет костыль, который будет плохо работать на больших или маленьких изображениях, отличных от того, на котором
            // провожу тесты я
        }
        //qDebug() << this->transform().m11(); // qtransform m11 and m22 for scaling
        //qDebug() << this->transform().m22();
        this->scale(viewScale, viewScale); // i scale view with the same ratio, so i use only one matrix scale value as they are equal
        wevent->accept();
        return;
    }

    if (wevent->modifiers() == Qt::ShiftModifier)
        horSB->setValue(horSB->value()+(sign)*(horSB->width()/20));
    else
        verSB->setValue(verSB->value()+(sign)*(verSB->height()/20));

    wevent->accept();
    return;
}

void ext_qgraphicsview::keyPressEvent (QKeyEvent *kevent)
{
    if (kevent->key() != Qt::Key_Escape)
    {
        kevent->ignore();
        return;
    }

    //this->setVisible(false);
    kevent->accept();
    return;
}
