#pragma once
#include"Mdl_BasicClass.h"

#include<QStringList>
#include"qvector3d.h"
#include"qpair.h"
#include"qhash.h"
#include"qprogressdialog.h"
#include <Qlist>
#include <qdockwidget.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qfiledialog.h>
#include <qstandarditemmodel.h>
#include <QTime>
#include <qmessagebox.h>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qinputdialog.h>
#include <qfile.h>
#include <qdir.h>
#include <QMutex>
#include <QTime>
#include <qcoreapplication.h>

const double epsilon = 0.0001;

class lzbPoint
{
public:
	lzbPoint() {}
	lzbPoint(int, double, double, double);
	lzbPoint(double, double, double);
	lzbPoint(double, double);
	lzbPoint(Mdl_Point);

	inline bool lzbPoint::operator ==(const lzbPoint& p1) const
	{
		if (abs(x - p1.x) < epsilon && abs(y - p1.y) < epsilon && abs(z - p1.z) < epsilon)
			return true;
		return false;
	}
	//inline bool operator==(const lzbPoint& other) const 
	//{
	//	return x == other.x && y == other.y && z == other.z;
	//}
	inline lzbPoint lzbPoint::operator +(const lzbPoint& p1) const
	{
		return lzbPoint(x + p1.x, y + p1.y, z + p1.z);
	}
	inline lzbPoint lzbPoint::operator -(const lzbPoint& p1) const
	{
		return lzbPoint(x - p1.x, y - p1.y, z - p1.z);
	}

	Mdl_Point toMdlPoint();
	double distanceP_XY(lzbPoint);

	// 自定义哈希函数
	inline uint qHash(const lzbPoint& key, uint seed = 0) {
		std::size_t h1 = std::hash<double>{}(key.x);
		std::size_t h2 = std::hash<double>{}(key.y);
		std::size_t h3 = std::hash<double>{}(key.z);
		std::size_t h4 = std::hash<int>{}(key.pNum);

		return static_cast<uint>(h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ seed);
	}

public:
	int pNum;
	double x, y, z;
};

//属性点
class lzbAttriPoint : public lzbPoint
{
public:
	lzbAttriPoint() {}
	~lzbAttriPoint() {}

	lzbAttriPoint(lzbPoint p)
		:lzbPoint(p), value(0) {}
	lzbAttriPoint(double xx, double yy, double zz, double VV);



public:
	double value;
	double r, g, b;
};

class lzbImPoint :public lzbPoint
{
public:
	lzbImPoint() {}
	lzbImPoint(double, double, double, double, double, double);

public:
	double dx, dy, dz;
};

class lzbPLine
{
public:
	lzbPLine() {}
	lzbPLine(QList<Mdl_Point>);
	lzbPLine(QList<lzbPoint>);
	lzbPLine(Mdl_Poly);
	~lzbPLine() {}

	QList<Mdl_Point> toMdl_PList();
	Mdl_Poly toMdl_Poly();
	
	//剔重
	void RemoveRepeat();
	//反转
	void Reserve();
	//是否顺时针
	bool isClockwise();

	//计算面积
	double CaculatePolyArea();



public:
	int i;
	QString Name;
	QString Information;
	bool isclose = false;
	QList<int> color;
	QList<lzbPoint> PList;
};

class lzbLine
{
public:
	lzbLine() {}
	lzbLine(Mdl_Point, Mdl_Point);
	lzbLine(lzbPoint, lzbPoint);
	lzbLine(const lzbLine &line);

	~lzbLine() {}
	void initABC();
	double getPointLineValue(lzbPoint);
	double pointtolineDistance(lzbPoint);
	double getPointLineValue(Mdl_Point);
	double pointtolineDistance(Mdl_Point);
	bool pointisinline(lzbPoint);
	bool pointisinline(Mdl_Point);

	inline bool lzbLine::operator ==(const lzbLine& l1) const
	{
		if ((l1.p1 == p1 && l1.p2 == p2) || (l1.p1 == p2 && l1.p2 == p1))
			return true;
		else
			return false;
	}


public:
	int LNum;
	lzbPoint p1, p2;
	double A, B, C;
};

class lzbSpline	//曲线(二维)
{
public:
	lzbSpline() {}
	lzbSpline(QList<lzbPoint> cp) { controlPoints = cp; }
	lzbSpline(QList<Mdl_Point> cp);
	~lzbSpline() {}

	QList<lzbPoint> computeSpline(int num);

public:
	int CNum;
	QList<lzbPoint> controlPoints;
};

class lzbTriangle
{
public:
	lzbTriangle() {};
	lzbTriangle(lzbPoint pp1, lzbPoint pp2, lzbPoint pp3) { p1 = pp1; p2 = pp2; p3 = pp3; }
	lzbTriangle(lzbAttriPoint pp1, lzbAttriPoint pp2, lzbAttriPoint pp3) { p1 = pp1; p2 = pp2; p3 = pp3; }
	lzbTriangle(Mdl_Triangle);
	~lzbTriangle() {};

	Mdl_Triangle toMdlTri();
	double caculateArea();

public:
	int num;
	lzbAttriPoint p1, p2, p3;
};

class lzbFault
{
public:

	QList<Mdl_Point> toMdlfaultPointQList();
	std::vector<Mdl_Point> toMdlfaultPointVector();
	QList<Mdl_Triangle> toMdlfaultfaceQlist();
	void setFaultP(QList<Mdl_Point>);

	void setFaultVP(QList<lzbPoint> VP) { pointvector = VP; }
	void setFaultFace(QList<Mdl_Triangle>);
	void setNorR(QList<bool>);
	void setfalutDistance(QList<double>);

public:
	int num;	//建模序号
	QString name;
	QList<lzbPoint> faultpoint;
	QList<lzbPoint> pointvector;
	QList<bool> NorR;	//正或逆断层（true：正；false：逆）
	QList<double> faultD;	//断层移动距离
	bool isright = false;
	QList<lzbTriangle> faultface;
};

class lzbSurface
{
public:
	QList<lzbTriangle> trilist;
};

class lzbSectionmdl
{
public:
	lzbSectionmdl() {}
	lzbSectionmdl(QList<Mdl_Point> pl) { plist = pl; }
	lzbSectionmdl(QList<lzbPoint> pl) { for (int i = 0; i < pl.size(); i++) plist.append(pl[i].toMdlPoint()); }
	lzbSectionmdl(QString name, QList<Mdl_Point> pl) { plist = pl; CutName = name; }

	~lzbSectionmdl() {}

	void setCutName(QString name) { CutName = name; }
	void setTriList(QList<QList<Mdl_Triangle>> tlist) { cutTrilist = tlist; }
	void setGeoCut(Mdl_CutSection cut);

public:
	QString CutName;
	QList<Mdl_Point> plist;	//按顺序存剖面点(为了方便暂时使用mdl)
	QList<lzbPLine> PLineList;	//剖面上的线，比如剖面上与标记面相交的线
	QList<QList<Mdl_Triangle>> cutTrilist;	//剖面的三角面(同样为了方便暂时使用mdl)
	QList<Mdl_Face> cutFace;	//剖面的三角面(同样为了方便暂时使用mdl)
};


class lzbImSurf		//隐式面类，用于隐式面临时存储
{
public:
	lzbImSurf() {}
	lzbImSurf(QList<lzbImPoint> iplist, QList<double>dd) { IPList = iplist; d = dd; }
	lzbImSurf(int n, QList<lzbImPoint> iplist, QList<double>dd) { num = n; IPList = iplist; d = dd; }
	lzbImSurf(QString name, QList<lzbImPoint> iplist, QList<double>dd) { faultnum = name; IPList = iplist; d = dd; }

public:
	int num;
	QString faultnum;
	QList<lzbImPoint> IPList;
	QList<double> d;
};
class lzbReionImSurf		//区域隐式面类，用于存储一个区域的隐式面(包括侵入岩的和地层的)
{


public:
	int num;
	QList<lzbImSurf> InstruSruflist;
	QList<lzbImSurf> StratuSruflist;
};




////用于跟动态库交换数据的变量
class transPoint
{
public:
	transPoint() {}
	transPoint(double x1, double y1, double z1)
		:x(x1), y(y1), z(z1)
	{}

	double x, y, z;

protected:
private:
};

class transPoly
{
public:
	transPoly() {}
	transPoly(QList<transPoint> p)
		:plist(p)
	{}

	QList<transPoint> plist;

protected:
private:
};

class transTetra
{
public:
	transTetra() {}
	transTetra(float coord[12], bool myFlag[4])
	{
		for (int i = 0; i < 4; i++)
		{
			tetraVertex[i] = transPoint(coord[3 * i], coord[3 * i + 1], coord[3 * i + 2]);
			boundFlag[i] = myFlag[i];
		}
	}

	transPoint tetraVertex[4];
	bool boundFlag[4];

protected:
private:
};

class lzbtransTetra
{
public:
	lzbtransTetra() {}
	lzbtransTetra(double coord[12], bool myFlag[4])
	{
		for (int i = 0; i < 4; i++)
		{
			tetraVertex[i] = transPoint(coord[3 * i], coord[3 * i + 1], coord[3 * i + 2]);
			boundFlag[i] = myFlag[i];
		}
	}

	transPoint tetraVertex[4];
	bool boundFlag[4];

protected:
private:
};