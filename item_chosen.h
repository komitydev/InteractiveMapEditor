#ifndef ITEM_CHOSEN_H
#define ITEM_CHOSEN_H

#include <QObject>

class item_chosen : public QObject
{
    Q_OBJECT
public:
    explicit item_chosen(QObject *parent = nullptr);
    void performMessaging(int a);
signals:
    void itemChosen(int b);
};

#endif // ITEM_CHOSEN_H
