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

    xLinePos = ui->windowSeparator->pos().x();

    ui->loadingLabel->setVisible(false);
    ui->LoadingProgressBar->setVisible(false);
    ui->mapView->setEnabled(false);

    //--- right panel setup ---
    ui->visualseparator3->setVisible(false);
    ui->regionGroupsControl->setVisible(false); // спрятать панельку с манипуляцией группами, так как пока что она не нужна
    ui->menudebug->setTitle(""); // hiding qmenu in menubar
    ui->selectQuizWidget->hide(); // виджет, который позволит играть в викторину с выбором указанного региона
    ui->guessQuizWidget->hide(); // виджет, который позволит играть в викторину с выбором правильного ответа из нескольких вариантов
    ui->editPanelWidget->hide(); // виджет позволяет настроить регионы (убрать или вернуть регионы в активные и настроить цвета)
    ui->regionCaptionWidget->hide(); // название выбранного региона - будет задаваться из соответствующего поля таблицы
}

MapEditor::~MapEditor()
{
    delete statusBarModeLabel;
    if (mapModel != nullptr)
    {
        delete mapModel;
    }
    delete ui;
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

    // or of course i know - inherit widget class and reimplement update slot (or another one) - will fix this later(have some more ideas)
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

// ----------- image analyzing start -----------
// the whole code part was set up to work with threads, but then i realised, that there are a lot to do except of this,
// so i returned to an old version (at least until i will have some time to deal with this)
void MapEditor::on_loadSourceImage_triggered()
{
    QString pathToImage = QFileDialog::getOpenFileName(nullptr, "Choose an image to analyze regions",
                                                       QCoreApplication::applicationDirPath(), "Image files (*.png *.jpg *.bmp)");
    if (pathToImage != "")
    {
        mapModel = new compositeMap(); // delete
        ui->statusbar->showMessage(pathToImage);
        ui->loadingLabel->setVisible(true);
        ui->LoadingProgressBar->setVisible(true);
        QObject::connect(&(mapModel->sgnl_obj), SIGNAL(loadingStep(int)), this, SLOT(on_LoadingProgressBar_valueChanged(int)));

        // analyzing in another thread
        // std::thread analyzeImageThread(&compositeMap::analyzeImage, mapModel, pathToImage);
        // analyzeImageThread.detach();
        mapModel->analyzeImage(pathToImage); // :) разумеется, ничто не могло быть так просто
        if (mapModel->analyzingSuccessful == false)
        {
            ui->statusbar->showMessage("При анализе изображения произошла ошибка или изображение не удается загрузить...");
        }
    }
    return;
}

void MapEditor::on_LoadingProgressBar_valueChanged(int value)
{
    ui->LoadingProgressBar->setValue(value);
    ui->LoadingProgressBar->update();
    if (value == 100)
    {
        QObject::disconnect(&(mapModel->sgnl_obj), SIGNAL(loadingStep(int)), this, SLOT(on_LoadingProgressBar_valueChanged(int))); // надо ли?
        finishMapAnalyzing();
    }
    return;
}

void MapEditor::finishMapAnalyzing()
{
    std::map<int,ext_qgraphicspixmapitem>::iterator showItems = mapModel->mapItems.begin();
    QGraphicsScene *map = new QGraphicsScene(ui->mapView);

    while (showItems != mapModel->mapItems.end())
    {
        map->addItem(&showItems->second);
        QObject::connect(&(showItems->second.sgnl), SIGNAL(itemChosen(int)), this, SLOT(infoTable_update(int))); // пока что при удалении композит мапы сигналы не разрываются
        QObject::connect(&(showItems->second.sgnl), SIGNAL(resetCurrentItemSignal()), this, SLOT(resetCurrentItemSlot())); // изменить слот
        showItems++;
    }

    ui->mapView->setScene(map);
    ui->mapView->setSceneRect(0, 0, mapModel->width, mapModel->height);
    ui->mapView->update(); // придумать, что делать с бэкграундом

    ui->loadingLabel->setVisible(false);
    ui->LoadingProgressBar->setVisible(false);
    ui->mapView->setEnabled(true);
    return;
}
// ----------- image analyzing end -----------

void MapEditor::on_enable_edit_mode_triggered() // управление статусбаром
{
    QAction* edit_mode = qobject_cast<QAction*>(sender());
    if (mapModel == nullptr)
    {
        edit_mode->setChecked(false);
        return;
    }
    if (edit_mode->isChecked())
    {
        editModeOn = true;
        ui->editPanelWidget->show();
        if (currentSelectedItem != -1)
        {
            mapModel->mapItems.at(currentSelectedItem).table->setEditTriggers(QAbstractItemView::DoubleClicked);
            //mapModel->mapItems.at(currentSelectedItem).table->close();
        }
        statusBarModeLabel->setText(statusBarEditModeMessage);
    }
    else
    {
        editModeOn = false;
        ui->editPanelWidget->hide();
        if (currentSelectedItem != -1)
        {
            mapModel->mapItems.at(currentSelectedItem).table->setEditTriggers(QAbstractItemView::NoEditTriggers);
            //mapModel->mapItems.at(currentSelectedItem).table->close();
        }
        statusBarModeLabel->setText(statusBarViewModeMessage);
    }
    return;
}

void MapEditor::resetCurrentItemSlot()
{
    currentSelectedItem = -1;
    ui->editPanelWidget->hide();
    //ui->statusbar->showMessage(QString::number(currentSelectedItem));
    return;
}
// посмотреть слот close() в мейн окне - через дебаг проследить, будет ли он вызываться при нажатии на крестик, через диспетчер задач, через меню
// и через альтф4

void MapEditor::infoTable_update(int number = 0)
{
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

    currentSelectedItem = number;
    loadEditPanel(); // при выборе нового региона загружаем информацию в панель
    if (editModeOn == true)
    {
        ui->editPanelWidget->show();
        mapModel->mapItems.at(currentSelectedItem).table->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
    else
    {
        ui->editPanelWidget->hide();
        mapModel->mapItems.at(currentSelectedItem).table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    //ui->statusbar->showMessage(QString::number(currentSelectedItem));


    //qDebug() << mapModel->mapItems.at(number).table->editTriggers();
    //mapModel->mapItems.at(number).table->setEditTriggers(QAbstractItemView::NoEditTriggers); // EXCELLENT!

    //mapModel->mapItems.at(number).table->setShowGrid(false);
    //QTableWidget *localtable = (mapModel->mapItems.at(number).table);


    //mapModel->mapItems.at(number).table->setRowCount(mapModel->mapItems.at(number).table->rowCount()+1);

    //QTableWidgetItem item00 = *(localtable->takeItem(0,0));
    //QString hehell00 = item00.text();
    //qDebug() << "text00" << hehell00;
    //QTableWidgetItem item01 = *(localtable->takeItem(0,1));
    //QString hehell01 = item01.text();
    //qDebug() << "text01" << hehell01;
    mapModel->mapItems.at(number).table->setParent(ui->infoTableWidget);
    mapModel->mapItems.at(number).table->resizeRowsToContents();
    ui->infoTableWidget->layout()->addWidget(mapModel->mapItems.at(number).table);

//    mapModel->mapItems.at(number).table->move(ui->infowidget->x(), ui->infowidget->y());
//    mapModel->mapItems.at(number).table->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    //mapModel->mapItems.at(number).table->setGeometry(ui->infowidget->x() - mapModel->mapItems.at(number).table->x(),
    //                                                 ui->infowidget->y() - mapModel->mapItems.at(number).table->y(),
    //                                                 ui->infowidget->width(), ui->infowidget->height());
    //mapModel->mapItems.at(number).table->adjustSize();
    //mapModel->mapItems.at(number).table->adjustSize(); // ????????????????????????????????????????????????????????????????????????????????????????????
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
}

void MapEditor::loadEditPanel()
{
    static QPixmap localColor(30, 30);
    ext_qgraphicspixmapitem *itemRef = &mapModel->mapItems.at(currentSelectedItem);
    localColor.fill(itemRef->getDefaultColor());
    ui->colorPreviewPixmap->setPixmap(localColor);
    //--------------------------------------------
    static QString colorLabelTextBase = " - текущий цвет выбранного региона в формате RGB: ";
    ui->colorPreviewLabel->setText(colorLabelTextBase + itemRef->getDefaultColor().name(QColor::HexRgb));
    //--------------------------------------------
    ui->groupList->setCurrentIndex(ui->groupList->findText(itemRef->getGroupTag()));
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
    return;
}

void MapEditor::on_save_file_triggered()
{
    QString pathToImage = QFileDialog::getSaveFileName(nullptr, "Choose the name for the file you want to save",
                                                       QCoreApplication::applicationDirPath(), "InteractiveMapEditor (*.imekd)");
    if (pathToImage == "")
        return;
    if (mapModel == nullptr)
        return;
    if (mapModel->width == 0 || mapModel->height == 0)
        return;

    QFile file(pathToImage);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    QString fileIdentificator = "|InteractiveMapEditorv04042021_file|";

    // saving mapModel
    out << fileIdentificator;
    out << mapModel->width;
    out << mapModel->height;

    std::map<int,ext_qgraphicspixmapitem>::iterator tables = mapModel->mapItems.begin();
    out << (int) mapModel->mapItems.size(); // преобразуем в целое
    while (tables != mapModel->mapItems.end())
    {
        (*tables).second.saveItem(&out);
        tables++;
    }

    std::map<QString, bool>::iterator colors = mapModel->usedColors.begin();
    out << (int) mapModel->usedColors.size(); // преобразуем в целое
    while (colors != mapModel->usedColors.end())
    {
        out << (*colors).first;
        colors++;
    }
    // finished saving

    file.close();
    return;
}

void MapEditor::on_open_file_triggered()
{
    QString pathToImage = QFileDialog::getOpenFileName(nullptr, "Choose the map file you want to open",
                                                       QCoreApplication::applicationDirPath(), "InteractiveMapEditor (*.imekd)");
    if (pathToImage == "")
        return;
    if (mapModel != nullptr)
    {
        ui->statusbar->showMessage("В данный момент в памяти уже находится версия модели - сперва сохраните ее, прежде чем открывать новый файл.");
        return;
    }
    mapModel = new compositeMap();

    QFile file(pathToImage);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    QString fileIdentificator = "|InteractiveMapEditorv04042021_file|";
    QString buf;

    in >> buf;

    if (buf != fileIdentificator)
    {
        file.close();
        ui->statusbar->showMessage("Ошибка при открытии файла. Возможно файл не был создан в данной программе.");
        delete mapModel;
        return;
    }
    in >> mapModel->width;
    in >> mapModel->height;
    int itemMapSize;
    in >> itemMapSize;

    for (int i = 0; i < itemMapSize; i++)
    {
        ext_qgraphicspixmapitem *newItem = new ext_qgraphicspixmapitem();
        int r_id; // read id
        in >> r_id;

        try
        {
            mapModel->mapItems.at(r_id).x();
        }
        catch (std::out_of_range &e2)
        {
            mapModel->mapItems.insert({ r_id, *newItem });
            ext_qgraphicspixmapitem *lref = &(mapModel->mapItems.at(r_id));
            lref->loadItem(&in, r_id);
            if (lref->getShowOnTheMap() == false)
            {
                lref->setAcceptHoverEvents(false);
                lref->setAcceptedMouseButtons(Qt::NoButton);
                lref->unsetCursor();
            }
//            ext_qgraphicspixmapitem lref = &(mapModel->mapItems.at(r_id));
//            lref.loadItem(&out, r_id);
//            if (lref.getShowOnTheMap() == false)
//            {
//                lref.setAcceptHoverEvents(false);
//                lref.setAcceptedMouseButtons(Qt::NoButton);
//                lref.unsetCursor();
//            }
        }

        delete newItem;
    }

    int colorMapSize;
    in >> colorMapSize;

    QString bufColor;

    for (int i = 0; i < colorMapSize; i++)
    {
        in >> bufColor;
        try // вообще список использованных цветов надо было бы сделать не картой
        {
            mapModel->usedColors.at(bufColor) = true; // костылик
        }
        catch (std::out_of_range &e2)
        {
            mapModel->usedColors.insert({ bufColor, true });
        }
    }

    file.close();
    finishMapAnalyzing();
    return;
}

void MapEditor::on_actionchecksaving_triggered()
{
    QImage *imag = new QImage("C:\\Users\\kuzmi\\Desktop\\testik.png");
    qDebug() << imag->data_ptr();
    imag->sizeInBytes();

    QFile file("file.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << QString("testingdatawriting");
    out << *imag;   // serialize a string
    out << QString("andalsoreading");
    out << int(322);
    out << QPixmap::fromImage(*imag);
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
    QPixmap justchecking;
    out1 >> justchecking;
    file1.close();

    QGraphicsScene *scene1 = new QGraphicsScene(ui->mapView);
    //scene1->addPixmap(QPixmap::fromImage(*imag2));
    scene1->addPixmap(justchecking);
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

void MapEditor::on_deleteLastRow_clicked()
{
    if (currentSelectedItem == -1)
        return;
    mapModel->mapItems.at(currentSelectedItem).deleteLastRowFromTable();
    return;
}

void MapEditor::on_addLastRow_clicked()
{
    if (currentSelectedItem == -1)
        return;
    mapModel->mapItems.at(currentSelectedItem).addRowToTable("", "");
    return;
}

void MapEditor::on_makeRegionInactive_clicked()
{
    if (currentSelectedItem == -1)
        return;
    mapModel->inactiveRegions.push_back(currentSelectedItem);
    QString regionName = mapModel->mapItems.at(currentSelectedItem).table->item(0,1)->text();
    if (regionName == "")
        regionName = "-";
    ui->inactiveRegionsList->addItem("ID: " + QString::number(currentSelectedItem) + " ; Название: "
            + regionName + " ; Размер: " + QString::number(mapModel->mapItems.at(currentSelectedItem).getSize()));
    mapModel->mapItems.at(currentSelectedItem).setGroupTag("no_group"); // чтобы разорвать связь с регионами
    mapModel->mapItems.at(currentSelectedItem).setIsMainInGroup(true);
    // нужно учесть, что если груп айди не равен -1, то надо разорвать связь с группой, назначить другой "главный" регион, скопировать таблицу и т.д.

    mapModel->mapItems.at(currentSelectedItem).hideItem();
    mapModel->mapItems.at(currentSelectedItem).resetHighlighting();
    return;
}

void MapEditor::on_colorDialogButton_clicked()
{
    if (currentSelectedItem == -1)
        return;
    QColor colorChoice = QColorDialog::getColor(Qt::white, this, "Выберите новый цвет для текущего региона");
    if (!colorChoice.isValid())
        return;
    mapModel->mapItems.at(currentSelectedItem).setNewDefaultColor(colorChoice);

    QPixmap localColor(30, 30);
    localColor.fill(colorChoice);
    ui->colorPreviewPixmap->setPixmap(localColor);
    //--------------------------------------------
    static QString colorLabelTextBase = " - текущий цвет выбранного региона в формате RGB: ";
    ui->colorPreviewLabel->setText(colorLabelTextBase + colorChoice.name(QColor::HexRgb));
    //--------------------------------------------

    return;
}

void MapEditor::on_createNewGroup_clicked()
{
    QString groupName = ui->groupCreation->text();
    if (groupName.isEmpty())
        return;
    try
    {
        mapModel->groupList.at(groupName) = true;
        ui->groupCreation->setText("");
        return;
        //qDebug() << "ex occured";
        return; // если исключение возникает, то оно возникает при попытке доступа к мап айтему, иначе выполнение - доходит до этого момента
    }
    catch (std::out_of_range &ex)
    {
        //qDebug() << "ex accepted";
        mapModel->groupList.insert({ groupName, true });
    }
    ui->groupList->addItem(groupName);
    //mapModel->groupList.push_back(ui->groupCreation->text());
    ui->groupCreation->setText("");
    return;
}

void MapEditor::on_putCurrentRegionIntoGroup_clicked()
{
    if (ui->groupList->currentIndex() == -1)
        return;
    if (currentSelectedItem == -1)
        return;
    mapModel->mapItems.at(currentSelectedItem).setGroupTag(ui->groupList->currentText());
    //qDebug() << ui->groupList->currentText();
    return;
    // сделать так, чтобы всей группе сразу задавался цвет мэйн региона из группы
    // сделать, чтобы сразу выбиралась вся область // область будет выделена после повторного выбора региона (надо переделать, но много переписывать)
}

void MapEditor::on_makeRegionActive_clicked() // awful
{
    if (ui->inactiveRegionsList->currentIndex() == -1)
        return;
    std::list<int>::iterator inactiveList = mapModel->inactiveRegions.begin();
    QString buff = "-1";

    while (inactiveList != mapModel->inactiveRegions.end())
    {
        buff = ui->inactiveRegionsList->currentText();
        buff = buff.mid(buff.indexOf(":") + 2, buff.indexOf(";") - buff.indexOf(":") - 3); // переделать(как и все остальное)

        if (*inactiveList == buff.toInt())
            break;
        inactiveList++;
    }
    ui->inactiveRegionsList->removeItem(ui->inactiveRegionsList->currentIndex());
    ui->inactiveRegionsList->setCurrentIndex(-1);
    mapModel->mapItems.at(buff.toInt()).showItem();
    mapModel->inactiveRegions.erase(inactiveList);
    return;
}



void MapEditor::on_downloadFlagsToTable_triggered()
{
    if (mapModel == nullptr)
    {
        ui->statusbar->showMessage("В данный момент в памяти нет модели карты - сперва обработайте изображение и задайте регионам имена, чтобы загрузить файл таблицы корректно.");
        return;
    }

    QStringList flags = QFileDialog::getOpenFileNames(nullptr, "Выберите файлы флагов для добавления в таблицу",
                            QCoreApplication::applicationDirPath(), "Images (*.png *.jpg)");

    if (flags.empty())
        return;

    auto flagsIter = flags.begin();

    int scalingWidth = 215; // ширина 2 столбца таблицы
    //int scalingHeight = 100; // захотелось
    std::map<QString, QImage> *flagsMap = new std::map<QString, QImage>();
    //double scaling = 1;

    while (flagsIter != flags.end())
    {
//        if (scalingHeight < 5)
//        {
//            delete flagsMap;
//            return;
//        }
        static QString str_buff = "";
        QImage imageBuff;
        str_buff = (*flagsIter).mid((*flagsIter).lastIndexOf("/") + 1, (*flagsIter).lastIndexOf(".") - (*flagsIter).lastIndexOf("/") - 1);
        if (!imageBuff.load(*flagsIter))
            continue; // ошибка
        //scaling =  scalingHeight / imageBuff.height();
        imageBuff = imageBuff.scaledToWidth(scalingWidth);
        flagsMap->insert({ str_buff, imageBuff });
        flagsIter++;
    }

    std::map<int, ext_qgraphicspixmapitem>::iterator mapItemsEditing = mapModel->mapItems.begin();

    while (mapItemsEditing != mapModel->mapItems.end()) // проходимся по всем мап айтемам, которые есть у нас в модели
    {
        static QString regionTitle = "";
        regionTitle = mapItemsEditing->second.table->item(0, 1)->text(); // если в таблице у конкретного региона не задано имя - пропустим его, иначе - заполним
        if (regionTitle == "")
        {
            mapItemsEditing++;
            continue;
        }
        try
        {
            auto img = flagsMap->at(regionTitle);
            mapModel->mapItems.at(mapItemsEditing->second.getID()).insertFlagLabel("Флаг", QPixmap::fromImage(flagsMap->at(regionTitle)));
        }
        catch (std::out_of_range &exception)
        {
            ;
        }
        mapItemsEditing++;
    }
    delete flagsMap;
    return;
}

void MapEditor::on_downloadInfoToTable_triggered()
{
    if (mapModel == nullptr)
    {
        ui->statusbar->showMessage("В данный момент в памяти нет модели карты - сперва обработайте изображение и задайте регионам имена, чтобы загрузить файл таблицы корректно.");
        return;
    }

    QString pathToImage = QFileDialog::getOpenFileName(nullptr, "Choose the .csv file you want to open",
                                                       QCoreApplication::applicationDirPath(), "Файл таблицы (*.csv)");

    if (pathToImage == "")
        return;

    QFile file(pathToImage);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    QString readBuffer;
    readBuffer = in.readLine();
    if (in.atEnd()) // файл пуст
    {
        ui->statusbar->showMessage("Файл не содержит данных");
        return;
    }
    if (readBuffer.mid(0, readBuffer.indexOf(";")) != "Название") // файл имеет неправильную структуру
    {
        ui->statusbar->showMessage("Имеет неправильную структуру. Первый столбец в таблице должен быть: \"Название\"");
        return;
    }

    QStringList captionList = readBuffer.split(";");
    std::map<QString, QString> tableImport;

    while (!in.atEnd())
    {
        readBuffer = in.readLine();
        tableImport.insert({ readBuffer.mid(0, readBuffer.indexOf(";")), readBuffer });
    }

    std::map<int, ext_qgraphicspixmapitem>::iterator mapItemsEditing = mapModel->mapItems.begin();

    while (mapItemsEditing != mapModel->mapItems.end()) // проходимся по всем мап айтемам, которые есть у нас в модели
    {
        static QString regionTitle = "";
        regionTitle = mapItemsEditing->second.table->item(0, 1)->text(); // если в таблице у конкретного региона не задано имя - пропустим его, иначе - заполним
        if (regionTitle == "")
        {
            mapItemsEditing++;
            continue;
        }
        auto cur_iter = tableImport.find(regionTitle); // находим по ключу в мапе, которую мы получили из таблицы в csv файле, нужное имя
        if (cur_iter != tableImport.end()) // если нашли, то заполним таблицу
        {
            ext_qgraphicspixmapitem *item_buf = &mapModel->mapItems.at(mapItemsEditing->first);
            QStringList stringList = cur_iter->second.split(";");
            auto captionIter = captionList.begin() + 1; // первый айтем - название
            auto currentRowIter = stringList.begin() + 1; // первый айтем - название

            while (true)
            {
                if (captionIter == captionList.end() || currentRowIter == stringList.end())
                    break;
                item_buf->addRowToTable(*captionIter, *currentRowIter);

                captionIter++;
                currentRowIter++;
            }
        }
        mapItemsEditing++;
    }
}
