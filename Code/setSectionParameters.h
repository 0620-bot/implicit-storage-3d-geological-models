#pragma once

#include <QWidget>
#include "ui_setSectionParameters.h"
#include "lzbBase.h"

class setSectionParameters : public QWidget
{
	Q_OBJECT

public:
	setSectionParameters(QWidget *parent = Q_NULLPTR);
	~setSectionParameters();

private:
	Ui::setSectionParameters ui;

	void initslot();

	QString implicitresultPath;
	QList<Mdl_Point> cutpoint;
	double Step, n;
	QString outputPath/* = QString("D:/C++/ImplicitStorage/x64/Release/multiscaleModel")*/;


private slots:

	void chooseImplicitSavePath();
	void chooseSectionSavePath();

	void define();
	void cancle();

signals:
	void completeSetSection(QString, QList<Mdl_Point>, double, double, QString);

};
