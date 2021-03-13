#include "compositemap.h"

compositeMap::compositeMap()
{
    return;
}

compositeMap::~compositeMap()
{
    return;
}

bool compositeMap::analyzeImage(QString pathToImage)
{
    QImage worldMap;
    qDebug() << "image load status" << worldMap.load(pathToImage);
    worldMap = worldMap.convertToFormat(QImage::Format_RGB32, Qt::MonoOnly);
    // check what conversion do to colorful image

    int w, h; // size of the image (width and height)
    w = worldMap.width();
    h = worldMap.height();
    this->width = worldMap.width();
    this->height = worldMap.height();

    QColor blackC;
    blackC.setNamedColor("#ff000000");
    QColor whiteC;
    whiteC.setNamedColor("#ffffffff");
    QColor highlightedC;
    highlightedC.setNamedColor("#ff00abff"); // HIGHLIGHT COLOR
//    QColor editC;
//    editC.setNamedColor("#ff00b403"); // green

    QColor tempC;
    //tempC.setNamedColor("#ffff6700"); // orange

    int mapSize = w * h; // кол-во пикселей во всем изображении - будем использовать для примерного отображения прогресса выполнения функции
    int curW = 0, curH = 0; // current X and Y coordinates of pixel on the source image
    int id = 0; // id that we will give to our region on the map to identify it further

    // СДЕЛАТЬ ЗДЕСЬ РЕГИОН ПОД НУЛЕВЫМ ID, КОТОРЫЙ БУДЕТ СОСТОЯТЬ ЦЕЛИКОМ ИЗ ЧЕРНЫХ ПИКСЕЛЕЙ ИСХОДНОГО ИЗОБРАЖЕНИЯ

    // пока не достигли последнего пикселя, проверяем, есть ли белые пиксели на изображении
    // и от каждого белого пикселя заполняем полностью регион, который соединяется с этим пикселем;
    // вся карта в итоге заполнится разными цветами и белых пикселей не останется

    pixelOnMap currPixel(curW, curH); // пиксель с текущими координатами
    QString curColor; // color for the current region
    int regionSize; // размер региона в пикселях

    std::stack <pixelOnMap> currentStepPixels;
    std::stack <pixelOnMap> nextStepPixels; // will become current step pixels on the next step after stack swapping

    pixelOnMap buffPixel; // current pixel
    pixelOnMap newPixel; // one of 4 possible new pixels to check on the next step

    int cleft, cright, ctop, cbottom;
    while (curW < w && curH < h)
    {
        currPixel.setNew(curW, curH);
        if (worldMap.pixelColor(currPixel.widthIndex, currPixel.heightIndex) != whiteC)
        {
            if (curW < (w - 1))
            {
                curW++;
            }
            else
            {
                curW = 0;
                curH++;
            }
            continue;
        }

        // если обнаружили белый пиксель - выделим крайние границы региона
        cleft = curW;
        cright = curW;
        ctop = curH;
        cbottom = curH;
        id++; // на 1 увеличим айди, чтобы после задать его новому региону
        regionSize = 0;

        //--- зарандомим цвет для текущего региона и проверим, есть ли он в использованных(если нет, тогда добавим)
        bool notGenerated = true; // если цвета в наборе использованных ранее не будет - меняем эту переменную на false и выходим из цикла
        while (notGenerated == true)
        {
//            curColor = "#ff";
//            for (int u = 0; u < 3; u++)
//            {
//                curColor += randomHex2S();
//            }
            curColor = generateNiceColor();
            tempC.setNamedColor(curColor);
            try
            {
                usedColors.at(tempC.name(QColor::HexArgb)) = true; // костыль (надо было использовать другую коллекцию, а не мапу)
            }
            catch (std::out_of_range e1)
            {
                usedColors.insert({ tempC.name(QColor::HexArgb), true });
                notGenerated = false;
            }
        }
        //---------------------------------
        currentStepPixels.push(currPixel);
        worldMap.setPixelColor(curW, curH, tempC);

        while (currentStepPixels.size() != 0)
        {
            while (currentStepPixels.size() != 0)
            {
                buffPixel = currentStepPixels.top();
                currentStepPixels.pop();

                // calculate min and max borders for the region
                if (cbottom < buffPixel.heightIndex)
                    cbottom = buffPixel.heightIndex;
                if (ctop > buffPixel.heightIndex)
                    ctop = buffPixel.heightIndex;
                if (cright < buffPixel.widthIndex)
                    cright = buffPixel.widthIndex;
                if (cleft > buffPixel.widthIndex)
                    cleft = buffPixel.widthIndex;
                //---------------------------------------------

                // just rewrite this abomination after testing functionality
                if (buffPixel.widthIndex - 1 >= 0 && worldMap.pixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex) != tempC
                        && worldMap.pixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex) != blackC)
                {
                    newPixel.setNew(buffPixel.widthIndex - 1, buffPixel.heightIndex);
                    nextStepPixels.push(newPixel);
                    worldMap.setPixelColor(buffPixel.widthIndex - 1, buffPixel.heightIndex, tempC);
                }
                if (buffPixel.widthIndex + 1 < w && worldMap.pixelColor(buffPixel.widthIndex + 1, buffPixel.heightIndex) != tempC
                        && worldMap.pixelColor(buffPixel.widthIndex + 1, buffPixel.heightIndex) != blackC)
                {
                    newPixel.setNew(buffPixel.widthIndex + 1, buffPixel.heightIndex);
                    nextStepPixels.push(newPixel);
                    worldMap.setPixelColor(buffPixel.widthIndex + 1, buffPixel.heightIndex, tempC);
                }
                if (buffPixel.heightIndex - 1 >= 0 && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex - 1) != tempC
                        && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex - 1) != blackC)
                {
                    newPixel.setNew(buffPixel.widthIndex, buffPixel.heightIndex - 1);
                    nextStepPixels.push(newPixel);
                    worldMap.setPixelColor(buffPixel.widthIndex, buffPixel.heightIndex - 1, tempC);
                }
                if (buffPixel.heightIndex + 1 < h && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex + 1) != tempC
                        && worldMap.pixelColor(buffPixel.widthIndex, buffPixel.heightIndex + 1) != blackC)
                {
                    newPixel.setNew(buffPixel.widthIndex, buffPixel.heightIndex + 1);
                    nextStepPixels.push(newPixel);
                    worldMap.setPixelColor(buffPixel.widthIndex, buffPixel.heightIndex + 1, tempC);
                }
                regionSize++; // для каждого белого пикселя, который мы перекрасили, увеличиваем общий счетчик пикселей для региона
            }
            currentStepPixels.swap(nextStepPixels); // обнуленный на этом моменте currentStepPixels меняем с заполненным nextStepPixels
        }

        // создание масок для региона
        // QImage QImage::copy(int x, int y, int width, int height) const
        int regw = cright-cleft, regh = cbottom-ctop;
        if (regw == 0)
            regw++;
        if (regh == 0)
            regh++;
//        QImage smallRegionImage = worldMap.copy(cleft, ctop, regw, regh);

//        QImage regionMask = smallRegionImage.createMaskFromColor(tempC.rgb(), Qt::MaskOutColor);
//        QPixmap regionPixmap(QPixmap::fromImage(smallRegionImage));
//        QBitmap regionBitmap(QPixmap::fromImage(regionMask));
//        regionPixmap.setMask(regionBitmap);

//        QPixmap highlightedPixmap(regw, regh);
//        highlightedPixmap.fill(highlightedC);
//        highlightedPixmap.setMask(regionBitmap);

        QImage *smallRegionImage = new QImage(worldMap.copy(cleft, ctop, regw, regh));

        QImage *regionMask = new QImage(smallRegionImage->createMaskFromColor(tempC.rgb(), Qt::MaskOutColor));

        //-----
        QPixmap *regionPixmap = new QPixmap(QPixmap::fromImage(*smallRegionImage));
        QBitmap *regionBitmap = new QBitmap(QPixmap::fromImage(*regionMask));
        //QBitmap regionBitmap(QPixmap::fromImage(*regionMask));
        //-----
        regionPixmap->setMask(*regionBitmap);

        QPixmap *highlightedPixmap = new QPixmap(regw, regh);
        highlightedPixmap->fill(highlightedC);
        highlightedPixmap->setMask(*regionBitmap);

        qDebug () << "l" << cleft << "r" << cright << "t" << ctop << "b" << cbottom;
        qDebug() << "id -" << id;
        //qDebug() << "sz -" << regionPixmap->;

        ext_qgraphicspixmapitem *newItem = new ext_qgraphicspixmapitem();
        newItem->setProps(id, *regionPixmap, *highlightedPixmap, /*editPixmap,*/ tempC, regionSize);

        try
        {
            mapItems.at(id).setX(cleft);
            mapItems.at(id).setY(ctop);
        }
        catch (std::out_of_range e2)
        {
            mapItems.insert({ id, *newItem });
            mapItems.at(id).setX(cleft);
            mapItems.at(id).setY(ctop);
        }

        //qDebug() << "gonna delete now v ";
        delete newItem; // деструктор срабатывает сам
        //qDebug() << "deleted it now ^ ";


        delete regionPixmap;
        delete regionBitmap;
        delete smallRegionImage;
        delete regionMask;
        delete highlightedPixmap;

        //Sleep(2500);

//        if (mapItems.size() > 150) // 146.5 mb for 200 objects
//            break; // test
    }
    return true;
}

QString compositeMap::generateNiceColor()
{
    QString res = "#ff"; // we generate color in ARGB format that has "AA" equal to "ff"
    int irand = 0;
    int offset = 48;
    irand = rand() % (256 - offset) + offset; // fist component of the color should have offset 0+64 to get away from tones that are close to black
    res += randomHex2S(irand);
    irand = rand() % (256 - offset); // second and third components should have offset 255-64 to get away from tones that are close to white
    res += randomHex2S(irand);
    irand = rand() % (256 - offset);
    res += randomHex2S(irand);
    return res;
}

QString compositeMap::randomHex2S(int irand) // generate string containing 2 hex symbols (numbers from 0 to 9 and letters A B C D E F), e.g. "a0" or "cf"
{
    QString res;
    int f, s; // first and second symbols in the string

    if (!(irand >= 0 && irand <= 255)) // dumb protection
        irand = 0;

    f = irand / 16;
    s = irand % 16;

    switch (f)
    {
        case 10:
            res += 'a';
            break;
        case 11:
            res += 'b';
            break;
        case 12:
            res += 'c';
            break;
        case 13:
            res += 'd';
            break;
        case 14:
            res += 'e';
            break;
        case 15:
            res += 'f';
            break;
        default:
            res += QString::number(f);
    }

    switch (s)
    {
        case 10:
            res += 'a';
            break;
        case 11:
            res += 'b';
            break;
        case 12:
            res += 'c';
            break;
        case 13:
            res += 'd';
            break;
        case 14:
            res += 'e';
            break;
        case 15:
            res += 'f';
            break;
        default:
            res += QString::number(s);
    }

    return res;
}
