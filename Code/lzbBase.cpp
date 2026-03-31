#include "lzbBase.h"

lzbPoint::lzbPoint(int n, double xx, double yy, double zz)
{
    pNum = n;
    x = xx;
    y = yy;
    z = zz;
}

lzbPoint::lzbPoint(double xx, double yy, double zz)
{
    x = xx;
    y = yy;
    z = zz;
}

lzbPoint::lzbPoint(double xx, double yy)
{
    x = xx;
    y = yy;
}

lzbPoint::lzbPoint(Mdl_Point pp)
{
    pNum = pp.pNum;
    x = pp.x;
    y = pp.y;
    z = pp.z;
}

Mdl_Point lzbPoint::toMdlPoint()
{
    Mdl_Point p;
    p.pNum = pNum;
    p.x = x;
    p.y = y;
    p.z = z;

    return p;
}

lzbImPoint::lzbImPoint(double xx, double yy, double zz, double dxx, double dyy, double dzz)
{
    x = xx; y = yy; z = zz; dx = dxx; dy = dyy; dz = dzz;
}


double lzbPoint::distanceP_XY(lzbPoint pp)
{
    double dis = sqrt((x - pp.x) * (x - pp.x) + (y - pp.y) * (y - pp.y));
    return dis;
}


lzbLine::lzbLine(Mdl_Point pp1, Mdl_Point pp2)
{
    p1 = lzbPoint(pp1);
    p2 = lzbPoint(pp2);
    initABC();
}

lzbLine::lzbLine(lzbPoint pp1, lzbPoint pp2)
{
    p1 = pp1;
    p2 = pp2;
    initABC();
}

lzbLine::lzbLine(const lzbLine& line)
{
    LNum = line.LNum;
    p1 = line.p1;
    p2 = line.p2;
    A = line.A;
    B = line.B;
    C = line.C;
}

void lzbLine::initABC()
{
    A = p2.y - p1.y;
    B = p1.x - p2.x;
    C = p2.x * p1.y - p1.x * p2.y; 
    if (B < 0)//保证B不为负，则可直接判断是在直线上方还是下方
        A = -A, B = -B, C = -C;
}

double lzbLine::getPointLineValue(lzbPoint pp)
{
    double value;
    value = A * pp.x + B * pp.y + C;
    return value;
}

double lzbLine::pointtolineDistance(lzbPoint pp)
{
    double dis;
    dis = abs(getPointLineValue(pp)) / sqrt(pow(A, 2) + pow(B, 2));
    return dis;
}

double lzbLine::getPointLineValue(Mdl_Point pp)
{
    return getPointLineValue(lzbPoint(pp));
}

double lzbLine::pointtolineDistance(Mdl_Point pp)
{
    return pointtolineDistance(lzbPoint(pp));
}

bool lzbLine::pointisinline(lzbPoint pp)
{
    if (pointtolineDistance(pp) < 0.004)
        return true;
    else
        return false;
}

bool lzbLine::pointisinline(Mdl_Point pp)
{
    return pointisinline(lzbPoint(pp));
}


lzbSpline::lzbSpline(QList<Mdl_Point> cp)
{
    int n = cp.size();
    for (int i = 0; i < n; i++)
    {
        controlPoints.append(cp[i]);
    }
}

QList<lzbPoint> lzbSpline::computeSpline(int num)
{
    int n = controlPoints.size() - 1;
    std::vector<double> h(n), alpha(n), l(n + 1), mu(n + 1), z(n + 1);
    std::vector<double> a(n + 1), b(n), c(n + 1), d(n);
    
    // 初始化 h 和 alpha
    for (int i = 0; i < n; ++i) {
        h[i] = controlPoints[i + 1].x - controlPoints[i].x;
    }
    
    // 初始化 alpha
    for (int i = 1; i < n; ++i) {
        alpha[i] = (3 / h[i]) * (controlPoints[i + 1].y - controlPoints[i].y) - (3 / h[i - 1]) * (controlPoints[i].y - controlPoints[i - 1].y);
    }
    
    // 设置边界条件
    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;
    
    // 构造 l, mu, z
    for (int i = 1; i < n; ++i) {
        l[i] = 2 * (controlPoints[i + 1].x - controlPoints[i - 1].x) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    
    // 设置边界条件
    l[n] = 1;
    z[n] = 0;
    c[n] = 0;
    
    // 计算 c, b, d
    for (int j = n - 1; j >= 0; --j) {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] = (controlPoints[j + 1].y - controlPoints[j].y) / h[j] - h[j] * (c[j + 1] + 2 * c[j]) / 3;
        d[j] = (c[j + 1] - c[j]) / (3 * h[j]);
        a[j] = controlPoints[j].y;
    }
    
    // 计算样条曲线上的点
    QList<lzbPoint> splinePoints;
    for (int i = 0; i < n; ++i) {
        double deltaX = h[i] / 10.0; // 划分为10个小区间
        double delta = 1.0 / double(num);
        for (double t = 0; t <= 1; t += delta) {
            double x = controlPoints[i].x + t * h[i];
            double y = a[i] + b[i] * t * h[i] + c[i] * pow(t * h[i], 2) + d[i] * pow(t * h[i], 3);
            splinePoints.push_back(lzbPoint(x, y, 0));
        }
    }
    
    return splinePoints;
}

QList<Mdl_Point> lzbFault::toMdlfaultPointQList()
{
    QList<Mdl_Point> fp;
    for (int i = 0; i < faultpoint.size(); i++)
    {
        fp.append(faultpoint[i].toMdlPoint());
    }
    return fp;
}

std::vector<Mdl_Point> lzbFault::toMdlfaultPointVector()
{
    std::vector<Mdl_Point> fp;
    for (int i = 0; i < faultpoint.size(); i++)
    {
        fp.push_back(faultpoint[i].toMdlPoint());
    }
    return fp;
}

QList<Mdl_Triangle> lzbFault::toMdlfaultfaceQlist()
{
    QList<Mdl_Triangle> mdlface;
    for (int i = 0; i < faultface.size(); i++)
    {
        mdlface.append(faultface[i].toMdlTri());
    }
    return mdlface;
}

void lzbFault::setFaultP(QList<Mdl_Point> mdlp)
{
    for (int i = 0; i < mdlp.size(); i++)
    {
        faultpoint.append(lzbPoint(mdlp[i]));
    }
}

void lzbFault::setFaultFace(QList<Mdl_Triangle> faulttri)
{
    for (int i = 0; i < faulttri.size(); i++)
    {
        faultface.append(lzbTriangle(faulttri[i]));
    }
}

void lzbFault::setNorR(QList<bool> tempNR)
{
    NorR = tempNR;
}

void lzbFault::setfalutDistance(QList<double> tempFD)
{
    faultD = tempFD;
}

lzbTriangle::lzbTriangle(Mdl_Triangle tri)
{
    num = tri.triNum;
    p1 = lzbPoint(tri.p1);
    p2 = lzbPoint(tri.p2);
    p3 = lzbPoint(tri.p3);
}

Mdl_Triangle lzbTriangle::toMdlTri()
{
    Mdl_Triangle mdltri;
    mdltri.triNum = num;
    mdltri.p1 = p1.toMdlPoint();
    mdltri.p2 = p2.toMdlPoint();
    mdltri.p3 = p3.toMdlPoint();
    return mdltri;
}

double lzbTriangle::caculateArea()
{
    // 三个点的坐标
    QVector3D A(p1.x, p1.y, p1.z);
    QVector3D B(p2.x, p2.y, p2.z);
    QVector3D C(p3.x, p3.y, p3.z);

    // 计算向量 AB 和 AC
    QVector3D AB = B - A;
    QVector3D AC = C - A;

    // 计算叉乘得到法向量
    QVector3D crossProduct = QVector3D::crossProduct(AB, AC);

    // 计算三角形的面积（法向量的长度）
    double area = 0.5 * crossProduct.length();

    return area;
}

lzbPLine::lzbPLine(QList<Mdl_Point> plist)
{
    for (int i = 0; i < plist.size(); i++)
        PList.append(lzbPoint(plist[i]));
    if (PList.first() == PList.last())
        isclose = true;
}

lzbPLine::lzbPLine(QList<lzbPoint> plist)
{
    for (int i = 0; i < plist.size(); i++)
        PList.append(plist[i]);
    if (PList.first() == PList.last())
        isclose = true;
}


lzbPLine::lzbPLine(Mdl_Poly pl)
{
    i = pl.polyNum;
    for (Mdl_Point p : pl.pList)
    {
        PList.append(p);
    }
    isclose = pl.closeFlag;
}

QList<Mdl_Point> lzbPLine::toMdl_PList()
{
    QList<Mdl_Point> plist;
    for (int i = 0; i < PList.size(); i++)
    {
        plist.append(PList[i].toMdlPoint());
    }

    return plist;
}

Mdl_Poly lzbPLine::toMdl_Poly()
{
    Mdl_Poly poly;
    poly.closeFlag = isclose;
    for (int i = 0; i < PList.size(); i++)
        poly.pList.push_back(PList[i].toMdlPoint());

    return poly;
}

void lzbPLine::RemoveRepeat()
{
    for (int i = 1; i < PList.size(); i++)
    {
        for (int j = 0; j < i; j++)
        {
            if (PList[i] == PList[j])
            {
                PList.removeAt(i);
                i--;
                break;
            }
        }
    }
}

void lzbPLine::Reserve()
{
    QList<lzbPoint> tempP;
    for (int i = PList.size() - 1; i >= 0; i--)
    {
        tempP.append(PList[i]);
    }
    PList = tempP;
}

bool lzbPLine::isClockwise()
{
    QPolygonF poly1;
    for (int i = 0; i < PList.size(); i++)
    {
        poly1 << QPoint(PList[i].x, PList[i].y);
    }

    qreal sum = 0.0;
    int n = poly1.size();

    for (int i = 0; i < n; i++) {
        QPointF currentPoint = poly1.at(i);
        QPointF nextPoint = poly1.at((i + 1) % n);

        sum += (nextPoint.x() - currentPoint.x()) * (nextPoint.y() + currentPoint.y());
    }

    return sum > 0.0;
}

double lzbPLine::CaculatePolyArea()
{
    QList<lzbPoint> plist = PList;
    if (plist.first() == plist.last())
        plist.pop_back();

    double area = 0.0;
    for (int i = 0; i < plist.size(); ++i) 
    {
        int j = (i + 1) % plist.size();
        area += plist[i].x * plist[j].y;
        area -= plist[j].x * plist[i].y;
    }
    area /= 2.0;
    return abs(area);
}


void lzbSectionmdl::setGeoCut(Mdl_CutSection cut)
{
    int num = cut.cutFaceList.size();
    for (int i = 0; i < num; i++)
    {
        cutFace.append(cut.cutFaceList[i].bodyFace);
    }
}
