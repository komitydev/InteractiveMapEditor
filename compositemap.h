#ifndef COMPOSITEMAP_H
#define COMPOSITEMAP_H

#include <map>
#include <ext_qgraphicspixmapitem.h>
#include <QTableWidget>
#include <stack>
#include <QBitmap>

#include <windows.h>

// класс, в котором будут храниться все созданные объекты в результате анализа изображения - в сумме все
// изображения, маски и прочие элементы будут составлять карту, которая будет отображаться в области просмотра

//QTableWidget
//QTableWidgetItem - одна ячейка

class compositeMap
{
public:
    compositeMap();
    ~compositeMap();

    int width = 0, height = 0; // source image width and height

    bool saved = false;
    QString pathToFile = "default.imekd";               // make defaul file name depend on date/time to make it unique - set in constructor
    std::map<int, ext_qgraphicspixmapitem> mapItems;    // first key(int) is for ID of the item
    std::map<int, QTableWidget> itemTables;             // here as well
    //std::map<int, int> itemSize;                        // size of the region
    std::map<QString, bool> usedColors;                   // color and id of the region that uses it

    bool analyzeImage(QString pathToImage); // file name with image to analyze
    QString randomHex2S(int randOffset);  // generate string containing 2 hex symbols (numbers from 0 to 9 and letters A B C D E F)
    QString generateNiceColor(); // nice means that it will differ a lot from white and black
};

class pixelOnMap    // pixel coordinates on image
{
public: int widthIndex;     // widthIndex
        int heightIndex;    // heightIndex
        void setNew(int wi, int he) { widthIndex = wi; heightIndex = he; }

public: pixelOnMap(int wi = 0, int he = 0) { widthIndex = wi; heightIndex = he; }
        ~pixelOnMap() { return; }
};

#endif // COMPOSITEMAP_H
