#pragma once

#include <QWidget>
#include "ui_chooseImplicitFunction.h"
#include "qfiledialog.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qcoreapplication.h"

#include "Mdl_BasicClass.h"

class chooseImplicitFunction : public QWidget
{
	Q_OBJECT

public:
	chooseImplicitFunction(QWidget *parent = Q_NULLPTR);
	~chooseImplicitFunction();


private:
	Ui::chooseImplicitFunction ui;

	void initslot();
	void readPointlist(QList<Mdl_Point>& TempMdlPList, QString PListStr, double baseX = 0, double baseY = 0);

	//QString TopSurfPathStr = QString("D:/C++/ImplicitStorage/x64/Release/BiaomianSurf.txt");
	//QString GoafSurfPathStr = QString("D:/C++/ImplicitStorage/x64/Release/GoafSurf.txt");
	//QString FaultSurfPathStr = QString("D:/C++/ImplicitStorage/x64/Release/FaultSurf.txt");
	//QString RegionSurfPathStr = QString("D:/C++/ImplicitStorage/x64/Release/RegionSurf.txt");
	//QString BPPathStr = QString("D:/C++/ImplicitStorage/x64/Release/BoundryPoint.txt");
	QString TopSurfPathStr;
	QString GoafSurfPathStr;
	QString FaultSurfPathStr;
	QString RegionSurfPathStr;
	QString BPPathStr;
	QList<Mdl_Point> BPPlist;
	double ModelDepth;

private slots:

	void chooseTopSurfPath();
	void chooseGoafSurfPath();
	void chooseFaultSurfPath();
	void chooseRegionSurfPath();
	void chooseBPPath();


	void define();
	void cancle();

signals:
	void completeChoose(QString, QString, QString, QString, QList<Mdl_Point>, double);
};
