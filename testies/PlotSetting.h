#ifndef PLOTSETTING_H
#define PLOTSETTING_H

#include <QColor>
#include <QFont>
#include <QVector>
#include <qwt_symbol.h>
#include <qwt_plot.h>

class QwtPlotCurve;

enum FillPointType
{
    eSmallEllipse = 0,
    eXCross,
    eCross,
    eEllipse,
    eFillEllipse,
    eRect,
    eFillRect,
    eDTriangle,
    eFillDTriangle,
    eDiamond,
    eFillDiamond,
    eStart1,
    eFillStart1,
};

// 线条样式结构体（每条线独立）
struct SLineStyle {
    bool visible = true;
    bool legendVisible = true;
    double lineWidth = 2;
    QColor lineColor = Qt::black;
    Qt::PenStyle lineStyle = Qt::SolidLine;
    QVector<qreal> customDashPattern;   // 自定义虚线模式
    bool useCurve = false;               // true=曲线, false=折线

    QwtSymbol::Style pointStyle = QwtSymbol::Ellipse;
    FillPointType fillPointStyle;
    double pointSize = 5;
    QColor pointColor = Qt::black;
    bool pointFilled = false;
    double pointWidth = 1;                  // 点边框宽度

};
struct MTFPlotItem
{
    int index = 0;
    int m_group = 0;
    QString label;
    double m_x, m_y;
    double m_angle;
    double m_length;
    QColor m_color;
    double m_width;
    bool visible = true;
    QwtPlotItem* m_item;
};
struct MTFLine {
    int index = 0;
    int m_group = 0;
    QString label;
    QString colorName;          // 原始颜色名称（来自文件）
    bool selected = false;
    QVector<QPointF> data;
    QwtPlotCurve* curve = nullptr;
    SLineStyle m_style;

    void setPointFillValue()
    {
        switch (m_style.fillPointStyle)
        {
        case eSmallEllipse:
        {
            m_style.pointStyle = QwtSymbol::Ellipse;
            m_style.pointFilled = true;
        }
        break;
        case eXCross:
        {
            m_style.pointStyle = QwtSymbol::XCross;
            m_style.pointFilled = false;
        }
        break;
        case eCross:
        {
            m_style.pointStyle = QwtSymbol::Cross;
            m_style.pointFilled = false;
        }
        break;
        case eEllipse:
        {
            m_style.pointStyle = QwtSymbol::Ellipse;
            m_style.pointFilled = false;
        }
        break;
        case eFillEllipse:
        {
            m_style.pointStyle = QwtSymbol::Ellipse;
            m_style.pointFilled = true;
        }
        break;
        case eRect:
        {
            m_style.pointStyle = QwtSymbol::Rect;
            m_style.pointFilled = false;
        }
        break;
        case eFillRect:
        {
            m_style.pointStyle = QwtSymbol::Rect;
            m_style.pointFilled = true;
        }
        break;
        case eDTriangle:
        {
            m_style.pointStyle = QwtSymbol::DTriangle;
            m_style.pointFilled = false;
        }
        break;
        case eFillDTriangle:
        {
            m_style.pointStyle = QwtSymbol::DTriangle;
            m_style.pointFilled = true;
        }
        break;
        case eDiamond:
        {
            m_style.pointStyle = QwtSymbol::Diamond;
            m_style.pointFilled = false;
        }
        break;
        case eFillDiamond:
        {
            m_style.pointStyle = QwtSymbol::Diamond;
            m_style.pointFilled = true;
        }
        break;
        case eStart1:
        {
            m_style.pointStyle = QwtSymbol::Star1;
            m_style.pointFilled = false;
        }
        break;
        case eFillStart1:
        {
            m_style.pointStyle = QwtSymbol::Star1;
            m_style.pointFilled = true;
        }
        break;
        default:
            break;
        }
    }
};
// 全局绘图设置
struct PlotSettings {

    PlotSettings() {}
    PlotSettings operator=(const PlotSettings& other)
    {
        backgroundColor = other.backgroundColor;
        xAxis = other.xAxis;
        yAxis = other.yAxis;
        gridVisible = other.gridVisible;
        gridMajorColor = other.gridMajorColor;
        gridMinorColor = other.gridMinorColor;
        gridMajorStyle = other.gridMajorStyle;
        gridMinorStyle = other.gridMinorStyle;
        legend = other.legend;
        titleFont = other.titleFont;
        titleColor = other.titleColor;
        origin = other.origin;
        QList<int> line_keys = other.m_lines.keys();
        for (int i = 0; i < line_keys.size(); i++)
        {
            if (m_lines.contains(line_keys[i]))
            {
                m_lines[line_keys[i]].m_style = other.m_lines[line_keys[i]].m_style;
                m_lines[line_keys[i]].setPointFillValue();
                m_lines[line_keys[i]].selected = other.m_lines[line_keys[i]].selected;
            }
        }
        QList<int> item_keys = other.m_items.keys();
        for (int i = 0; i < item_keys.size(); i++)
        {
            if (m_lines.contains(item_keys[i]))
            {
                m_items[line_keys[i]].m_x = other.m_items[line_keys[i]].m_x;
                m_items[line_keys[i]].m_y = other.m_items[line_keys[i]].m_y;
                m_items[line_keys[i]].m_length = other.m_items[line_keys[i]].m_length;
                m_items[line_keys[i]].visible = other.m_items[line_keys[i]].visible;
                m_items[line_keys[i]].m_color = other.m_items[line_keys[i]].m_color;
                m_items[line_keys[i]].m_width = other.m_items[line_keys[i]].m_width;
            }
        }
        return *this;
    }
    PlotSettings(const PlotSettings& other)
    {
        backgroundColor = other.backgroundColor;
        xAxis = other.xAxis;
        yAxis = other.yAxis;
        gridVisible = other.gridVisible;
        gridMajorColor = other.gridMajorColor;
        gridMinorColor = other.gridMinorColor;
        gridMajorStyle = other.gridMajorStyle;
        gridMinorStyle = other.gridMinorStyle;
        legend = other.legend;
        titleFont = other.titleFont;
        titleColor = other.titleColor;
        origin = other.origin;
        QList<int> line_keys = other.m_lines.keys();
        for (int i=0;i< line_keys.size();i++)
        {
            if (m_lines.contains(line_keys[i]))
            {
                m_lines[line_keys[i]].m_style = other.m_lines[line_keys[i]].m_style;
                m_lines[line_keys[i]].selected = other.m_lines[line_keys[i]].selected;
            }  
        }
        QList<int> item_keys = other.m_items.keys();
        for (int i = 0; i < item_keys.size(); i++)
        {
            if (m_lines.contains(item_keys[i]))
            {
                m_items[line_keys[i]].m_x = other.m_items[line_keys[i]].m_x;
                m_items[line_keys[i]].m_y = other.m_items[line_keys[i]].m_y;
                m_items[line_keys[i]].m_length = other.m_items[line_keys[i]].m_length;
                m_items[line_keys[i]].visible = other.m_items[line_keys[i]].visible;
                m_items[line_keys[i]].m_color = other.m_items[line_keys[i]].m_color;
                m_items[line_keys[i]].m_width = other.m_items[line_keys[i]].m_width;
            }
        }
    }
    // 背景
    QColor backgroundColor = Qt::white;

    // 坐标轴
    struct AxisSettings {
        bool visible = true;
        QString title;
        QFont titleFont;
        QColor titleColor = Qt::black;
        QFont tickFont;
        QColor tickColor = Qt::black;
        double min = 0.0;
        double max = 1.0;
        double step = 0.1;
        bool autoRange = true;
        bool logScale = false;
    } xAxis, yAxis;

    // 网格
    bool gridVisible = true;
    QColor gridMajorColor = Qt::lightGray;
    QColor gridMinorColor = Qt::lightGray;
    Qt::PenStyle gridMajorStyle = Qt::SolidLine;
    Qt::PenStyle gridMinorStyle = Qt::DotLine;

    // 图例
    struct LegendSettings {
        bool visible = true;
        QwtPlot::LegendPosition position = QwtPlot::RightLegend;
        QFont font;
        QColor color = Qt::black;
    } legend;

    // 标题
    QString title;
    QFont titleFont;
    QColor titleColor = Qt::black;

    // 原点
    enum Origin { BottomLeft, BottomRight, Center };
    Origin origin = BottomLeft;

    bool m_bCurveGroupEditable = false;

    bool m_bItemGroupEditable = false;
    //bool m_bSettingGroup = true;
    QHash<int, MTFLine> m_lines;

    QHash<int, MTFPlotItem> m_items;
};

#endif // PLOTSETTING_H