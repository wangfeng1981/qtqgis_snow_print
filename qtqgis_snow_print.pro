
QT += core gui xml widgets webenginewidgets




CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp


#QGIS
INCLUDEPATH += "C:\OSGeo4W\include"
INCLUDEPATH += "C:\OSGeo4W\apps\qgis-ltr\include"
INCLUDEPATH += "C:\Qt\Qt5.14.2\5.14.2\msvc2017\include"
INCLUDEPATH += "C:\Qt\Qt5.14.2\5.14.2\msvc2017\include\QtWidgets"
INCLUDEPATH += "C:\Qt\Qt5.14.2\5.14.2\msvc2017\include\QtPrintSupport"
INCLUDEPATH += "C:\Qt\Qt5.14.2\5.14.2\msvc2017\include\QtSvg"
INCLUDEPATH += "C:\Qt\Qt5.14.2\5.14.2\msvc2017\include\QtWebEngineWidgets"

LIBS += -L"C:\Qt\Qt5.14.2\5.14.2\msvc2017\lib"
LIBS += -L"C:\OSGeo4W\apps\Qt5\lib"
LIBS += -L"C:\OSGeo4W\apps\qgis-ltr\lib" -lqgis_core -lqgis_gui -lqgis_app


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
