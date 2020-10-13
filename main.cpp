
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include <QCoreApplication>
#include <QDir>

#include "qgsapplication.h"

#include <iostream>
#include "qgsproject.h"
#include "qgslayout.h"
#include "qgsprintlayout.h"
#include "qgsrasterlayer.h"
#include "qgsvectorlayer.h"
#include "qgslayoutitemmap.h"
#include "qgslayoutexporter.h"
#include "qgssinglebandpseudocolorrenderer.h"
#include "qgsrastershader.h"
#include "qgslayoutitemmapgrid.h"
#include "qgslayoutitempicture.h"
#include "qgslayoutitem.h"

using namespace std;


int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QDir currdir = QDir::currentPath() ;
    qDebug()<< currdir.path() ;
    QgsApplication::setPrefixPath( currdir.path() );
    // "PROJ_LIB: C:\Program Files\GDAL\projlib"
    qputenv("PROJ_LIB","D:\\coding\\build-qtqgis_snow_print-unknown-Release\\release\\share\\proj");

    QgsApplication a(argc,argv,false);
    QgsApplication::initQgis();

    cout<<"v1.0 qtqgis_snow_print"<<endl ;

    QgsProject* project= QgsProject::instance() ;

    QgsRasterLayer* rasterLayer = new
        QgsRasterLayer(
            "F:\\test-snow.tif",
            "raster",
            "gdal");
    if(rasterLayer->isValid())
    {

        cout<<"raster layer is good."<<endl ;
        project->addMapLayer(rasterLayer,true) ;

        //set renderer
        QList<QgsColorRampShader::ColorRampItem> crlist ;
        crlist.append( QgsColorRampShader::ColorRampItem(0,QColor(0,0,0,255), QString("Fill"))) ;
        crlist.append( QgsColorRampShader::ColorRampItem(1,QColor(128,128,128,255), QString("Cloud"))) ;
        crlist.append( QgsColorRampShader::ColorRampItem(2,QColor(20,0,177,255), QString("Sea"))) ;
        crlist.append( QgsColorRampShader::ColorRampItem(3,QColor(0, 176, 82,255), QString("Land"))) ;
        crlist.append( QgsColorRampShader::ColorRampItem(4,QColor(255,255,255,255), QString("Snow"))) ;

        QgsColorRampShader* crshader= new QgsColorRampShader(0,255,nullptr, QgsColorRampShader::Type::Exact ) ;
        crshader->setColorRampItemList(crlist) ;

        QgsRasterShader *rasterShader = new QgsRasterShader();
        rasterShader->setRasterShaderFunction(crshader) ;
        QgsSingleBandPseudoColorRenderer* pseudRender = new QgsSingleBandPseudoColorRenderer(
                        rasterLayer->dataProvider() , 1 , rasterShader
                    ) ;
        rasterLayer->setRenderer(pseudRender);

        QgsRectangle rect1 = rasterLayer->extent();
        cout<<rect1.xMinimum()<<","<<rect1.xMaximum()<<endl ;
        cout<<rect1.yMinimum()<<","<<rect1.yMaximum()<<endl ;

        cout<<"add layer ok."<<endl ;

        QgsLayout layout( project ) ;
        layout.initializeDefaults();


        QgsLayoutItemMap* mapitem = QgsLayoutItemMap::create(&layout) ;
        mapitem->setRect( 0.0,0.0,40.0,40.0);
        mapitem->zoomToExtent(rasterLayer->extent());
        mapitem->setFrameEnabled(true);
        mapitem->setCrs(QgsCoordinateReferenceSystem("EPSG:4326"));//must set crs, if not grid display a mess.
        layout.addLayoutItem(mapitem) ;

        //grid
        QgsLayoutItemMapGrid* grid0 = new QgsLayoutItemMapGrid("grid0"  , mapitem) ;
        grid0->setEnabled(true);
        grid0->setCrs( QgsCoordinateReferenceSystem("EPSG:4326"));
        grid0->setAnnotationFont(QFont("Helvetica",8));
        grid0->setIntervalX(20.0);
        grid0->setIntervalY(20.0);
        QgsStringMap dashlinemap ;
        //<prop v="133,133,133,255" k="line_color"/>
        // <prop v="dash" k="line_style"/>
        dashlinemap["line_color"] = "133,133,133,255";
        dashlinemap["line_style"] = "dash" ;
        dashlinemap["line_width"] = "0.1" ;
        QgsLineSymbol* dashline = QgsLineSymbol::createSimple(dashlinemap);

        grid0->setLineSymbol(dashline) ;
        grid0->setAnnotationEnabled(true);
        grid0->setAnnotationFormat( QgsLayoutItemMapGrid::DecimalWithSuffix);
        grid0->setAnnotationPrecision(0);
        mapitem->grids()->addGrid(grid0);

        mapitem->attemptMove( QgsLayoutPoint(20,20) ) ;//left top margin 5mm x 5mm
        mapitem->attemptResize( QgsLayoutSize(100,100) ) ;//10cmx10cm full page is A4 21cmx29.7cm

        //legend
        QgsLayoutItemPicture* picture = new QgsLayoutItemPicture(&layout) ;
        picture->setRect(0.0,0.0,80.0,10.0) ;
        picture->setPicturePath( QStringLiteral( "D:/coding/qtqgis_snow_print/legend.png" ) );
        layout.addLayoutItem(picture);
        picture->attemptMove( QgsLayoutPoint(20,124) );


        QgsLayoutExporter exporter(&layout);
        QgsLayoutExporter::ImageExportSettings esetting = QgsLayoutExporter::ImageExportSettings() ;
        esetting.cropToContents =true ;
        exporter.exportToImage("D:/testqtqgis.png" , esetting);

        cout<<"export done."<<endl ;

//        layoutImagePath = os.path.join(QgsProject.instance().homePath(), "layoutimage.png")
//        exporter = QgsLayoutExporter(layout)
//        exporter.exportToImage(layoutImagePath, QgsLayoutExporter.ImageExportSettings())

    }else
    {
        cout<<"raster layer is bad."<<endl ;
    }
    cout<<"quiting..."<<endl;
    a.quit();
    cout<<"out"<<endl;

    return 0;
}
