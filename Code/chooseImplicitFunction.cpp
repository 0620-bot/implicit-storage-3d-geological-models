#include "chooseImplicitFunction.h"

chooseImplicitFunction::chooseImplicitFunction(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initslot();

	////²âÊÔ Ô¤ÏÈÊäÈë
	//ui.TopSurfPathlineEdit->setText(TopSurfPathStr);
	//ui.GoafSurfPathlineEdit->setText(GoafSurfPathStr);
	//ui.FaultSurfPathlineEdit->setText(FaultSurfPathStr);
	//ui.LayerSurfPathlineEdit->setText(RegionSurfPathStr);
	//ui.BPPathlineEdit->setText(BPPathStr);
	//readPointlist(BPPlist, BPPathStr);
	//ui.ModelDepthlineEdit->setText(QString("%0").arg(ModelDepth));
}

chooseImplicitFunction::~chooseImplicitFunction()
{
}

void chooseImplicitFunction::initslot()
{	
	connect(ui.TopSurfPathpushButton, SIGNAL(clicked(bool)), this, SLOT(chooseTopSurfPath()));
	connect(ui.GoafSurfPathpushButton, SIGNAL(clicked(bool)), this, SLOT(chooseGoafSurfPath()));
	connect(ui.FaultSurfPathpushButton, SIGNAL(clicked(bool)), this, SLOT(chooseFaultSurfPath()));
	connect(ui.LayerSurfPathpushButton, SIGNAL(clicked(bool)), this, SLOT(chooseRegionSurfPath()));
	connect(ui.BPPathpushButton, SIGNAL(clicked(bool)), this, SLOT(chooseBPPath()));

	connect(ui.Define, SIGNAL(clicked(bool)), this, SLOT(define()));
	connect(ui.Cancle, SIGNAL(clicked(bool)), this, SLOT(cancle()));
}

void chooseImplicitFunction::readPointlist(QList<Mdl_Point>& TempMdlPList, QString PListStr, double baseX, double baseY)
{
	QFile mfiledtm(PListStr);
	mfiledtm.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream mymin(&mfiledtm);

	while (!mymin.atEnd())
	{
		QString inStr = mymin.readLine();
		QStringList strList = inStr.split(",");
		if (strList.size() < 2)
			continue;
		Mdl_Point  p;
		p.x = strList[0].toDouble() + baseX;
		p.y = strList[1].toDouble() + baseY;
		p.z = 0;
		TempMdlPList.append(p);
	}
	mfiledtm.close();
}

void chooseImplicitFunction::chooseTopSurfPath()
{
	QString curpath = QCoreApplication::applicationDirPath() + "/ImplicitModel";
	QString TopSurfPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Choose Top Surface"), curpath, QString::fromLocal8Bit("Implicit file(*.txt);;all file(*.*)"));
	if (TopSurfPath.isEmpty())
		return;

	ui.TopSurfPathlineEdit->setText(TopSurfPath);
	TopSurfPathStr = TopSurfPath;
}
void chooseImplicitFunction::chooseGoafSurfPath()
{
	QString curpath = QCoreApplication::applicationDirPath() + "/ImplicitModel";
	QString GoafSurfPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Choose Goaf Surface"), curpath, QString::fromLocal8Bit("Implicit file(*.txt);;all file(*.*)"));
	if (GoafSurfPath.isEmpty())
		return;

	ui.GoafSurfPathlineEdit->setText(GoafSurfPath);
	GoafSurfPathStr = GoafSurfPath;
}
void chooseImplicitFunction::chooseFaultSurfPath()
{
	QString curpath = QCoreApplication::applicationDirPath() + "/ImplicitModel";
	QString FaultSurfPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Choose Fault Surface"), curpath, QString::fromLocal8Bit("Implicit file(*.txt);;all file(*.*)"));
	if (FaultSurfPath.isEmpty())
		return;

	ui.FaultSurfPathlineEdit->setText(FaultSurfPath);
	FaultSurfPathStr = FaultSurfPath;
}
void chooseImplicitFunction::chooseRegionSurfPath()
{
	QString curpath = QCoreApplication::applicationDirPath() + "/ImplicitModel";
	QString RegionSurfPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Choose Layer Surface"), curpath, QString::fromLocal8Bit("Implicit file(*.txt);;all file(*.*)"));
	if (RegionSurfPath.isEmpty())
		return;

	ui.LayerSurfPathlineEdit->setText(RegionSurfPath);
	RegionSurfPathStr = RegionSurfPath;
}
void chooseImplicitFunction::chooseBPPath()
{
	QString curpath = QCoreApplication::applicationDirPath() + "/ImplicitModel";
	QString BPPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Choose Boundry Points"), curpath, QString::fromLocal8Bit("Point file(*.txt);;all file(*.*)"));
	if (BPPath.isEmpty())
		return;

	ui.BPPathlineEdit->setText(BPPath);
	BPPathStr = BPPath;

	readPointlist(BPPlist, BPPathStr);
}

void chooseImplicitFunction::define()
{
	ModelDepth = ui.ModelDepthlineEdit->text().toDouble();
	emit completeChoose(TopSurfPathStr, GoafSurfPathStr, FaultSurfPathStr, RegionSurfPathStr, BPPlist, ModelDepth);

	this->close();
}

void chooseImplicitFunction::cancle()
{
	this->close();
}
