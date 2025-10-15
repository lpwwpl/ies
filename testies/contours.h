#ifndef MARCHINGSQUARES_STD_H
#define MARCHINGSQUARES_STD_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>

using namespace std;
#include <QPolygonF>


class MarchingSquares
{
public:

    class levelPaths : public  vector<QPolygonF> {
    public:

        void openPoligon(double x, double y);
        void lineTo(double x, double y);
        void closePoligon();

    private:
        QPolygonF _current;

    };

    MarchingSquares(vector<vector<double> >& data, double xMin, double xMax, double yMin, double yMax);

    vector<levelPaths> mkIsos(vector<double>& levels);

    static constexpr double epsilon = 1e-10;

    ~MarchingSquares() { delete dataP; }

private:
    class IsoCell {
    public:
        enum side {
            LEFT, RIGHT, TOP, BOTTOM, NONE
        };
        IsoCell();
        IsoCell(int nInfo, bool isFlipped);
        void setFlipped(bool isFlipped);
        void setNeighborInfo(int nInfo);
        int getNeighborInfo();
        bool isFlipped();
        QPointF normalizedPointCCW(side cellSide);
        side firstSideCCW(side prev);
        side secondSideCCW(side prev);
        side nextCellCCW(side prev);
        void clearIso();
        double getLeft();
        void setLeft(double left);
        double getRight();
        void setRight(double right);
        double getTop();
        void setTop(double top);
        double getBottom();
        void setBottom(double bottom);

    private:
        bool _flipped;
        int  _neighborInfo;
        double _left, _right, _top, _bottom;


    };




    /**
     * Coordinates within this distance of each other are considered identical.
     * This affects whether new segments are or are not created in an iso
     * shape GeneralPath, in particular whether or not to generate a call
     * to lineTo().
     */
    double* padData(vector<vector<double> >& data);
    IsoCell* mkContour(double& level, int& numRows, int& numCols);
    levelPaths mkIso(IsoCell* isoData, double threshold, int& numRows, int& numCols);
    void interpolateCrossing(IsoCell* isoData, int r, int c, double threshold, int rows, int cols);
    void isoSubpath(IsoCell* isoData, int r, int c, levelPaths& iso, int& numRows, int& numCols);


    int p_rows, p_cols;
    double* dataP;
    double _xMin, _xMax, _yMin, _yMax;
    double _xStep, _yStep;
    int _ySize, _xSize;


};
#endif // MARCHINGSQUARES_STD_H