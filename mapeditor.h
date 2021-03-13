#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QMainWindow>

//-----------------------------------
#include <QLabel>
#include <QPixmap>
#include <QAction>
#include <QFileDialog>

#include <QDebug>
#include <windows.h>
#include <thread>
#include "compositemap.h"
//-----------------------------------

QT_BEGIN_NAMESPACE
namespace Ui { class MapEditor; }
QT_END_NAMESPACE

class MapEditor : public QMainWindow
{
    Q_OBJECT

public:
    MapEditor(QWidget *parent = nullptr);
    ~MapEditor();

private slots:
    void on_pushButton_clicked();
    void on_enable_edit_mode_triggered();

    void on_loadSourceImage_triggered(); // зададим путь к исходному изображению для создания карты

private:
    Ui::MapEditor *ui;

    //--- управление статусбаром ---
    QLabel *statusBarModeLabel;
    QString statusBarEditModeMessage = "Режим редактирования активен",
            statusBarViewModeMessage = "Режим просмотра активен";
    bool editModeOn = false;
    //------------------------------

    QString pathToImage = "kek"; // изображение для создания первоначальной карты версии карты
    bool savedSession = true;
    compositeMap *mapModel;

    void rewritethis();

    void testthreads();
    void threadstart();
};
#endif // MAPEDITOR_H
