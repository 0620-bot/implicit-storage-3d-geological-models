#include "setSectionParameters.h"

setSectionParameters::setSectionParameters(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initslot();
	//cutpoint.append(lzbPoint(85465.443, 50570, 0).toMdlPoint());
	//cutpoint.append(lzbPoint(92432.921, 50540, 0).toMdlPoint());
	//ui.lineEditStip->setText("50");
	//ui.lineEditN->setText("30");
}

setSectionParameters::~setSectionParameters()
{

}

void setSectionParameters::initslot()
{
	connect(ui.chooseImplicitSaveResult, SIGNAL(clicked(bool)), this, SLOT(chooseImplicitSavePath()));
	connect(ui.chooseoutputpath, SIGNAL(clicked(bool)), this, SLOT(chooseSectionSavePath()));

	connect(ui.difine, SIGNAL(clicked(bool)), this, SLOT(define()));
	connect(ui.cancle, SIGNAL(clicked(bool)), this, SLOT(cancle()));
}

void setSectionParameters::chooseImplicitSavePath()
{
	QString curpath = QCoreApplication::applicationDirPath();
	QString ImplicitResultPath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Choose Implicit Save Result"), QCoreApplication::applicationDirPath(), QString::fromLocal8Bit("Implicit Save Result(*.txt);;all file(*.*)"));
	if (ImplicitResultPath.isEmpty())
		return;

	ui.ImplicitSaveResultlineEdit->setText(ImplicitResultPath);
	implicitresultPath = ImplicitResultPath;
}

void setSectionParameters::chooseSectionSavePath()
{
	QString curpath = QCoreApplication::applicationDirPath();
	QString SectionPath = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("Choose Implicit Save Result"), QCoreApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (SectionPath.isEmpty())
		return;

	ui.lineEditoutPutPath->setText(SectionPath);
	outputPath = SectionPath;
}

void setSectionParameters::define()
{
	QString Point = ui.textEditSectionPoint->toPlainText();
	QStringList pointlist = Point.split("\n");
	for (auto point : pointlist)
	{
		QStringList pointXYZ = point.split(",");
		if (pointXYZ.size() == 3)
			cutpoint.append(lzbPoint(pointXYZ[0].toDouble(), pointXYZ[1].toDouble(), pointXYZ[2].toDouble()).toMdlPoint());
		else if(pointXYZ.size() == 2)
			cutpoint.append(lzbPoint(pointXYZ[0].toDouble(), pointXYZ[1].toDouble(), 0).toMdlPoint());
	}

	Step = ui.lineEditStip->text().toDouble();
	n = ui.lineEditN->text().toInt();
	emit completeSetSection(implicitresultPath, cutpoint, Step, n, outputPath);

	this->close();
}

void setSectionParameters::cancle()
{
	this->close();
}
