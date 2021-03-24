#include "item_chosen.h"

item_chosen::item_chosen(QObject *parent) : QObject(parent)
{

}

void item_chosen::performMessaging(int a)
{
    emit this->itemChosen(a);
}
