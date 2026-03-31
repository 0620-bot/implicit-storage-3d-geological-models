#ifndef BASICCLASS_H
#define BASICCLASS_H

#include <list>
#include <vector>
#include <string>
#include <deque>

//using namespace std;

////点
struct Mdl_Point 
{
	int pNum;
	double x,y,z;
};
////线段
struct Mdl_Line 
{
	int lineNum;
	Mdl_Point p1,p2;
};
////多段线
struct Mdl_Poly 
{
	int polyNum;
	int geoColor;
	std::vector <Mdl_Point> pList;
	bool closeFlag;		////闭合标志
};

////三角形
struct Mdl_Triangle
{
	int triNum;
	Mdl_Point p1,p2,p3;
	bool isOverlap;
	int AdjacentTriID_1, AdjacentTriID_2, AdjacentTriID_3;
	int displayOrder;
	int Level;
	int numFlag;/////钻孔数据点层
};
////四面体
struct Mdl_Tetrahedra
{
	int tetraNum;
	Mdl_Point p1, p2, p3, p4;
	bool flag1, flag2, flag3, flag4;
};
////面
struct Mdl_Face 
{
	int faceNum;
	std::deque <Mdl_Point> triPointList;
	std::deque <Mdl_Triangle> triList;
};
////产状
struct Mdl_Attitude 
{
	Mdl_Point location;
	double dip,dipAng;
};
////带产状的点，不同于产状
struct Mdl_AttiPoint : Mdl_Point
{
	double dip, dipAng;		////倾向、倾角
};
////钻孔点 为了绘制钻孔点而设置
struct DrillPoint : Mdl_Point
{
	double LayerThickness; //为了便于绘制钻孔,分层厚度
	float r,g,b;//颜色
};
////每个点带产状的多段线，为什么建这个类型？因为这是一个由一系列带产状点组成的多段线，这个多段线不一定只用于弧段，因此多建一层数据
struct Mdl_AttiPoly
{
	std::vector <Mdl_AttiPoint> attiPointList;
};
////剖面点
struct Point_RouteSec
{
	std::string Code;
	std::string STRAPHA;
	int  RegId;
	int  ArcId;
	double TwoD_x;
	double TwoD_y;
	double ThreeD_x;
	double ThreeD_y;
	double ThreeD_z;
	std::string LITHO;
	int REGCOLOR;
};
////附在弧段上的约束线
struct Mdl_ConstraintLine
{
	int lineNum;
	Mdl_Poly polyLine;   ////约束线上的多段线数据
	int arcID;
	int pointID;
};

////弧段
struct Mdl_Arc
{
	int arcNum;
	Mdl_AttiPoly polyLine;	////弧段所带的多段线数据，带产状
	int direction;
	int LeftRegion , RightRegion;
	int faultID;
	std::vector <Mdl_ConstraintLine> consLineList;////约束线
};
////区
struct Mdl_Region 
{
	int regNum;
	std::vector <Mdl_Arc> arcList;
	int geoColor;
	std::string strapha, litho, geoName;

};
////断层
struct Mdl_Fault 
{
	int objectId;
	Mdl_AttiPoly faultPoly;	////断层出露线是有产状的
	std::string lineCode;
	Mdl_Face faultFace;
};
////定位线
struct Mdl_RouteSecLocation 
{
	int lineID;
	////两个平面点，一个空间点
	Mdl_Point planePoint1, planePoint2, realPoint;
};
////剖面区
struct Mdl_SecReg 
{
	int regId;
	int geoColor;
	std::string STRAPHA;
	std::string LITHO;

	/////剖面上各个区的弧段，其中planeArcList为平面图上的弧段集合，spatialArcList为输出得到的空间弧段集合
	std::vector <Mdl_Arc> planeArcList, spatialArcList;
	////输出剖面三维模型
	Mdl_Face regFace;
};

//路线剖面
struct Mdl_RouteRegSection
{
	std::string secCode;

	////定位线
	std::vector <Mdl_RouteSecLocation> locationLineList;

	////初始化里面的二维信息
	std::vector <Mdl_SecReg> secRegList;

	////剖面约束线
	std::vector <Mdl_Poly> consPolyList;

};
////单纯线转化
struct Mdl_RouteLineSection
{
	std::string secCode;
	////剖面线
	std::vector <Mdl_Poly> planeLineList, spatialLineList;
	////定位线
	std::vector <Mdl_RouteSecLocation> locationLineList;
};

//剖面信息
struct Mdl_RouteSection
{
	std::string secCode;

	//路线剖面
	Mdl_RouteRegSection regSection;

	//单纯剖面线
	Mdl_RouteLineSection lineSection;

	//定位线
	std::vector <Mdl_RouteSecLocation> locationLineList;
};
////钻孔
class DrillLayer
{
public:
	int layerNumber;			////钻孔编号
	std::vector <Mdl_Point> pList;	////地层采样点
	std::vector <DrillPoint> DrillPlist; //钻孔中每层点以及其信息

};
////地质体
struct Mdl_Body 
{
	int regNum;
	Mdl_Face bodyFace;
	std::deque <Mdl_Tetrahedra> tetraList;
	std::string strapha, litho, geoName;
	int geoColor;
};
////剖切面
struct Mdl_CutSection 
{
	std::string lineCode;		////剖面代码
	//std::vector <int> regNumList;
	//std::vector <Mdl_Face> cutFaceList;
	std::deque <Mdl_Body> cutFaceList;		////剖切得到的剖面模型
};

struct Mdl_ConsLine
{
	int lineNum;

	std::string lineCode;////剖面代码
	int arcNoID;
	int pointID;

	Mdl_Poly consPoly;
};

////地质图中包含的内容
struct Mdl_GeoMap 
{
	////建模需要赋值的变量
	std::vector <Mdl_Point> demPointList;			////DEM高程点
	std::vector <Mdl_Attitude> attitudeList;		////产状
	std::vector <Mdl_Region> regionList;			////区
	std::vector <Mdl_Fault> faultList;				////断层
	std::vector <Mdl_RouteLineSection> lineSecList;		////剖面线
	std::vector <Mdl_RouteRegSection> regSecList;	////路线剖面

	std::vector <Mdl_ConsLine> consLineList;////单体建模约束线

	std::vector <Mdl_RouteSection> sectionList;////剖面信息
	////钻孔
	std::vector <DrillLayer> DrillLayerList;//////钻孔信息
	int maxLayerNum;//钻孔最大层数

	////建模参数
	int numModelDemPoint;		////建模用的DEM点数
	std::string arcOrder;				////切割序列 
	double modelAltitude;		////建模标高
	char *consSecCodeList;		////参与约束的剖面
	double consEpsilon;			////匹配阈值
	bool isSecConstrain;		////是否剖面约束
	bool isAttitudeCalculated;	////产状数据是否已经计算好，如果赋值为false，SBMSDLL将在程序中进行计算
	double gridLen;				////网格距离约束，在进行第四系建模等时候，需要网格距离约束

	////建模返回的结果
	std::deque <Mdl_Body> bodyList;				////建模结果
	std::deque <Mdl_Body> explictBodyList;      ////显式建模结果
	std::vector <Mdl_CutSection> cutSectionList;	////剖切面

	std::vector <Mdl_Face> GeologicalFormationList;  ////构造面
};

#endif