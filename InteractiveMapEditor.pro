QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    compositemap.cpp \
    ext_qgraphicspixmapitem.cpp \
    ext_qgraphicsview.cpp \
    item_chosen.cpp \
    main.cpp \
    mapeditor.cpp

HEADERS += \
    compositemap.h \
    ext_qgraphicspixmapitem.h \
    ext_qgraphicsview.h \
    item_chosen.h \
    mapeditor.h

FORMS += \
    mapeditor.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    mapeditor_resources.qrc
