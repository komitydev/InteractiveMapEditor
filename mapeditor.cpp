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

    testthreads(); // delete this
}

MapEditor::~MapEditor()
{
    delete statusBarModeLabel;
    delete ui;
}


void MapEditor::on_pushButton_clicked()
{
    ui->statusbar->showMessage("test");
}

void MapEditor::on_enable_edit_mode_triggered()
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

void MapEditor::rewritethis()
{
    QString pathToImage = QFileDialog::getOpenFileName(nullptr, "Choose an image to analyze regions",
                                                       /*QCoreApplication::applicationDirPath()*/"C:/Users/kuzmi/Desktop/Qt Projects/InteractiveMap/resources",
                                                       "Image files (*.png *.jpg *.bmp)");
    ui->linktofile->setPlainText(pathToImage);
    QImage worldMap;
    worldMap.load(pathToImage);
    worldMap = worldMap.convertToFormat(QImage::Format_RGB32, Qt::MonoOnly);
    //--------------
    checkColors(worldMap);
    //return;
    //--------------

    // if dialog is declined - it unloads the scene - fix, cuz it may cause pointer exceptions
    // еще при множественной загрузке файлов на сцену память отъедается и не выгружается
    QColor waterC; // color for water on the map
    waterC.setNamedColor("#ff00abff");
    QColor borderC;
    borderC.setNamedColor("#ff000000");

    QColor blackC;
    blackC.setNamedColor("#ff000000");
    QColor whiteC;
    whiteC.setNamedColor("#ffffffff");
    QColor blueC;
    blueC.setNamedColor("#ff0000ff");

    //----------
    blueC = waterC;
    blackC = borderC;
    //----------

    int w, h;
    w = worldMap.width();
    h = worldMap.height();

    qDebug() << "\nHeight: " << h << "\nWidth: " << w << "\n";

    class pixelOnMap
    {
    public: int widthIndex;
            int heightIndex;
    public: pixelOnMap(int wi, int he) { this->widthIndex = wi; this->heightIndex = he; }
            ~pixelOnMap() { ; }
            void showFields() { qDebug() << "\nheightIndex: " << heightIndex << "\nwidthIndex: " << widthIndex << "\n";}
    };

    int curW = 0, curH = 0;
    std::stack <pixelOnMap> currentStepPixels;
    pixelOnMap currPixel(curW, curH);
    currentStepPixels.push(currPixel);
    worldMap.setPixelColor(curW, curH, blueC);
    currPixel.showFields();

    std::stack <pixelOnMap> nextStepPixels; // will become current step pixels on the next step after stack swapping

    //----------------------------
    QGraphicsScene *map = new QGraphicsScene(ui->graphicsView);
    map->addPixmap(QPixmap::fromImage(worldMap));

    ui->graphicsView->setSceneRect(0, 0, worldMap.width(), worldMap.height());
    ui->graphicsView->setScene(map);
    //----------------------------

    while (currentStepPixels.size() != 0)
    {
        while (currentStepPixels.size() != 0)
        {
            pixelOnMap buffPixel = currentStepPixels.top();
            currentStepPixels.pop();
            //qDebug() << "wi" << buffPixel.widthIndex << "he" << buffPixel.heightIndex << "size" << currentStepPixels.size() + 1;
            // KASTILIKI! just rewrite this abomination after testing functionality
            if (buffPixel.widthIndex - 1 >= 0 && worldMap.pixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex) != blueC
                    && worldMap.pixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex) != blackC)
            {
                pixelOnMap pixel1(buffPixel.widthIndex - 1, buffPixel.heightIndex);
                nextStepPixels.push(pixel1);
                worldMap.setPixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex, blueC);
                //qDebug() << "left";
            }
            if (buffPixel.widthIndex + 1 < w && worldMap.pixelColor(buffPixel.widthIndex + 1, buffPixel.heightIndex) != blueC
                    && worldMap.pixelColor(buffPixel.widthIndex + 1, buffPixel.heightIndex) != blackC)
            {
                pixelOnMap pixel2(buffPixel.widthIndex + 1, buffPixel.heightIndex);
                nextStepPixels.push(pixel2);
                worldMap.setPixelColor(buffPixel.widthIndex + 1, buffPixel.heightIndex, blueC);
                //qDebug() << "right";
            }
            if (buffPixel.heightIndex - 1 >= 0 && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex - 1) != blueC
                    && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex - 1) != blackC)
            {
                pixelOnMap pixel3(buffPixel.widthIndex, buffPixel.heightIndex - 1);
                nextStepPixels.push(pixel3);
                worldMap.setPixelColor(buffPixel.widthIndex, buffPixel.heightIndex - 1, blueC);
                //qDebug() << "top";
            }
            if (buffPixel.heightIndex + 1 < h && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex + 1) != blueC
                    && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex + 1) != blackC)
            {
                pixelOnMap pixel4(buffPixel.widthIndex, buffPixel.heightIndex + 1);
                nextStepPixels.push(pixel4);
                worldMap.setPixelColor(buffPixel.widthIndex, buffPixel.heightIndex + 1, blueC);
                //qDebug() << "bot";
            }
            //Sleep(15000);
//            map->clear();
//            map->addPixmap(QPixmap::fromImage(worldMap));
//            ui->graphicsView->setSceneRect(0, 0, worldMap.width(), worldMap.height());
//            ui->graphicsView->setScene(map);
//            Sleep(2500);
            //qDebug() << nextStepPixels.size();
        }
        currentStepPixels.swap(nextStepPixels); // обнуленный на этом моменте currentStepPixels меняем с заполненным nextStepPixels
    }

    QImage waterMask = worldMap.createMaskFromColor(waterC.rgb(), Qt::MaskOutColor);
    QPixmap *waterPixmap = new QPixmap(QPixmap::fromImage(worldMap));
    QBitmap *waterBitmap = new QBitmap(QPixmap::fromImage(waterMask));
    waterPixmap->setMask(*waterBitmap);
    //waterBitmap->setMask()

    //worldMap.setPixelColor(0, 0, color);
    //QPixmap worldPixMap;
    //worldPixMap.fromImage(worldMap);

    //QGraphicsScene *map1 = new QGraphicsScene(ui->graphicsView);

    //works
    //QGraphicsPixmapItem *maskTest = new QGraphicsPixmapItem();
    qgraphicspixmapitem_mityan *maskTest = new qgraphicspixmapitem_mityan();
    //maskTest->installSceneEventFilter()
    //maskTest->event
    maskTest->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    maskTest->setAcceptHoverEvents(true);
    //maskTest->acceptHoverEvents();
    //maskTest->setPixmap(QPixmap::fromImage(waterMask)); // CHANGED HERE!
    maskTest->setPixmap(*waterPixmap); // CHANGED HERE!
    maskTest->defaultPixmap = *waterPixmap;
    maskTest->highlightedPixmap = *waterPixmap;

    //-----------
    QColor blueC1;
    blueC1.setNamedColor("#ff0000ff");
    //-----------

    maskTest->highlightedPixmap.fill(blueC1);
    maskTest->setCursor(Qt::ForbiddenCursor);
    map->addItem(maskTest);
    ui->graphicsView->setSceneRect(0, 0, worldMap.width(), worldMap.height());
    ui->graphicsView->setScene(map);
    return;

    map->addPixmap(QPixmap::fromImage(worldMap));
    //map->clear();
    ui->graphicsView->setSceneRect(0, 0, worldMap.width(), worldMap.height());
    ui->graphicsView->setScene(map);
    //map->setSceneRect(0, 0, worldMap.width(), worldMap.height());

    //ui->graphicsView->resetTransform(); // 1
    //ui->graphicsView->show();
}
