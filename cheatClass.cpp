#include "cheatClass.h"

cheatClass::cheatClass(QObject *parent) : QObject(parent)
{
    return;
}

void cheatClass::performMessaging(int a)
{
    emit this->itemChosen(a);
    return;
}

void cheatClass::loadingPerformed(int a)
{
    emit this->loadingStep(a);
    return;
}
