#ifndef COMPOSITEMAP_H
#define COMPOSITEMAP_H

#include <map>
#include <ext_qgraphicspixmapitem.h>
#include <QTableWidget>
#include <stack>
#include <QBitmap>

#include <windows.h>
#include <QProgressBar>
#include <cheatClass.h>

// класс, в котором будут храниться все созданные объекты в результате анализа изображения - в сумме все
// изображения, маски и прочие элементы будут составлять карту, которая будет отображаться в области просмотра

class compositeMap
{
public:
    compositeMap();
    ~compositeMap();

    cheatClass sgnl_obj; // have function to send signal to notify progress bar about loading
    bool analyzingSuccessful = false;

    int width = 0, height = 0; // source image width and height

    bool saved = false;
    QString pathToFile = "default.imekd";               // make defaul file name depend on date/time to make it unique - set in constructor
    std::map<int, ext_qgraphicspixmapitem> mapItems;    // first key(int) is for ID of the item
    std::map<QString, bool> usedColors;                   // color and id of the region that uses it

    void analyzeImage(QString pathToImage); // file name with image to analyze

    QString randomHex2S(int randOffset);  // generate string containing 2 hex symbols (numbers from 0 to 9 and letters A B C D E F)
    QString generateNiceColor(); // nice means that it will differ a lot from pure white and black
};

class pixelOnMap    // pixel coordinates on image
{
public: pixelOnMap(int wi = 0, int he = 0) { widthIndex = wi; heightIndex = he; }
        int widthIndex;
        int heightIndex;
        void setNew(int wi, int he) { widthIndex = wi; heightIndex = he; }
};

#endif // COMPOSITEMAP_H
