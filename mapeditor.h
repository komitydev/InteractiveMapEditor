#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QMainWindow>

//-----------------------------------
#include <QLabel>
#include <QPixmap>
#include <QAction>

#include <QDebug>
#include <windows.h>
#include <thread>
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

private:
    Ui::MapEditor *ui;

    QLabel *statusBarModeLabel;
    QString statusBarEditModeMessage = "Режим редактирования активен",
            statusBarViewModeMessage = "Режим просмотра активен";
    bool editModeOn = false;


    void rewritethis();

    void testthreads();
    void threadstart();
};
#endif // MAPEDITOR_H
