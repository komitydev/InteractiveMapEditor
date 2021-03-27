#include "compositemap.h"

compositeMap::compositeMap()
{
    return;
}

compositeMap::~compositeMap()
{
    return;
}

void compositeMap::analyzeImage(QString pathToImage)
{
    QImage worldMap;
    if (worldMap.load(pathToImage) == false)
    {
        analyzingSuccessful = false;
        return; // failed to load image
    }
    worldMap = worldMap.convertToFormat(QImage::Format_Mono, Qt::ThresholdDither | Qt::MonoOnly); // making black and white image
    worldMap = worldMap.convertToFormat(QImage::Format_RGB32); // setting image to color format again to be able to manipulate pixels

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
    QColor tempC;

    int id = 0; // id that we will give to our region on the map to identify it further

    // in a two-color image black pixels are for the borders - we extract mask from black-colored pixels and set this region as "non-interactable"
    QImage *bordersMask = new QImage(worldMap.createMaskFromColor(blackC.rgb(), Qt::MaskOutColor));
    QPixmap *bordersPixmap = new QPixmap(QPixmap::fromImage(*bordersMask));
    QBitmap *bordersBitmap = new QBitmap(QPixmap::fromImage(*bordersMask));
    bordersPixmap->setMask(*bordersBitmap);

    ext_qgraphicspixmapitem *bordersItem = new ext_qgraphicspixmapitem();
    bordersItem->setProps(id, *bordersPixmap, *bordersPixmap, blackC, 0);

    try
    {
        mapItems.at(id).setX(0);
        mapItems.at(id).setY(0);
    }
    catch (std::out_of_range e2)
    {
        mapItems.insert({ id, *bordersItem });
        mapItems.at(id).setX(0);
        mapItems.at(id).setY(0);
        mapItems.at(id).setAcceptHoverEvents(false);            // for the borders we need to disable hover events,
        mapItems.at(id).setAcceptedMouseButtons(Qt::NoButton);  // all mouse buttons
        mapItems.at(id).unsetCursor();                          // and return cursor to default state
    }

    delete bordersItem;
    delete bordersPixmap;
    delete bordersBitmap;
    delete bordersMask;
    // -----------

    int mapSize = w * h; // кол-во пикселей во всем изображении - будем использовать для примерного отображения прогресса выполнения функции
    int countedPixels = 0;
    int curW = 0, curH = 0; // current X and Y coordinates of pixel on the source image

    pixelOnMap currPixel(curW, curH); // пиксель с текущими координатами
    QString curColor; // color for the current region
    int regionSize; // размер региона в пикселях

    std::stack <pixelOnMap> currentStepPixels;
    std::stack <pixelOnMap> nextStepPixels; // will become current step pixels on the next step after stack swapping

    pixelOnMap buffPixel; // current pixel
    pixelOnMap newPixel; // one of 4 possible new pixels to check on the next step

    int cleft, cright, ctop, cbottom;

    // пока не достигли последнего пикселя, проверяем, есть ли белые пиксели на изображении
    // и от каждого белого пикселя заполняем полностью регион, который соединяется с этим пикселем;
    // вся карта в итоге заполнится разными цветами и белых пикселей не останется

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
        int regw = (cright-cleft) + 1, regh = (cbottom-ctop) + 1;

        QImage *smallRegionImage = new QImage(worldMap.copy(cleft, ctop, regw, regh));
        QImage *regionMask = new QImage(smallRegionImage->createMaskFromColor(tempC.rgb(), Qt::MaskOutColor));
        QPixmap *regionPixmap = new QPixmap(QPixmap::fromImage(*smallRegionImage));
        QBitmap *regionBitmap = new QBitmap(QPixmap::fromImage(*regionMask));
        regionPixmap->setMask(*regionBitmap);
        QPixmap *highlightedPixmap = new QPixmap(regw, regh);
        highlightedPixmap->fill(highlightedC);
        highlightedPixmap->setMask(*regionBitmap);

        ext_qgraphicspixmapitem *newItem = new ext_qgraphicspixmapitem();
        newItem->setProps(id, *regionPixmap, *highlightedPixmap, tempC, regionSize);

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

        delete newItem;
        delete regionPixmap;
        delete regionBitmap;
        delete smallRegionImage;
        delete regionMask;
        delete highlightedPixmap;

        countedPixels += regionSize; // накапливаем количество пикселей, чтобы посчитать примерный(не учитывается кол-во пикселей из первой черной маски) прогресс
        // emit signal with size / mapSize * 100%
        sgnl_obj.loadingPerformed((int) (countedPixels / mapSize * 100));
    }
    //emit signal with 100% and hide progress bar in slot, because you do not need it more
    sgnl_obj.loadingPerformed(100);
    analyzingSuccessful = true;
    return;
}

QString compositeMap::generateNiceColor()
{
    QString res = "#ff"; // we generate color in #AARRGGBB format that has "AA" equal to "ff"
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
