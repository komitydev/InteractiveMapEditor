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
#include <QObject>
#include <math.h>
#include <QImage>
//#include <QFrame>
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
    int tableId = 0;
    int prevTableId;
    QTableWidget *prevtable = nullptr;

private slots:
    void on_pushButton_clicked();
    void on_enable_edit_mode_triggered();

    void on_loadSourceImage_triggered(); // зададим путь к исходному изображению для создания карты

    void infoTable_update(int number);

    void on_actiondelete_table_from_map_class_triggered();

    void on_save_file_triggered();

    void on_actionresize_context_triggered();

    void on_actionchecksaving_triggered();

    void on_actioncreatetable_triggered();

private:
    Ui::MapEditor *ui;

    //--- управление статусбаром ---
    QLabel *statusBarModeLabel;
    QString statusBarEditModeMessage = "Режим редактирования активен",
            statusBarViewModeMessage = "Режим просмотра активен";
    bool editModeOn = false;
    //------------------------------

    QString pathToImage = "kek"; // изображение для создания первоначальной версии карты
    bool savedSession = true;
    compositeMap *mapModel = nullptr;

    void rewritethis();

    void testthreads();
    void threadstart();

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
