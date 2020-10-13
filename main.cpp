
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include <QCoreApplication>
#include <QDir>

#include "qgsapplication.h"

#include <iostream>
#include <fstream>
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
#include "qgsfeature.h"
#include "qgsfeaturerequest.h"
#include "qgsfeaturesource.h"
#include "qgis_core.h"
#include "qgsrenderer.h"
#include "qgssymbollayer.h"
#include "qgssinglesymbolrenderer.h"
#include "qgsfillsymbollayer.h"

#include "../sharedcodes2/ajson5.h"

using namespace std;
using namespace ArduinoJson;

int main(int argc, char *argv[])
{
    qDebug()<<"A program to print map." ;
    qDebug()<<"usage:qtqgis_snow_print config.json input.tif out.png" ;
    qDebug()<<"v1.0" ;//2020-10-14

//    if( argc!=4 )
//    {
//        qDebug()<<"no enough params (4)." ;
//        return 11 ;
//    }

//    string configfile = argv[1];
//    string inputfile = argv[2];
//    string outfile = argv[3] ;

    string configfile = "D:/coding/qtqgis_snow_print/print.json";
    string inputfile = "F:/test-snow.tif";
    string outfile = "D:/test-snow-print.png" ;

    cout<<"configfile:"<<configfile<<endl;
    cout<<"inputfile:"<<inputfile<<endl;
    cout<<"outfile:"<<outfile<<endl;

    string landshpfile = "";
    string legendfile = "";
    string projdir = "" ;

    {
        ifstream ifs(configfile.c_str()) ;
        DynamicJsonBuffer dynbuffer;
        JsonObject& jsonroot = dynbuffer.parse(ifs) ;
        landshpfile = jsonroot["landshp"].as<char*>();
        legendfile = jsonroot["legend"].as<char*>();
        projdir = jsonroot["projdir"].as<char*>();
    }
    cout<<"landshpfile:"<<landshpfile<<endl;
    cout<<"legendfile:"<<legendfile<<endl;
    cout<<"projdir:"<<projdir<<endl ;


    //QCoreApplication a(argc, argv);
    QDir currdir = QDir::currentPath() ;
    qDebug()<< currdir.path() ;
    QgsApplication::setPrefixPath( currdir.path() );
    // "PROJ_LIB: C:\Program Files\GDAL\projlib"
    qputenv("PROJ_LIB",projdir.c_str());

    QgsApplication a(argc,argv,false);
    QgsApplication::initQgis();

    cout<<"v1.0 qtqgis_snow_print"<<endl ;

    QgsProject* project= QgsProject::instance() ;

    QgsRasterLayer* rasterLayer = new
        QgsRasterLayer(
            QString(inputfile.c_str()),
            "raster",
            "gdal");

    QgsVectorLayer* landlayer = new QgsVectorLayer( QString(landshpfile.c_str()) , "land", "ogr");

    if(rasterLayer->isValid() && landlayer->isValid() )
    {
        cout<<"shp layer is good."<<endl;

        //make line color
        QgsFeatureRenderer* featureRender = landlayer->renderer();
        QgsRenderContext qrcontext;
        QgsSymbolList symlist = featureRender->symbols( qrcontext );
        QgsSymbol* symbol0 = symlist[0];
        QgsSymbolLayer* symlayer = symbol0->symbolLayer(0);
        QgsSimpleFillSymbolLayer* symlayer1 = (QgsSimpleFillSymbolLayer *)symlayer;
        symlayer1->setStrokeWidth(0.1);
        symlayer->setStrokeColor( QColor(128,128,0)) ;
        symlayer->setFillColor( QColor(0,0,0,0)) ;


        cout<<"raster layer is good."<<endl ;
        project->addMapLayer(rasterLayer,true) ;
        project->addMapLayer(landlayer,true);

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
        picture->setPicturePath( QString(legendfile.c_str()) );
        layout.addLayoutItem(picture);
        picture->attemptMove( QgsLayoutPoint(20,124) );


        QgsLayoutExporter exporter(&layout);
        QgsLayoutExporter::ImageExportSettings esetting = QgsLayoutExporter::ImageExportSettings() ;
        esetting.cropToContents =true ;
        exporter.exportToImage( QString(outfile.c_str()) , esetting);

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
