#include "setModellingParameters.h"

setModellingParameters::setModellingParameters(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initslot();

	////²âÊÔ Ô¤ÏÈÊäÈë
	//ui.ImplicitSaveResultlineEdit->setText(implicitresultPath);
	//ui.minScalelineEdit->setText(QString("%0").arg(100));
	//ui.maxScalelineEdit->setText(QString("%0").arg(400));
	//ui.steplineEdit->setText(QString("%0").arg(100));
	//ui.outputpathlineEdit->setText(modeloutputPath);
}

setModellingParameters::~setModellingParameters()
{
}

void setModellingParameters::initslot()
{
	connect(ui.chooseImplicitSaveResult, SIGNAL(clicked(bool)), this, SLOT(chooseImplicitSavePath()));
	connect(ui.chooseoutputpath, SIGNAL(clicked(bool)), this, SLOT(choosemultimodelSavePath()));

	connect(ui.Define, SIGNAL(clicked(bool)), this, SLOT(define()));
	connect(ui.Cancle, SIGNAL(clicked(bool)), this, SLOT(cancle()));
}

void setModellingParameters::chooseImplicitSavePath()
{
	QString curpath = QCoreApplication::applicationDirPath();
	QString ImplicitResultPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Choose Implicit Save Result"), QCoreApplication::applicationDirPath(), QString::fromLocal8Bit("Implicit Save Result(*.txt);;all file(*.*)"));
	if (ImplicitResultPath.isEmpty())
		return;

	ui.ImplicitSaveResultlineEdit->setText(ImplicitResultPath);
	implicitresultPath = ImplicitResultPath;
}


void setModellingParameters::choosemultimodelSavePath()
{
	QString curpath = QCoreApplication::applicationDirPath();
	QString ModelPath = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("Choose Implicit Save Result"), QCoreApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (ModelPath.isEmpty())
		return;

	ui.outputpathlineEdit->setText(ModelPath);
	modeloutputPath = ModelPath;
}

void setModellingParameters::define()
{
	minScale = ui.minScalelineEdit->text().toDouble();
	maxScale = ui.maxScalelineEdit->text().toDouble();
	Step = ui.steplineEdit->text().toDouble();
	emit completeSet(implicitresultPath, modeloutputPath, minScale, maxScale, Step);

	this->close();
}

void setModellingParameters::cancle()
{
	this->close();
}
