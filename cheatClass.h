#ifndef CHEATCLASS_H
#define CHEATCLASS_H

#include <QObject>

class cheatClass : public QObject // this class will contain signals for classes in which i don't know yet how to implement signal system
{
    Q_OBJECT
public:
    explicit cheatClass(QObject *parent = nullptr);
    void performMessaging(int a);
    void loadingPerformed(int a);
    void resetCurrentItem();
signals:
    void itemChosen(int b);
    void loadingStep(int b);
    void resetCurrentItemSignal();
};

#endif // CHEATCLASS_H
