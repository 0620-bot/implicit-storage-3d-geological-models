#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ImplicitStorage.h"
#include "lzbBase.h"
#include "qlibrary.h"

#include "chooseImplicitFunction.h"
#include "setModellingParameters.h"
#include "setSectionParameters.h"

class ImplicitStorage : public QMainWindow
{
    Q_OBJECT

public:
    ImplicitStorage(QWidget *parent = Q_NULLPTR);

public slots:
    void ImplictSaveWindow();
    void ImplictSavePath(QString, QString, QString, QString, QList<Mdl_Point>, double);
    void ImplictSaveModel();

    void ImplictReadWindow();
    void ImplictSetParameters(QString, QString, double, double, double);
    void buildMultiScaleModel();
    void ImplictReadModel();

    void ImplicitCutWindow();
    void ImplicitCutParameters(QString, QList<Mdl_Point>, double, double, QString);
    void GenerateImpSection();


    void ReadImsurfPath(QList<lzbImSurf>&, QString);    //读取隐式面
    void ReadFaultImsurfPath(QList<lzbImSurf>&, QString);    //存储断层隐式面
    void ReadReginImsurfPath(QList<lzbReionImSurf>&, QString);   //读取区域隐式面

    void SaveImsurfPath(QList<lzbImSurf>, QString);    //存储隐式面
    void SaveFaultImsurfPath(QList<lzbImSurf>, QString);    //存储断层隐式面
    void SaveReginImsurfPath(QList<lzbReionImSurf>, QString);   //存储区域隐式面

    //隐式剖切
    void getTruefaceSection(QList<QList<Mdl_Triangle>>& implicitcut);

private:
    Ui::ImplicitStorageClass ui;

    void getcolorcard();
    void initslot();
    void ImSurfSave(QList<lzbImSurf> TopSurflist, QList<lzbImSurf> GoafSurflist, QList<lzbImSurf> FaultSurflist, QList<lzbReionImSurf> RegionSurflist, QString surfPath);
    void ImSurfRead(QList<lzbImSurf>& TopSurflist, QList<lzbImSurf>& GoafSurflist, QList<lzbImSurf>& FaultSurflist, QList<lzbReionImSurf>& RegionSurflist, QString surfPath);

    QList<QList<int>> colorcard;
    double model_depth;
    lzbPLine boundary;
    QList<QList<int>> goaforder;
    QList<int> faultorder;
    int regionnum;
    QList<int> regionorder_instru;
    QList<int> regionorder_stratu;


    QString TopSurfPath;
    QString GoafSurfPath;
    QString FaultSurfPath;
    QString RegionSurfPath;

    //多尺度建模参数
    QString implicitresultPath;
    double minScale, maxScale, Step;
    QString modeloutputPath;

    //剖切参数
    QList<Mdl_Point> cutPList;
    double SectionStep, N;
    QString SectionOutputPath;

    QList<Mdl_Point> demPointList;  //dem点(比下面的后读)
    QList<Mdl_Triangle> topSurfTri; //DEM TIN网

    Mdl_GeoMap submodelresult_direct;   //亚层建模结果


    QList<Mdl_Point> freecutplist;//自由剖切点列表
};
