#include "ImplicitStorage.h"

ImplicitStorage::ImplicitStorage(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    getcolorcard();
    initslot();
}
//获取色卡
void ImplicitStorage::getcolorcard()
{
    colorcard.clear();

    QString colorfile = "data/DrillColor.txt";
    QFile afile(colorfile);
    if (!afile.exists())//文件不存在
    {
        qDebug() << "file not exists";
        ui.statusBar->showMessage("file not exists");
        return;
    }
    if (!afile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "file open faile";
        ui.statusBar->showMessage("file open faile");
        return;
    }
    QTextStream astream(&afile);//用文本流读文件
    astream.setAutoDetectUnicode(true);//自动检测Unicode

    //读取色卡并储存
    QString str;
    while (!astream.atEnd())
    {
        str = astream.readLine();
        QStringList list = str.split(",");
        QList<int> color_i;
        int num = list[0].toInt(); int r = list[1].toInt(), g = list[2].toInt(), b = list[3].toInt();
        color_i.append(num); color_i.append(r); color_i.append(g); color_i.append(b);
        colorcard.append(color_i);
    }
}
//信号连接
void ImplicitStorage::initslot()
{
    connect(ui.chooseimplicitfunctions, SIGNAL(clicked(bool)), this, SLOT(ImplictSaveWindow()));
    connect(ui.StoragepushButton, SIGNAL(clicked(bool)), this, SLOT(ImplictSaveModel()));

    connect(ui.setmodellingparameters, SIGNAL(clicked(bool)), this, SLOT(ImplictReadWindow()));
    connect(ui.modelling, SIGNAL(clicked(bool)), this, SLOT(buildMultiScaleModel()));

    connect(ui.setsectiongparameters, SIGNAL(clicked(bool)), this, SLOT(ImplicitCutWindow()));
    connect(ui.generatesection, SIGNAL(clicked(bool)), this, SLOT(GenerateImpSection()));
}

void ImplicitStorage::ImplictSaveWindow()
{
    chooseImplicitFunction* WidgetChooseFunction = new chooseImplicitFunction();
    connect(WidgetChooseFunction, SIGNAL(completeChoose(QString, QString, QString, QString, QList<Mdl_Point>, double)), this, SLOT(ImplictSavePath(QString, QString, QString, QString, QList<Mdl_Point>, double)));

    WidgetChooseFunction->show();
}
void ImplicitStorage::ImplictSavePath(QString Top, QString Goaf, QString Fault, QString Region, QList<Mdl_Point> BPP, double modelD)
{
    TopSurfPath = Top;
    GoafSurfPath = Goaf;
    FaultSurfPath = Fault;
    RegionSurfPath = Region;

    boundary = lzbPLine(BPP);
    model_depth = modelD;
    ui.statusBar->showMessage("Complete the selection of implicit funtions.");
}
void ImplicitStorage::ImplictSaveModel()
{
    QList<lzbImSurf> TopSurflist;
    QList<lzbImSurf> GoafSurflist;
    QList<lzbImSurf> FaultSurflist;
    QList<lzbReionImSurf> RegionSurflist;

    ReadImsurfPath(TopSurflist, TopSurfPath);
    ReadImsurfPath(GoafSurflist, GoafSurfPath);
    ReadFaultImsurfPath(FaultSurflist, FaultSurfPath);
    ReadReginImsurfPath(RegionSurflist, RegionSurfPath);

    QString surfPath = QString("%0/ModelSurf.txt").arg(QCoreApplication::applicationDirPath());
    ImSurfSave(TopSurflist, GoafSurflist, FaultSurflist, RegionSurflist, surfPath);
    ui.statusBar->showMessage("Complete implicit storage of the model.");
}


void ImplicitStorage::ImplictReadWindow()
{
    setModellingParameters* WidgetsetParameters = new setModellingParameters();
    connect(WidgetsetParameters, SIGNAL(completeSet(QString, QString, double, double, double)), this, SLOT(ImplictSetParameters(QString, QString, double, double, double)));

    WidgetsetParameters->show();
}
void ImplicitStorage::ImplictSetParameters(QString implicitresult, QString outputpath, double minscale, double maxscale, double step)
{
    implicitresultPath = implicitresult;
    modeloutputPath = outputpath;
    minScale = minscale;
    maxScale = maxscale;
    Step = step;

    ui.statusBar->showMessage("Complete set modelling parameters.");
}
void ImplicitStorage::buildMultiScaleModel()
{
    //读取隐式存储结果并拆分存储隐式面
    ui.statusBar->showMessage("Read implicit save model result...");
    ImplictReadModel();

    //调用动态库，实现各尺度自动建模及保存
    int modelnum = (maxScale - minScale) / Step;
    for (int i = 0; i <= modelnum; i++)
    {
        QList<QList<Mdl_Triangle>> complexLayerModelResult;
        double scale = minScale + i * Step;
        QList<Mdl_Point> TopSurfPoint;
        //计时器
        QTime time;
        time.start();
        //隐式面文件()
        QString quaterPath;
        typedef void(*dllMainLayerModelling)(QList <Mdl_Point> _demImpPList, Mdl_Poly BoundaryPolygon, double accuracy,
            QString TopSurfPath, QString QuaternaryPath, QString GoafSurfPath, QString FaultSurfPath, QString RegionSurfPath, float modelElevation, QList<QList<Mdl_Triangle>>& trist, bool isSolid);
        QLibrary toptriDLL(QString("%0/MarchTetrahedraDLL.dll").arg(QCoreApplication::applicationDirPath()));
        if (toptriDLL.load())
        {
            dllMainLayerModelling tp = (dllMainLayerModelling)toptriDLL.resolve("lzbdllFastVisiable_KnowSurf");//导出函数
            tp(TopSurfPoint, boundary.toMdl_Poly(), scale, TopSurfPath, quaterPath, GoafSurfPath, FaultSurfPath, RegionSurfPath, model_depth, complexLayerModelResult, false);
        }
        else
        {
            ui.statusBar->showMessage(QString("toptriDLL is not load!"));
            return;
        }
        toptriDLL.unload();
        ui.statusBar->showMessage(QString("Complete scale-%0m model, time taken %1s. (%2 models left)").arg(scale).arg(time.elapsed() / 1000.0).arg(modelnum - i));

        //导出模型结果
        QList<QList<int>> colorlist;
        for (int i = 0; i < complexLayerModelResult.size(); i++)
        {
            QList<int> color;
            if (complexLayerModelResult.size() - i <= 8)
            {
                color.append(0); color.append(190); color.append(255);
            }
            else if (complexLayerModelResult.size() - i == 9)
            {
                color.append(40); color.append(40); color.append(40);
            }
            else
                color.append(colorcard[i][1]); color.append(colorcard[i][2]); color.append(colorcard[i][3]);
            colorlist.append(color);
        }
        //导出
        QString outputpath = modeloutputPath + QString("/Scale-%0.obj").arg(scale);
        typedef void(*outobj)(QString path, QList<QList<Mdl_Triangle>> model, QList<QList<int>> colorlist, bool iss);
        QLibrary OutputDLL("OutOBJDLL.dll");
        outobj ob;
        if (OutputDLL.load())
            ob = (outobj)OutputDLL.resolve("exportOBJwithColor");//导出函数
        else
            return;
        ob(outputpath, complexLayerModelResult, colorlist, true);
    }
    ui.statusBar->showMessage(QString("Complete multiscale modelling..."));
}
void ImplicitStorage::ImplictReadModel()
{
    QList<lzbImSurf> TopSurflist;
    QList<lzbImSurf> GoafSurflist;
    QList<lzbImSurf> FaultSurflist;
    QList<lzbReionImSurf> RegionSurflist;
    //读取各隐式面
    ImSurfRead(TopSurflist, GoafSurflist, FaultSurflist, RegionSurflist, implicitresultPath);

    //分开存储隐式面并保存路径
    TopSurfPath = QString("%0/ImplicitSurfdata/TopSurf.txt").arg(QCoreApplication::applicationDirPath());
    GoafSurfPath = QString("%0/ImplicitSurfdata/GoafSurf.txt").arg(QCoreApplication::applicationDirPath());
    FaultSurfPath = QString("%0/ImplicitSurfdata/FaultSurf.txt").arg(QCoreApplication::applicationDirPath());
    RegionSurfPath = QString("%0/ImplicitSurfdata/RegionSurf.txt").arg(QCoreApplication::applicationDirPath());
    SaveImsurfPath(TopSurflist, TopSurfPath);
    SaveImsurfPath(GoafSurflist, GoafSurfPath);
    SaveFaultImsurfPath(FaultSurflist, FaultSurfPath);
    SaveReginImsurfPath(RegionSurflist, RegionSurfPath);
}

void ImplicitStorage::ImplicitCutWindow()
{
    setSectionParameters* WidgetsetParameters = new setSectionParameters();
    connect(WidgetsetParameters, SIGNAL(completeSetSection(QString, QList<Mdl_Point>, double, double, QString)), this, SLOT(ImplicitCutParameters(QString, QList<Mdl_Point>, double, double, QString)));

    WidgetsetParameters->show();
}
void ImplicitStorage::ImplicitCutParameters(QString implicitresult, QList<Mdl_Point> cutPoine, double step, double n, QString path)
{
    implicitresultPath = implicitresult;
    cutPList = cutPoine;
    SectionStep = step;
    N = n;
    SectionOutputPath = path;
}
void ImplicitStorage::GenerateImpSection()
{
    //读取隐式存储结果并拆分存储隐式面
    ui.statusBar->showMessage("Read implicit save model result...");
    ImplictReadModel();

    lzbSectionmdl onesec(cutPList);
    //计时器
    QTime time;
    time.start();

    QList<Mdl_Face> truefacecut;
    QList<Mdl_Poly> facecutline;
    QString quaterPath;
    //需要计算的与标记面的交线
    QStringList BJMianpathList;
    QString BJMianpath1 = QString("%0/data/70JBJmian.txt").arg(QCoreApplication::applicationDirPath());
    QFile mfiledtm(BJMianpath1);
    mfiledtm.open(QIODevice::ReadOnly | QIODevice::Text);
    if (mfiledtm.size() != 0)       //有标记面才加上
        BJMianpathList.append(BJMianpath1);
    typedef void(*dllMainLayerModelling)(QList<Mdl_Point> plist, double jingdu, QString topsurfpath, QString QuaternaryPath,
        QString GoafSurfPath, QString FaultSurfPath, QString RegionSurfPath, float modelElevation, QStringList BJImSurfList, QList<Mdl_Face>& trist, QList<Mdl_Poly>& bjline);
    QLibrary toptriDLL("ImplictCutDLL.dll");
    if (toptriDLL.load())
    {
        dllMainLayerModelling tp = (dllMainLayerModelling)toptriDLL.resolve("lzbdllMultidataGetTrueCut_BJLine_KnowSurf");//导出函数
        tp(onesec.plist, SectionStep, TopSurfPath, quaterPath, GoafSurfPath, FaultSurfPath, RegionSurfPath, model_depth, BJMianpathList, truefacecut, facecutline);
    }
    //处理标记线
    QList<lzbPLine> plineresult;
    for (int i = 0; i < facecutline.size(); i++)
    {
        lzbPLine midplie;
        midplie.Name = BJMianpathList[i].split("/").last().split(".").first();
        midplie.color.append(0); midplie.color.append(0); midplie.color.append(200);
        for (int j = 0; j < facecutline[i].pList.size(); j++)
            midplie.PList.append(facecutline[i].pList[j]);
        plineresult.append(midplie);
    }
    onesec.cutFace = truefacecut;
    onesec.PLineList = plineresult;
    ui.statusBar->showMessage(QString("Complete scale-%0m section model, time taken %1s.").arg(SectionStep).arg(time.elapsed() / 1000.0));

    //导出剖面
    QList<QList<Mdl_Triangle>> cutTrilist;
    for (int i = 0; i < onesec.cutFace.size(); i++)
    {
        QList<Mdl_Triangle> mid;
        for (int j = 0; j < onesec.cutFace[i].triList.size(); j++)
        {
            Mdl_Triangle tt = onesec.cutFace[i].triList[j];
            mid.append(tt);
        }
        cutTrilist.append(mid);
    }
    QList<QList<int>> colorlist;
    for (int i = 0; i < cutTrilist.size(); i++)
    {
        QList<int> color;
        if (cutTrilist.size() - i <= 8)
        {
            color.append(0); color.append(190); color.append(255);
        }
        else if (cutTrilist.size() - i == 9)
        {
            color.append(40); color.append(40); color.append(40);
        }
        else
            color.append(colorcard[i][1]); color.append(colorcard[i][2]); color.append(colorcard[i][3]);
        colorlist.append(color);
    }
    //导出
    QString outputpath = SectionOutputPath + QString("/ImplicitSection.obj");
    typedef void(*outobj)(QString path, QList<QList<Mdl_Triangle>> model, QList<QList<int>> colorlist, bool issingle);
    QLibrary OutputDLL("OutOBJDLL.dll");
    outobj ob;
    if (OutputDLL.load())
        ob = (outobj)OutputDLL.resolve("exportOBJwithColor");//导出函数
    else
        return;
    ob(outputpath, cutTrilist, colorlist, false);
    ui.statusBar->showMessage(QString("Complete section model.(path:%0)").arg(outputpath));
}




void ImplicitStorage::ReadImsurfPath(QList<lzbImSurf>& GoafSurflist, QString filepath)
{   
    //读取隐式面文件
    QFile mfiledtm(filepath);
    mfiledtm.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream mymin(&mfiledtm);

    QList <lzbImPoint> midImPList;
    QList <double> d;

    int num = 1;

    while (!mymin.atEnd())
    {
        QString inStr = mymin.readLine();
        if (inStr == "Surf")
        {
            midImPList.clear();
            d.clear();
            continue;
        }
        if (inStr == "ENDSurf")
        {
            //if (midImPList.isEmpty())continue;	//2100962
            GoafSurflist.append(lzbImSurf(num, midImPList, d));
            num++;
            continue;
        }
        QStringList strList = inStr.split(",");

        if (strList.size() == 6)
        {
            midImPList.append(lzbImPoint(strList[0].toDouble(), strList[1].toDouble(), strList[2].toDouble(), strList[3].toDouble(), strList[4].toDouble(), strList[5].toDouble()));
        }
        if (strList.size() == 1)
        {
            if (inStr.split(".").size() != 1)
            {
                d.append(inStr.toDouble());
            }
        }
    }
}
void ImplicitStorage::ReadFaultImsurfPath(QList<lzbImSurf>& faultSurflist, QString filepath)
{
    //读取隐式面文件
    QFile mfiledtm(filepath);
    mfiledtm.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream mymin(&mfiledtm);

    QString faultnum;
    QList <lzbImPoint> midImPList;
    QList <double> d;

    int flag = 0;
    lzbImPoint myPoint;

    while (!mymin.atEnd())
    {
        QString inStr = mymin.readLine();
        if (inStr.contains("Surf") && !inStr.contains("END") && !inStr.contains("List"))
        {
            faultnum = inStr.split("_").last();
            midImPList.clear();
            d.clear();
            continue;
        }
        if (inStr == "ENDSurf")
        {
            //if (midImPList.isEmpty())continue;	//2100962
            faultSurflist.append(lzbImSurf(faultnum, midImPList, d));
            continue;
        }
        QStringList strList = inStr.split(",");

        if (strList.size() == 6)
        {
            midImPList.append(lzbImPoint(strList[0].toDouble(), strList[1].toDouble(), strList[2].toDouble(), strList[3].toDouble(), strList[4].toDouble(), strList[5].toDouble()));
        }
        if (strList.size() == 1)
        {
            if (inStr.split(".").size() != 1)
            {
                d.append(inStr.toDouble());
            }
        }
    }
}
void ImplicitStorage::ReadReginImsurfPath(QList<lzbReionImSurf>& regionimsurflist, QString filepath)
{
    //读取区域隐式面文件
    QFile mfiledtm(filepath);
    mfiledtm.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream mymin(&mfiledtm);

    lzbReionImSurf regiomsurf;
    QList<lzbImPoint> midImPList;
    QList<double> d;

    int regionnum = 1;
    int InstruSrufnum = 1;
    int StratuSrufnum = 1;

    while (!mymin.atEnd())
    {
        QString inStr = mymin.readLine();
        if (inStr == "REGION")
        {
            regiomsurf.InstruSruflist.clear();
            regiomsurf.StratuSruflist.clear();
            continue;
        }
        if (inStr == "instruHRBF")
        {
            midImPList.clear();
            d.clear();
            continue;
        }
        if (inStr == "HRBFSurf")
        {
            midImPList.clear();
            d.clear();
            continue;
        }
        if (inStr == "ENDREGION")
        {
            regiomsurf.num = regionnum;
            regionimsurflist.append(regiomsurf);
            regionnum++;
            InstruSrufnum = 1;
            StratuSrufnum = 1;
            continue;
        }
        if (inStr == "ENDinstruHRBF")
        {
            //if (midImPList.isEmpty())continue;	//2100962
            regiomsurf.InstruSruflist.append(lzbImSurf(InstruSrufnum, midImPList, d));
            InstruSrufnum++;
            continue;
        }
        if (inStr == "ENDHRBFSurf")
        {
            //if (midImPList.isEmpty())continue;	//2100962
            regiomsurf.StratuSruflist.append(lzbImSurf(StratuSrufnum, midImPList, d));
            StratuSrufnum++;
            continue;
        }
        QStringList strList = inStr.split(",");

        if (strList.size() == 6)
        {
            midImPList.append(lzbImPoint(strList[0].toDouble(), strList[1].toDouble(), strList[2].toDouble(), strList[3].toDouble(), strList[4].toDouble(), strList[5].toDouble()));
        }
        if (strList.size() == 1)
        {
            if (inStr.split(".").size() != 1)
            {
                d.append(inStr.toDouble());
            }
        }
    }
}

void ImplicitStorage::SaveImsurfPath(QList<lzbImSurf> surf, QString savepath)
{
    ////结果输出
    QFile outfile(savepath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream moutFile(&outfile);
    QString strResultTri;
    //导出采空区隐式面
    strResultTri = QObject::tr("SurfList");
    moutFile << strResultTri << "\n";
    for (int i = 0; i < surf.size(); i++)
    {
        strResultTri = QObject::tr("Surf");
        moutFile << strResultTri << "\n";
        for (int j = 0; j < surf[i].IPList.size(); j++)
        {
            QString midStr[6];
            midStr[0] = QString("%0").arg(surf[i].IPList[j].x, 0, 'f', 5);
            midStr[1] = QString("%0").arg(surf[i].IPList[j].y, 0, 'f', 5);
            midStr[2] = QString("%0").arg(surf[i].IPList[j].z, 0, 'f', 5);
            midStr[3] = QString("%0").arg(surf[i].IPList[j].dx, 0, 'f', 5);
            midStr[4] = QString("%0").arg(surf[i].IPList[j].dy, 0, 'f', 5);
            midStr[5] = QString("%0").arg(surf[i].IPList[j].dz, 0, 'f', 5);
            strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
            moutFile << strResultTri << "\n";
        }
        for (int j = 0; j < surf[i].d.size(); j++)
        {
            strResultTri = QString("%0").arg(surf[i].d[j], 0, 'f', 30);
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDSurf");
        moutFile << strResultTri << "\n";
    }
    strResultTri = QObject::tr("ENDSurfList");
    moutFile << strResultTri << "\n";
    outfile.close();
}
void ImplicitStorage::SaveFaultImsurfPath(QList<lzbImSurf> surf, QString savepath)
{
    ////结果输出
    QFile outfile(savepath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream moutFile(&outfile);
    QString strResultTri;
    //导出断层隐式面
    strResultTri = QObject::tr("SurfList");
    moutFile << strResultTri << "\n";
    for (int i = 0; i < surf.size(); i++)
    {
        strResultTri = QObject::tr("Surf_%0").arg(surf[i].faultnum);
        moutFile << strResultTri << "\n";
        for (int j = 0; j < surf[i].IPList.size(); j++)
        {
            QString midStr[6];
            midStr[0] = QString("%0").arg(surf[i].IPList[j].x, 0, 'f', 5);
            midStr[1] = QString("%0").arg(surf[i].IPList[j].y, 0, 'f', 5);
            midStr[2] = QString("%0").arg(surf[i].IPList[j].z, 0, 'f', 5);
            midStr[3] = QString("%0").arg(surf[i].IPList[j].dx, 0, 'f', 5);
            midStr[4] = QString("%0").arg(surf[i].IPList[j].dy, 0, 'f', 5);
            midStr[5] = QString("%0").arg(surf[i].IPList[j].dz, 0, 'f', 5);
            strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
            moutFile << strResultTri << "\n";
        }
        for (int j = 0; j < surf[i].d.size(); j++)
        {
            strResultTri = QString("%0").arg(surf[i].d[j], 0, 'f', 30);
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDSurf");
        moutFile << strResultTri << "\n";
    }
    strResultTri = QObject::tr("ENDSurfList");
    moutFile << strResultTri << "\n";
    outfile.close();
}
void ImplicitStorage::SaveReginImsurfPath(QList<lzbReionImSurf> regionsurf, QString savepath)
{
    ////结果输出
    QFile outfile(savepath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream moutFile(&outfile);
    QString strResultTri;

    for (int k = 0; k < regionsurf.size(); k++)
    {
        strResultTri = QObject::tr("REGION");
        moutFile << strResultTri << "\n";

        QList<lzbImSurf> midinstrusurf = regionsurf[k].InstruSruflist;
        QList<lzbImSurf> midyacengsurf = regionsurf[k].StratuSruflist;
        //导出侵入岩隐式面
        strResultTri = QObject::tr("instruHRBFList");
        moutFile << strResultTri << "\n";
        for (int i = 0; i < midinstrusurf.size(); i++)
        {
            strResultTri = QObject::tr("instruHRBF");
            moutFile << strResultTri << "\n";
            for (int j = 0; j < midinstrusurf[i].IPList.size(); j++)
            {
                QString midStr[6];
                midStr[0] = QString("%0").arg(midinstrusurf[i].IPList[j].x, 0, 'f', 5);
                midStr[1] = QString("%0").arg(midinstrusurf[i].IPList[j].y, 0, 'f', 5);
                midStr[2] = QString("%0").arg(midinstrusurf[i].IPList[j].z, 0, 'f', 5);
                midStr[3] = QString("%0").arg(midinstrusurf[i].IPList[j].dx, 0, 'f', 5);
                midStr[4] = QString("%0").arg(midinstrusurf[i].IPList[j].dy, 0, 'f', 5);
                midStr[5] = QString("%0").arg(midinstrusurf[i].IPList[j].dz, 0, 'f', 5);
                strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
                moutFile << strResultTri << "\n";
            }
            for (int j = 0; j < midinstrusurf[i].d.size(); j++)
            {
                strResultTri = QString("%0").arg(midinstrusurf[i].d[j], 0, 'f', 30);
                moutFile << strResultTri << "\n";
            }
            strResultTri = QObject::tr("ENDinstruHRBF");
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDinstruHRBFList");
        moutFile << strResultTri << "\n";
        //导出各亚层隐式面
        strResultTri = QObject::tr("HRBFSurfList");
        moutFile << strResultTri << "\n";
        for (int i = 0; i < midyacengsurf.size(); i++)
        {
            strResultTri = QObject::tr("HRBFSurf");
            moutFile << strResultTri << "\n";
            for (int j = 0; j < midyacengsurf[i].IPList.size(); j++)
            {
                QString midStr[6];
                midStr[0] = QString("%0").arg(midyacengsurf[i].IPList[j].x, 0, 'f', 5);
                midStr[1] = QString("%0").arg(midyacengsurf[i].IPList[j].y, 0, 'f', 5);
                midStr[2] = QString("%0").arg(midyacengsurf[i].IPList[j].z, 0, 'f', 5);
                midStr[3] = QString("%0").arg(midyacengsurf[i].IPList[j].dx, 0, 'f', 5);
                midStr[4] = QString("%0").arg(midyacengsurf[i].IPList[j].dy, 0, 'f', 5);
                midStr[5] = QString("%0").arg(midyacengsurf[i].IPList[j].dz, 0, 'f', 5);
                strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
                moutFile << strResultTri << "\n";
            }
            for (int j = 0; j < midyacengsurf[i].d.size(); j++)
            {
                strResultTri = QString("%0").arg(midyacengsurf[i].d[j], 0, 'f', 30);
                moutFile << strResultTri << "\n";
            }
            strResultTri = QObject::tr("ENDHRBFSurf");
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDHRBFSurfList");
        moutFile << strResultTri << "\n";

        strResultTri = QObject::tr("ENDREGION");
        moutFile << strResultTri << "\n";
    }
    outfile.close();
}



void ImplicitStorage::getTruefaceSection(QList<QList<Mdl_Triangle>>& implicitcut)
{
    //获取剖面设置精度
    bool ok;
    double jingdu = QInputDialog::getDouble(this, QString::fromLocal8Bit("输入建模精度"), QString::fromLocal8Bit("网格精度"), 0.5, 0, 1000, 5, &ok);
    if (ok) qDebug() << "value:" << jingdu;
    else return;

    //freecutplist
    lzbSectionmdl onecut;
    QList<Mdl_Face> truefacecut;
    //根据切割三角网得出真剖面,利用保存的隐式曲面
    QString demPoiPath = QString("%0/BiaomianSurf.txt").arg(QCoreApplication::applicationDirPath());
    QString goafSurfpath = QString("%0/GoafSurf.txt").arg(QCoreApplication::applicationDirPath());
    QString faultSurfpath = QString("%0/FaultSurf.txt").arg(QCoreApplication::applicationDirPath());
    QString regionSurfpath = QString("%0/RegionSurf.txt").arg(QCoreApplication::applicationDirPath());
    typedef void(*dllMainLayerModelling)(QList<Mdl_Point> plist, int jingdu, QString demPPath,
        QString GoafSurfPath, QString FaultSurfPath, QString RegionSurfPath, float modelElevation, QList<Mdl_Face>& trist);
    QLibrary toptriDLL("ImplictCutDLL.dll");
    if (toptriDLL.load())
    {
        dllMainLayerModelling tp = (dllMainLayerModelling)toptriDLL.resolve("lzbdllMultidataGetTrueCut_BJLine_KnowSurf");//导出函数
        tp(freecutplist, jingdu, demPoiPath, goafSurfpath, faultSurfpath, regionSurfpath, model_depth, truefacecut);
    }
    toptriDLL.unload();

    for (int i = 0; i < truefacecut.size(); i++)
    {
        QList<Mdl_Triangle> mid;
        for (int j = 0; j < truefacecut[i].triList.size(); j++)
        {
            mid.append(truefacecut[i].triList[j]);
        }
        implicitcut.append(mid);
    }
}

void ImplicitStorage::ImSurfSave(QList<lzbImSurf> TopSurflist, QList<lzbImSurf> GoafSurflist, QList<lzbImSurf> FaultSurflist, QList<lzbReionImSurf> RegionSurflist, QString surfPath)
{
    ////结果输出
    QFile outfile(surfPath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream moutFile(&outfile);
    QString strResultTri;

    //导出建模深度
    strResultTri = QObject::tr("MODELING_DEPTH:");
    moutFile << strResultTri << ":";
    strResultTri = QString("%0").arg(model_depth);
    moutFile << strResultTri << "\n";

    //导出模型边界
    strResultTri = QObject::tr("BOUNDARY_POINT");
    moutFile << strResultTri << "\n";
    for (int i = 0; i < boundary.PList.size(); i++)
    {
        QString midStr[2];
        midStr[0] = QString("%0").arg(boundary.PList[i].x, 0, 'f', 3);
        midStr[1] = QString("%0").arg(boundary.PList[i].y, 0, 'f', 3);
        strResultTri = QString("%0,%1").arg(midStr[0]).arg(midStr[1]);
        moutFile << strResultTri << "\n";
    }
    strResultTri = QObject::tr("END_BOUNDARY_POINT");
    moutFile << strResultTri << "\n";
    
    //导出各隐式面信息
    //导出顶面
    for (int i = 0; i < TopSurflist.size(); i++)
    {
        strResultTri = QObject::tr("TOPSURF").arg(TopSurflist[i].num);
        moutFile << strResultTri << "\n";
        for (int j = 0; j < TopSurflist[i].IPList.size(); j++)
        {
            QString midStr[6];
            midStr[0] = QString("%0").arg(TopSurflist[i].IPList[j].x, 0, 'f', 5);
            midStr[1] = QString("%0").arg(TopSurflist[i].IPList[j].y, 0, 'f', 5);
            midStr[2] = QString("%0").arg(TopSurflist[i].IPList[j].z, 0, 'f', 5);
            midStr[3] = QString("%0").arg(TopSurflist[i].IPList[j].dx, 0, 'f', 5);
            midStr[4] = QString("%0").arg(TopSurflist[i].IPList[j].dy, 0, 'f', 5);
            midStr[5] = QString("%0").arg(TopSurflist[i].IPList[j].dz, 0, 'f', 5);
            strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
            moutFile << strResultTri << "\n";
        }
        for (int j = 0; j < TopSurflist[i].d.size(); j++)
        {
            strResultTri = QString("%0").arg(TopSurflist[i].d[j], 0, 'f', 30);
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDTOPSURF");
        moutFile << strResultTri << "\n";
    }

    //导出采空区隐式面
    for (int i = 0; i < GoafSurflist.size(); i++)
    {
        strResultTri = QObject::tr("GOAFSURF_%0").arg(GoafSurflist[i].num);
        moutFile << strResultTri << "\n";
        for (int j = 0; j < GoafSurflist[i].IPList.size(); j++)
        {
            QString midStr[6];
            midStr[0] = QString("%0").arg(GoafSurflist[i].IPList[j].x, 0, 'f', 5);
            midStr[1] = QString("%0").arg(GoafSurflist[i].IPList[j].y, 0, 'f', 5);
            midStr[2] = QString("%0").arg(GoafSurflist[i].IPList[j].z, 0, 'f', 5);
            midStr[3] = QString("%0").arg(GoafSurflist[i].IPList[j].dx, 0, 'f', 5);
            midStr[4] = QString("%0").arg(GoafSurflist[i].IPList[j].dy, 0, 'f', 5);
            midStr[5] = QString("%0").arg(GoafSurflist[i].IPList[j].dz, 0, 'f', 5);
            strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
            moutFile << strResultTri << "\n";
        }
        for (int j = 0; j < GoafSurflist[i].d.size(); j++)
        {
            strResultTri = QString("%0").arg(GoafSurflist[i].d[j], 0, 'f', 30);
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDGOAFSURF");
        moutFile << strResultTri << "\n";
    }
    //导出断层隐式面
    for (int i = 0; i < FaultSurflist.size(); i++)
    {
        strResultTri = QObject::tr("FAULTSURF_%0").arg(FaultSurflist[i].faultnum);
        moutFile << strResultTri << "\n";
        for (int j = 0; j < FaultSurflist[i].IPList.size(); j++)
        {
            QString midStr[6];
            midStr[0] = QString("%0").arg(FaultSurflist[i].IPList[j].x, 0, 'f', 5);
            midStr[1] = QString("%0").arg(FaultSurflist[i].IPList[j].y, 0, 'f', 5);
            midStr[2] = QString("%0").arg(FaultSurflist[i].IPList[j].z, 0, 'f', 5);
            midStr[3] = QString("%0").arg(FaultSurflist[i].IPList[j].dx, 0, 'f', 5);
            midStr[4] = QString("%0").arg(FaultSurflist[i].IPList[j].dy, 0, 'f', 5);
            midStr[5] = QString("%0").arg(FaultSurflist[i].IPList[j].dz, 0, 'f', 5);
            strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
            moutFile << strResultTri << "\n";
        }
        for (int j = 0; j < FaultSurflist[i].d.size(); j++)
        {
            strResultTri = QString("%0").arg(FaultSurflist[i].d[j], 0, 'f', 30);
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDFAULTSURF");
        moutFile << strResultTri << "\n";
    }
    //导出区域隐式面
    for (int k = 0; k < RegionSurflist.size(); k++)
    {
        strResultTri = QObject::tr("REGION_%0").arg(RegionSurflist[k].num);
        moutFile << strResultTri << "\n";

        QList<lzbImSurf> midinstrusurf = RegionSurflist[k].InstruSruflist;
        QList<lzbImSurf> midyacengsurf = RegionSurflist[k].StratuSruflist;
        //导出侵入岩隐式面
        for (int i = 0; i < midinstrusurf.size(); i++)
        {
            strResultTri = QObject::tr("INSTRUSURF_%0").arg(midinstrusurf[i].num);
            moutFile << strResultTri << "\n";
            for (int j = 0; j < midinstrusurf[i].IPList.size(); j++)
            {
                QString midStr[6];
                midStr[0] = QString("%0").arg(midinstrusurf[i].IPList[j].x, 0, 'f', 5);
                midStr[1] = QString("%0").arg(midinstrusurf[i].IPList[j].y, 0, 'f', 5);
                midStr[2] = QString("%0").arg(midinstrusurf[i].IPList[j].z, 0, 'f', 5);
                midStr[3] = QString("%0").arg(midinstrusurf[i].IPList[j].dx, 0, 'f', 5);
                midStr[4] = QString("%0").arg(midinstrusurf[i].IPList[j].dy, 0, 'f', 5);
                midStr[5] = QString("%0").arg(midinstrusurf[i].IPList[j].dz, 0, 'f', 5);
                strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
                moutFile << strResultTri << "\n";
            }
            for (int j = 0; j < midinstrusurf[i].d.size(); j++)
            {
                strResultTri = QString("%0").arg(midinstrusurf[i].d[j], 0, 'f', 30);
                moutFile << strResultTri << "\n";
            }
            strResultTri = QObject::tr("ENDINSTRUSURF");
            moutFile << strResultTri << "\n";
        }
        //导出各亚层隐式面
        for (int i = 0; i < midyacengsurf.size(); i++)
        {
            strResultTri = QObject::tr("STRATUSURF_%0").arg(midyacengsurf[i].num);
            moutFile << strResultTri << "\n";
            for (int j = 0; j < midyacengsurf[i].IPList.size(); j++)
            {
                QString midStr[6];
                midStr[0] = QString("%0").arg(midyacengsurf[i].IPList[j].x, 0, 'f', 5);
                midStr[1] = QString("%0").arg(midyacengsurf[i].IPList[j].y, 0, 'f', 5);
                midStr[2] = QString("%0").arg(midyacengsurf[i].IPList[j].z, 0, 'f', 5);
                midStr[3] = QString("%0").arg(midyacengsurf[i].IPList[j].dx, 0, 'f', 5);
                midStr[4] = QString("%0").arg(midyacengsurf[i].IPList[j].dy, 0, 'f', 5);
                midStr[5] = QString("%0").arg(midyacengsurf[i].IPList[j].dz, 0, 'f', 5);
                strResultTri = QString("%0,%1,%2,%3,%4,%5").arg(midStr[0]).arg(midStr[1]).arg(midStr[2]).arg(midStr[3]).arg(midStr[4]).arg(midStr[5]);
                moutFile << strResultTri << "\n";
            }
            for (int j = 0; j < midyacengsurf[i].d.size(); j++)
            {
                strResultTri = QString("%0").arg(midyacengsurf[i].d[j], 0, 'f', 30);
                moutFile << strResultTri << "\n";
            }
            strResultTri = QObject::tr("ENDSTRATUSURF");
            moutFile << strResultTri << "\n";
        }
        strResultTri = QObject::tr("ENDREGION");
        moutFile << strResultTri << "\n";
    }

    //导出切割顺序等头信息
    strResultTri = QObject::tr("GOAFORDER");
    moutFile << strResultTri << ":";
    strResultTri = QString("1,2;");
    for (int i = 3; i <= GoafSurflist.size(); i += 2)
        strResultTri += QString("%0,%1;").arg(i).arg(i + 1);
    moutFile << strResultTri << "\n";

    strResultTri = QObject::tr("FAULTORDER");
    moutFile << strResultTri << ":";
    strResultTri = QString("1;");
    for (int i = 2; i <= FaultSurflist.size(); i++)
        strResultTri += QString("%0;").arg(i);
    moutFile << strResultTri << "\n";

    strResultTri = QObject::tr("REGIONNUM");
    moutFile << strResultTri << ":";
    strResultTri = QString("%0;").arg(RegionSurflist.size());
    moutFile << strResultTri << "\n";

    strResultTri = QObject::tr("REGIONORDER_INSTRU");
    moutFile << strResultTri << ":";
    strResultTri = QString("1;");
    for (int i = 2; i <= RegionSurflist[0].InstruSruflist.size(); i++)
        strResultTri += QString("%0;").arg(i);
    moutFile << strResultTri << "\n";

    strResultTri = QObject::tr("REGIONORDER_STRATU");
    moutFile << strResultTri << ":";
    strResultTri = QString("1;");
    for (int i = 2; i <= RegionSurflist[0].StratuSruflist.size(); i++)
        strResultTri += QString("%0;").arg(i);
    moutFile << strResultTri << "\n";

    outfile.close();
}
void ImplicitStorage::ImSurfRead(QList<lzbImSurf>& TopSurflist, QList<lzbImSurf>& GoafSurflist, QList<lzbImSurf>& FaultSurflist, QList<lzbReionImSurf>& RegionSurflist, QString surfPath)
{
    QString flag;
    QList <lzbPoint> midPList;
    QList <lzbImPoint> midImPList;
    QList <double> d;
    int num = 1;
    QString faultnum;

    //读取隐式面文件
    QFile mfiledtm(surfPath);
    mfiledtm.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream mymin(&mfiledtm);

    //读取深度及边界
    while (!mymin.atEnd())
    {
        QString inStr = mymin.readLine();
        if (inStr.contains("MODELING_DEPTH"))       //读取建模深度
        {
            model_depth = inStr.split(":").last().toDouble();
            continue;
        }

        QStringList strList = inStr.split(",");
        if (strList.size() == 2)
        {
            midPList.append(lzbPoint(strList[0].toDouble(), strList[1].toDouble(), 0));
        }
        if (inStr.contains("END_BOUNDARY_POINT"))   //读取边界
        {
            boundary = midPList;
            break;
        }
    }

    //读取各界面
    while (!mymin.atEnd())
    {
        QString inStr = mymin.readLine();
        if (inStr.contains("REGION"))
            break;
        if (inStr.contains("SURF")&&(!inStr.contains("END")))
        {
            flag = inStr.split("_").first();
            if (inStr.split("_").size() > 1)
            {
                if (flag == "FAULTSURF")
                    faultnum = inStr.split("_").last();
                else
                    num = inStr.split("_").last().toInt();
            }
            midImPList.clear();
            d.clear();
            continue;
        }
        if (inStr.contains("END"))
        {
            if (flag == "TOPSURF")
                TopSurflist.append(lzbImSurf(num, midImPList, d));
            if (flag == "GOAFSURF")
                GoafSurflist.append(lzbImSurf(num, midImPList, d));
            if (flag == "FAULTSURF")
                FaultSurflist.append(lzbImSurf(faultnum, midImPList, d));

            continue;
        }
        QStringList strList = inStr.split(",");

        if (strList.size() == 6)
        {
            midImPList.append(lzbImPoint(strList[0].toDouble(), strList[1].toDouble(), strList[2].toDouble(), strList[3].toDouble(), strList[4].toDouble(), strList[5].toDouble()));
        }
        if (strList.size() == 1)
        {
            if (inStr.split(".").size() != 1)
            {
                d.append(inStr.toDouble());
            }
        }
    }


    //读取区域面
    lzbReionImSurf regiomsurf;
    int regionnum = 1;
    int InstruSrufnum = 1;
    int StratuSrufnum = 1;
    QString inStr;
    while (!mymin.atEnd())
    {
        inStr = mymin.readLine();
        if (inStr.contains("REGION") && (!inStr.contains("END")))
        {
            regiomsurf.InstruSruflist.clear();
            regiomsurf.StratuSruflist.clear();
            continue;
        }
        if (inStr.contains("INSTRUSURF") && (!inStr.contains("END")))
        {
            midImPList.clear();
            d.clear();
            continue;
        }
        if (inStr.contains("STRATUSURF") && (!inStr.contains("END")))
        {
            midImPList.clear();
            d.clear();
            continue;
        }
        if (inStr.contains("ENDREGION"))
        {
            regiomsurf.num = regionnum;
            RegionSurflist.append(regiomsurf);
            regionnum++;
            InstruSrufnum = 1;
            StratuSrufnum = 1;
            continue;
        }
        if (inStr.contains("ENDINSTRUSURF"))
        {
            regiomsurf.InstruSruflist.append(lzbImSurf(InstruSrufnum, midImPList, d));
            InstruSrufnum++;
            continue;
        }
        if (inStr.contains("ENDSTRATUSURF"))
        {
            regiomsurf.StratuSruflist.append(lzbImSurf(StratuSrufnum, midImPList, d));
            StratuSrufnum++;
            continue;
        }
        if (inStr.contains("ORDER"))
            break;

        QStringList strList = inStr.split(",");

        if (strList.size() == 6)
        {
            midImPList.append(lzbImPoint(strList[0].toDouble(), strList[1].toDouble(), strList[2].toDouble(), strList[3].toDouble(), strList[4].toDouble(), strList[5].toDouble()));
        }
        if (strList.size() == 1)
        {
            if (inStr.split(".").size() != 1)
            {
                d.append(inStr.toDouble());
            }
        }
    }

    //读取切割顺序
    while (!mymin.atEnd())
    {
        if (inStr.contains("GOAFORDER"))
        {
            QStringList orderList = inStr.split(":").last().split(";");
            for (int i = 0; i < orderList.size(); i++)
            {
                QList<int> midlist;
                QStringList orderList2 = orderList[i].split(",");
                for (int j = 0; j < orderList2.size(); j++)
                    midlist.append(orderList2[j].toInt());
                goaforder.append(midlist);
            }
        }

        if (inStr.contains("FAULTORDER"))
        {
            QStringList orderList = inStr.split(":").last().split(";");
            for (int i = 0; i < orderList.size(); i++)
            {
                faultorder.append(orderList[i].toInt());
            }
        }

        if (inStr.contains("REGIONNUM"))
        {
            QString order = inStr.split(":").last().split(";").first();
            regionnum = order.toInt();
        }

        if (inStr.contains("REGIONORDER_INSTRU"))
        {
            QStringList orderList = inStr.split(":").last().split(";");
            for (int i = 0; i < orderList.size(); i++)
            {
                regionorder_instru.append(orderList[i].toInt());
            }
        }

        if (inStr.contains("EGIONORDER_STRATU"))
        {
            QStringList orderList = inStr.split(":").last().split(";");
            for (int i = 0; i < orderList.size(); i++)
            {
                regionorder_stratu.append(orderList[i].toInt());
            }
        }
        inStr = mymin.readLine();
    }
}
