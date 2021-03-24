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
    xLinePos = ui->windowSeparator->pos().x();
}

MapEditor::~MapEditor()
{
    delete statusBarModeLabel;
    if (mapModel != nullptr)
        delete mapModel;

    delete ui;

}


void MapEditor::on_pushButton_clicked()
{
    //ui->mapwidget->resize(50, ui->mapwidget->size().height());
    ui->statusbar->showMessage("test");
    if (mapModel != nullptr)
        delete mapModel;
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
                                                       QCoreApplication::applicationDirPath(), "Image files (*.png *.jpg *.bmp)");
    if (pathToImage != "")
    {
        mapModel = new compositeMap(); // delete
        ui->statusbar->showMessage(pathToImage);
//        if (!mapModel->analyzeImage(pathToImage))
//        {
//            qDebug() << "founderror";
//            return;
//        }
        try {
            mapModel->analyzeImage(pathToImage);
        }  catch (std::exception ex) {
            qDebug() << "something failed";
        }
        //return;
        // отображение айтемов из списка в mapModel в графикс вью
        std::map<int,ext_qgraphicspixmapitem>::iterator showItems = mapModel->mapItems.begin();
        QGraphicsScene *map = new QGraphicsScene(ui->mapView);

        //map->setBackgroundBrush(Qt::black);
        //ext_qgraphicspixmapitem *finaltest = new ext_qgraphicspixmapitem();
        while (showItems != mapModel->mapItems.end())
        {
            //finaltest = &showItems->second;
            map->addItem(&showItems->second);
            QObject::connect(&(showItems->second.sgnl), SIGNAL(itemChosen(int)), this, SLOT(infoTable_update(int)));
            //showItems->second.setX(100); // calculate borders - then set position according to the left and top borders - ez optimization(maybe not)
            //qDebug() << showItems->second.getSize();
            showItems++;

        }
        ui->mapView->setScene(map);
        ui->mapView->setSceneRect(0, 0, mapModel->width, mapModel->height);
        ui->mapView->update(); // придумать, что делать с бэкграундом
    }
    return;
}

void MapEditor::infoTable_update(int number = 0)
{
    static bool isit = false; // данные меняются в таблице, но не отображаются
    static int firstItemID = -1;
    static QTableWidget *previousTable = mapModel->mapItems.at(number).table;
//    if (firstItemID == -1)
//    {
//        firstItemID = number; // this line can be deleted
//    }
//    else
    if (firstItemID != -1)
    {
        previousTable->close();
        previousTable = mapModel->mapItems.at(number).table;
//        QList<QTableWidget*> i1 = ui->infowidget->findChildren<QTableWidget *>();
//        if (i1.empty() == true)
//        {
//            qDebug() << "empty";
//        }
//        else
//        {
//            qDebug() << "beg ---" << i1 << "--- end";
//            //i1[0]->hide();
//            qDebug() << "closed? -"<< i1[0]->close(); // //////////////////////////
//            i1[0]->setDisabled(true);
//            i1[0]->raise();
//            //i1[0]->lower();
//        }
    }
    firstItemID = number;


    //mapModel->mapItems.at(number).table->setShowGrid(false);
    //QTableWidget *localtable = (mapModel->mapItems.at(number).table);


    //mapModel->mapItems.at(number).table->setRowCount(mapModel->mapItems.at(number).table->rowCount()+1);

    //QTableWidgetItem item00 = *(localtable->takeItem(0,0));
    //QString hehell00 = item00.text();
    //qDebug() << "text00" << hehell00;
    //QTableWidgetItem item01 = *(localtable->takeItem(0,1));
    //QString hehell01 = item01.text();
    //qDebug() << "text01" << hehell01;
    mapModel->mapItems.at(number).table->setParent(ui->infowidget);
    mapModel->mapItems.at(number).table->raise();
    mapModel->mapItems.at(number).table->show();
    //mapModel->mapItems.at(number).table->raise();
    //localtable->setVisible(true);
    mapModel->mapItems.at(number).table->update();

    return;
    /*
    std::map<int,ext_qgraphicspixmapitem>::iterator tables = mapModel->mapItems.begin();
    while(tables->first != number)
    {
        tables++;
    }
    //tables->second
    tables->second.table.setParent(ui->infowidget);
    tables->second.table.setVisible(true);
    tables->second.table.setUpdatesEnabled(true);
    QTableWidgetItem *mylord = (*tables).second.table.takeItem(0,0);

    //(*tables).second.table.itemChanged();
    tables->second.table.update();
    //(*tables).
    ui->statusbar->showMessage(QString::number(number));

    QTableWidget *helloitsme;
    */
    //ext_qgraphicspixmapitem itsme = mapModel->mapItems.at(number);
    //auto checkingit = mapModel->mapItems.at(number);
    //qDebug() << typeid(checkingit).name();
    //uncoment bottom
    /*QList<QTableWidget*> i1 = ui->infowidget->findChildren<QTableWidget *>();
    if (i1.empty() == true)
    {
        qDebug() << "empty";
    }
    else
    {
        i1[0]->close();
    }
    try
    {
        mapModel->mapItems.at(number).table.setParent(ui->infowidget);
        mapModel->mapItems.at(number).table.setVisible(true);
        if (isit == false)
        {
            isit = true;
            mapModel->mapItems.at(number).table.takeItem(0,0);
            qDebug() << "1 fail";
        }
        else
        {
            qDebug() << "2 success";
        }
        //mapModel->mapItems.at(number).table.update();
        ui->statusbar->showMessage(QString::number(number));
    }
    catch (std::out_of_range e2)
    {
        ui->statusbar->showMessage(QString::number(number));
    }*/
    //ниже комменты, убрать
    //ui->infoTable = helloitsme;
    //ui->infoTable->setParent(ui->infowidget);
//    QTableWidgetItem *it1 = new QTableWidgetItem();
//    it1->setText("test");
//    ui->infoTable->setRowCount(1);
//    ui->infoTable->setColumnCount(2);
//    ui->infoTable->setItem(0, 0, it1);
//    ui->infoTable->setVisible(true);
//    ui->infoTable->update();
    //ui->statusbar->showMessage("это пиздец.");
}

void MapEditor::mousePressEvent(QMouseEvent *mevent)
{
    xLinePos = ui->windowSeparator->pos().x();
    xMouseStart = mevent->x();
    deltaStart = xMouseStart - xLinePos;
    xMousePrev = xMouseStart;
    if (std::abs(deltaStart) > 2) // cursor changes within 2 pixels from the separator position
    {
        mevent->ignore();
        return;
    }
    dragInProgress = true;
    mevent->accept();
    return;
}

void MapEditor::mouseMoveEvent(QMouseEvent *mevent)
{
    // do not know how to set right size policies to save the ratio of manually resized widgets(from these mouse event handlers)
    // if you resize 2 widgets manually using separator drag - the next time you resize the whole window - widgets will reset to default position...
    // this shouldn't work like this, but i do not know how to fix this for now
    if (dragInProgress == false)
    {
        mevent->ignore();
        return;
    }
    mouseDelta = xMousePrev - mevent->x();

    if (mouseDelta == 0)
    {
        mevent->accept();
        return;
    }
    if (ui->infowidget->size().width() + mouseDelta < ui->infowidget->minimumWidth())
    {
        mouseDelta = ui->infowidget->size().width() - ui->infowidget->minimumWidth();
    }
    if (ui->mapwidget->size().width() - mouseDelta < ui->mapwidget->minimumWidth()) // если тянем влево, дельта > 0
    {
        mouseDelta = ui->mapwidget->size().width() - ui->mapwidget->minimumWidth();
    }

    ui->mapwidget->resize(ui->mapwidget->size().width() - mouseDelta, ui->mapwidget->size().height());
    ui->windowSeparator->move(ui->windowSeparator->x() - mouseDelta, ui->windowSeparator->y());
    ui->infowidget->resize(ui->infowidget->size().width() + mouseDelta, ui->infowidget->size().height());
    ui->infowidget->move(ui->infowidget->x() - mouseDelta, ui->infowidget->y());

    ui->centralwidget->update();
    xMousePrev = mevent->x();
    mevent->accept();
    return;
}

void MapEditor::mouseReleaseEvent(QMouseEvent *mevent)
{
    dragInProgress = false;
    mevent->accept();
    return;
}

void MapEditor::on_actiondelete_table_from_map_class_triggered()
{
    int i = 0;
    qDebug() << "chilren total" << i << "children begin >" << ui->infowidget->findChildren<QTableWidget *>() << "< children end";
    std::map<int, ext_qgraphicspixmapitem>::iterator hello = mapModel->mapItems.begin();
    while (hello != mapModel->mapItems.end())
    {
        delete(mapModel->mapItems.at(hello->second.getID()).table);
        i++;
        hello++;
    }
    qDebug() << "chilren total" << i << "children begin >" << ui->infowidget->findChildren<QTableWidget *>() << "< children end";
    //mapItems.at(number).table
}

void MapEditor::on_save_file_triggered()
{
    return;
}

void MapEditor::on_actionresize_context_triggered()
{
    QList<QTableWidget *> helloa = ui->infowidget->findChildren<QTableWidget *>();
    QTableWidget* currentone;
    while (helloa.empty() != true)
    {
        currentone = helloa.takeFirst();
        currentone->resizeRowsToContents();
        qDebug() << "popped one";
    }
}

QTableWidget* buff = nullptr;

void MapEditor::on_actionchecksaving_triggered()
{
    QImage *imag = new QImage("C:\\Users\\kuzmi\\Desktop\\testik.png");
    qDebug() << imag->data_ptr();
    imag->sizeInBytes();

    QFile file("file.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << QString("testingdatawriting");
    out << QImage(*imag);   // serialize a string
    out << QString("andalsoreading");
    out << int(322);
    file.close();

    QImage *imag2 = new QImage();
    QFile file1("file.dat");
    file1.open(QIODevice::ReadOnly);
    QDataStream out1(&file1);   // we will serialize the data into the file
    QString buff;
    out1 >> buff;
    qDebug() << buff;
    out1 >> *imag2;
    out1 >> buff;
    qDebug() << buff;
    int ibuff;
    out1 >> ibuff;
    qDebug() << ibuff;
    file1.close();

    QGraphicsScene *scene1 = new QGraphicsScene(ui->mapView);
    scene1->addPixmap(QPixmap::fromImage(*imag2));
    ui->mapView->setScene(scene1);
    ui->mapView->setSceneRect(0, 0, 1000, 1000);
    ui->mapView->update();

//    QFile file("out.txt");
//        if (!file.open(QIODevice::WriteOnly))
//            return;
//    file.write("test");
//    file.write("and");
//    file.write("again");
//    file.close();

//    while (!file.atEnd()) {
//        QByteArray line = file.readLine();
//        process_line(line);
//    }

    //QPixmap *picture = new QPixmap();
    //picture->data_ptr();

    //ui->infowidget->setEnabled(false);
    if (buff == nullptr)
        return;
    //
    //
    //ui->infowidget->
    return;
}

void MapEditor::on_actioncreatetable_triggered()
{
    QTableWidget* currentone = new QTableWidget(ui->infowidget);
    currentone->setRowCount(5);
    currentone->setColumnCount(2);
    currentone->resize(250,500);
    currentone->show();
    currentone->raise();
    buff = currentone;
    return;
}
