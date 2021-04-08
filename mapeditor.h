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
//#include <thread>
#include "compositemap.h"
#include <QObject>
#include <math.h>
#include <QImage>
//#include <QFrame>
#include <QColorDialog>
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
    void on_enable_edit_mode_triggered();
    void on_loadSourceImage_triggered(); // зададим путь к исходному изображению для создания карты
    void infoTable_update(int number);
    void on_actiondelete_table_from_map_class_triggered();
    void on_save_file_triggered();
    void on_actionchecksaving_triggered();
    void on_LoadingProgressBar_valueChanged(int value);

    void on_open_file_triggered();

    void resetCurrentItemSlot();

    void on_deleteLastRow_clicked();

    void on_addLastRow_clicked();

    void on_makeRegionInactive_clicked();

    void on_colorDialogButton_clicked();

    void on_createNewGroup_clicked();

    void on_putCurrentRegionIntoGroup_clicked();

    void on_makeRegionActive_clicked();

private:
    Ui::MapEditor *ui;

    int currentSelectedItem = -1; // id in item map
    void loadEditPanel(); // загружает в edit panel информацию о текущем выбранном регионе
    //--- управление статусбаром ---
    QLabel *statusBarModeLabel;
    QString statusBarEditModeMessage = "Режим редактирования активен",
            statusBarViewModeMessage = "Режим просмотра активен";
    bool editModeOn = false;
    //------------------------------
    void finishMapAnalyzing();
    QTableWidget *prevtable = nullptr;

    QString pathToImage = ""; // изображение для создания первоначальной версии карты
    compositeMap *mapModel = nullptr;

    // map and info widgets resizing
    bool dragInProgress = false;
    int xLinePos;
    int xMouseStart; // position of the drag start to calculate delta
    int deltaStart; // difference between starting point of the drag and the line position
    int xMousePrev; // previous mouse X pos
    int mouseDelta;
    void mousePressEvent(QMouseEvent *mevent);
    void mouseMoveEvent(QMouseEvent *mevent);
    void mouseReleaseEvent(QMouseEvent *mevent);
};
#endif // MAPEDITOR_H
