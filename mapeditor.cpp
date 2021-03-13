#include "mapeditor.h"
#include "ui_mapeditor.h"

MapEditor::MapEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MapEditor)
{
    ui->setupUi(this);

    statusBarModeLabel = new QLabel (statusBarViewModeMessage);
    statusBarModeLabel->setText(statusBarViewModeMessage);
    ui->statusbar->addPermanentWidget(statusBarModeLabel);

    mapModel = new compositeMap(); //

    testthreads(); // delete this
}

MapEditor::~MapEditor()
{
    delete statusBarModeLabel;
    delete mapModel;

    delete ui;

}


void MapEditor::on_pushButton_clicked()
{
    ui->statusbar->showMessage("test");
}

void MapEditor::on_enable_edit_mode_triggered() // управление статусбаром
{
    QAction* edit_mode = qobject_cast<QAction*>(sender());
    if (edit_mode->isChecked())
    {
        editModeOn = true;
        statusBarModeLabel->setText(statusBarEditModeMessage);
    }
    else
    {
        editModeOn = false;
        statusBarModeLabel->setText(statusBarViewModeMessage);
    }
    return;
}

void MapEditor::testthreads() // will use this method to calculate regions without blocking user interface
{
    ui->progressBar->setValue(0); // will have to show placeholder instead of graphics view and after function will do its work - show target content
    std::thread tryit(&MapEditor::threadstart, this);
    qDebug() << "started";
    tryit.detach();
    qDebug() << "detached";
}

void MapEditor::threadstart()
{
    Sleep(10000);
    ui->progressBar->setValue(100);
    qDebug() << "finished";
}

// класс для хранения текущего открытого файла - путь, флаг сохранения объект класса, хранящий полную сущность "карта"
// при изменении чего-либо в режиме редактирования - изменяем объект этой сущности
// в классе экземпляра интерфейса создаем объект вышеупомянутого класса. при нажатии на кнопку(загрузить),
// начинается пиздец, который заебешься программировать

// посмотреть слот close() в мейн окне - через дебаг проследить, будет ли он вызываться при нажатии на крестик, через диспетчер задач, через меню
// и через альтф4


void MapEditor::on_loadSourceImage_triggered()
{
    QString pathToImage = QFileDialog::getOpenFileName(nullptr, "Choose an image to analyze regions",
                                                       QCoreApplication::applicationDirPath()
                                                       /*"C:/Users/kuzmi/Desktop/Qt Projects/InteractiveMap/resources"*/,
                                                       "Image files (*.png *.jpg *.bmp)");
    // не работает открытие файлы с кириллицей

    if (pathToImage != "")
    {
        ui->statusbar->showMessage(pathToImage);
        mapModel->analyzeImage(pathToImage);
        // отображение айтемов из списка в mapModel в графикс вью
        std::map<int,ext_qgraphicspixmapitem>::iterator showItems = mapModel->mapItems.begin();
        QGraphicsScene *map = new QGraphicsScene(ui->mapView);

        //map->setBackgroundBrush(Qt::black);
        //ext_qgraphicspixmapitem *finaltest = new ext_qgraphicspixmapitem();
        while (showItems != mapModel->mapItems.end())
        {
            //finaltest = &showItems->second;
            map->addItem(&showItems->second);
            //showItems->second.setX(100); // calculate borders - then set position according to the left and top borders - ez optimization(maybe not)
            qDebug() << showItems->second.getSize();
            showItems++;

        }
        ui->mapView->setScene(map);
        ui->mapView->setSceneRect(0, 0, mapModel->width, mapModel->height);
        ui->mapView->show(); // придумать, что делать с бэкграундом
    }
    return; // check what happens when you cancel dialog
}
