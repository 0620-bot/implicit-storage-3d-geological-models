#pragma once

#include <QWidget>
#include "ui_setModellingParameters.h"
#include "qfiledialog.h"

class setModellingParameters : public QWidget
{
	Q_OBJECT

public:
	setModellingParameters(QWidget *parent = Q_NULLPTR);
	~setModellingParameters();

private:
	Ui::setModellingParameters ui;

	void initslot();

	QString implicitresultPath/* = QString("D:/C++/ImplicitStorage/x64/Release/ModelSurf.txt")*/;
	double minScale, maxScale, Step;
	QString modeloutputPath/* = QString("D:/C++/ImplicitStorage/x64/Release/multiscaleModel")*/;

private slots:

	void chooseImplicitSavePath();
	void choosemultimodelSavePath();


	void define();
	void cancle();

signals:
	void completeSet(QString, QString, double, double, double);

};
