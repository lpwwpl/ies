#include "SimplePropertyBrowser.h"
#include <QDialog>
#include <QTabWidget>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QColorDialog>
#include <QFontDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QDialogButtonBox>


#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_plot_renderer.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_legend_label.h>
#include <qwt_plot_grid.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
SimplePropertyBrowser::SimplePropertyBrowser(PlotSettings* settings, QwtPlot* plot, QwtPlotGrid* grid, QwtLegend* legend, QWidget* parent)
    :QWidget(parent),m_settings(settings),m_plot(plot),m_grid(grid),m_legend(legend)
{
    initSimplePropertyWidget();
}
SimplePropertyBrowser::~SimplePropertyBrowser()
{

}

void setCurrentCurve(QwtPlotCurve* curve)
{

}
// 连接信号：选择线条变化时，加载该线条的当前设置到控件
void SimplePropertyBrowser::loadLineSettings(int index) {
    if (index < 0 || index >= m_settings->m_lines.size()) return;
    const MTFLine& line = m_settings->m_lines[index];

    lineWidthSpin->setValue(line.m_style.lineWidth);
    // 设置线颜色
    lineColorLabel->setPalette(QPalette(line.m_style.lineColor));

    lineStyleCombo->blockSignals(true);
    lineStyleCombo->setCurrentIndex(line.m_style.lineStyle);
    lineStyleCombo->blockSignals(false);

    lineCurveCombo->blockSignals(true);
    lineCurveCombo->setCurrentIndex(line.m_style.useCurve);
    lineCurveCombo->blockSignals(false);
    // 点样式
    pointStyleCombo->blockSignals(true);    
    pointStyleCombo->setCurrentIndex(line.m_style.fillPointStyle);
    pointStyleCombo->blockSignals(false);
    // 点填充
    //pointFilledCheck->setChecked(line.pointFilled);
    // 点大小
    pointSizeSpin->setValue(line.m_style.pointSize);
    // 点颜色
    pointColorLabel->setPalette(QPalette(line.m_style.pointColor));
    m_plot->replot();
};

void SimplePropertyBrowser::initSimplePropertyWidget()
{
    setWindowTitle(tr("绘图设置"));
    QTabWidget* tabWidget = new QTabWidget(this);

    ////////////////////////////////////////////////////////////////////////////////////
    // 全局标签页
    globalTab = new QWidget();
    QFormLayout* globalLayout = new QFormLayout(globalTab);

    // 背景颜色
    QPushButton* bgColorBtn = new QPushButton(tr("选择颜色"));
    bgColorLabel = new QLabel();
    bgColorLabel->setAutoFillBackground(true);
    bgColorLabel->setPalette(QPalette(m_settings->backgroundColor));
    connect(bgColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->backgroundColor, this);
        if (c.isValid()) {
            m_settings->backgroundColor = c;
            bgColorLabel->setPalette(QPalette(c));
            m_plot->setCanvasBackground(m_settings->backgroundColor);
            m_plot->replot();
        }
        });
    QHBoxLayout* bgLayout = new QHBoxLayout();
    bgLayout->addWidget(bgColorLabel);
    bgLayout->addWidget(bgColorBtn);
    globalLayout->addRow(tr("背景颜色:"), bgLayout);

    // 显示网格
    gridVisibleCheck = new QCheckBox();
    gridVisibleCheck->setChecked(m_settings->gridVisible);
    globalLayout->addRow(tr("显示网格:"), gridVisibleCheck);
    connect(gridVisibleCheck, &QCheckBox::toggled, this, [this](bool) {
        m_settings->gridVisible = gridVisibleCheck->isChecked();
        applyGridSettings();
        });

    // 原点位置
    originCombo = new QComboBox();
    originCombo->addItem(tr("左下角"), static_cast<int>(PlotSettings::BottomLeft));
    originCombo->addItem(tr("右下角"), static_cast<int>(PlotSettings::BottomRight));
    originCombo->addItem(tr("中心"), static_cast<int>(PlotSettings::Center));
    int originIdx = originCombo->findData(static_cast<int>(m_settings->origin));
    if (originIdx >= 0) originCombo->setCurrentIndex(originIdx);
    globalLayout->addRow(tr("原点位置:"), originCombo);
    connect(originCombo, &QComboBox::currentTextChanged, this, [this](const QString&) {
        m_settings->origin = static_cast<PlotSettings::Origin>(originCombo->currentData().toInt());
        applyOrigin();
        signalUpdateScaleDiv();
        //applyXAxisSettings();
        //applyYAxisSettings();
        });


    // 标题
    titleEdit = new QLineEdit(m_settings->title);
    globalLayout->addRow(tr("标题:"), titleEdit);
    connect(titleEdit, &QLineEdit::textChanged, this, [this](const QString&) {
        m_settings->title = titleEdit->text();
        applyTitleSettings();
        });


    QPushButton* titleFontBtn = new QPushButton(tr("选择字体"));
    titleFontLabel = new QLabel(m_settings->titleFont.family());
    titleFontLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    titleFontLabel->adjustSize();
    titleFontLabel->setWordWrap(true);
    connect(titleFontBtn, &QPushButton::clicked, [this]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, m_settings->titleFont, this);
        if (ok) {
            m_settings->titleFont = font;
            applyTitleSettings();
        }
        });
    QHBoxLayout* titleFontLayout = new QHBoxLayout();
    titleFontLayout->addWidget(titleFontLabel);
    titleFontLayout->addWidget(titleFontBtn);
    globalLayout->addRow(tr("标题字体:"), titleFontLayout);
    QPushButton* titleColorBtn = new QPushButton(tr("选择颜色"));
    titleColorLabel = new QLabel();
    titleColorLabel->setAutoFillBackground(true);
    titleColorLabel->setPalette(QPalette(m_settings->titleColor));
    connect(titleColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->titleColor, this);
        if (c.isValid()) {
            m_settings->titleColor = c;
            applyTitleSettings();
        }
        });

    // 图例
    legendVisibleCheck = new QCheckBox();
    legendVisibleCheck->setChecked(m_settings->legend.visible);
    globalLayout->addRow(tr("显示图例:"), legendVisibleCheck);
    connect(legendVisibleCheck, &QCheckBox::toggled, this, [this](bool) {
        m_settings->legend.visible = legendVisibleCheck->isChecked();
        if (m_settings->legend.visible)
        {
            // 显示图例：如果之前移除了，需要重新插入；如果已存在，直接显示并更新样式
            if (!m_legend) {
                m_legend = new QwtLegend();
                // 可能需要对图例进行一些初始化设置
            }
            m_plot->insertLegend(m_legend, m_settings->legend.position);

            m_legend->setVisible(true);
            updateLegendItemsStyle();
        }
        else
        {
            // 隐藏图例：从 Plot 中移除，完全消失
            delete m_legend;
            m_plot->insertLegend(nullptr);
            m_legend = NULL;
            // 如果 m_legend 是你管理的成员，可以保留对象以便下次快速显示，
            // 或者直接删除并置空（取决于你的内存管理策略）
            // 这里假设保留 m_legend 对象，仅从 plot 中移除
        }
        });

    // 图例位置
    legendPosCombo = new QComboBox();
    legendPosCombo->addItem("左", (QwtPlot::LeftLegend));
    legendPosCombo->addItem("右", (QwtPlot::RightLegend));
    legendPosCombo->addItem("下", (QwtPlot::BottomLegend));
    legendPosCombo->addItem("上", (QwtPlot::TopLegend));
    // 根据当前设置选择
    int idx = legendPosCombo->findData(m_settings->legend.position);
    if (idx >= 0) legendPosCombo->setCurrentIndex(idx);
    globalLayout->addRow(tr("图例位置:"), legendPosCombo);
    connect(legendPosCombo, &QComboBox::currentTextChanged, this, [this](const QString&) {
        m_settings->legend.position = static_cast<QwtPlot::LegendPosition>(legendPosCombo->currentData().toInt());
        applyLegendSettings();
        });

    // 图例字体
    QPushButton* legendFontBtn = new QPushButton(tr("选择字体"));
    legendFontLabel = new QLabel(m_settings->legend.font.family());
    legendFontLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    legendFontLabel->adjustSize();
    legendFontLabel->setWordWrap(true);
    connect(legendFontBtn, &QPushButton::clicked, [this]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, m_settings->legend.font, this);
        if (ok) {
            m_settings->legend.font = font;
            applyLegendSettings();
        }
        });
    QHBoxLayout* legendFontLayout = new QHBoxLayout();
    legendFontLayout->addWidget(legendFontLabel);
    legendFontLayout->addWidget(legendFontBtn);
    globalLayout->addRow(tr("图例字体:"), legendFontLayout);

    // 图例颜色
    QPushButton* legendColorBtn = new QPushButton(tr("选择颜色"));
    legendColorLabel = new QLabel();
    legendColorLabel->setAutoFillBackground(true);
    legendColorLabel->setPalette(QPalette(m_settings->legend.color));
    connect(legendColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->legend.color, this);
        if (c.isValid()) {
            m_settings->legend.color = c;
            applyLegendSettings();
        }
        });
    QHBoxLayout* legendColorLayout = new QHBoxLayout();
    legendColorLayout->addWidget(legendColorLabel);
    legendColorLayout->addWidget(legendColorBtn);
    globalLayout->addRow(tr("图例文字颜色:"), legendColorLayout);




    /////////////////////////////////////////////////////////////////////////////////////////
    // 坐标轴标签页
    axisTab = new QWidget();
    QFormLayout* axisLayout = new QFormLayout(axisTab);

    // X轴
    QGroupBox* xGroup = new QGroupBox("X轴");
    QFormLayout* xLayout = new QFormLayout(xGroup);
    xVisible = new QCheckBox();
    xVisible->setChecked(m_settings->xAxis.visible);
    xLayout->addRow(tr("显示:"), xVisible);
    connect(xVisible, &QCheckBox::toggled, this, [this](bool) {
        m_settings->xAxis.visible = xVisible->isChecked();
        applyXAxisSettings();
        });

    xTitle = new QLineEdit(m_settings->xAxis.title);
    xLayout->addRow(tr("标题:"), xTitle);
    connect(xTitle, &QLineEdit::textChanged, this, [this](const QString&) {
        m_settings->xAxis.title = xTitle->text();
        applyXAxisSettings();
        });

    xAutoRange = new QCheckBox();
    connect(xAutoRange, &QCheckBox::toggled, this, [this](bool) {
        m_settings->xAxis.autoRange = xAutoRange->isChecked();
            xMin->setEnabled(!xAutoRange->isChecked());
            xMax->setEnabled(!xAutoRange->isChecked());
            xStep->setEnabled(!xAutoRange->isChecked());
        applyXAxisSettings();
        });

    xLayout->addRow(tr("自动范围:"), xAutoRange);
    xMin = new QDoubleSpinBox();
    xMin->setRange(-1e6, 1e6);
    //xMin->setValue(m_settings->xAxis.min);
    //xMin->setEnabled(!m_settings->xAxis.autoRange);
    xLayout->addRow(tr("最小值:"), xMin);
    connect(xMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
        m_settings->xAxis.min = xMin->value();
        applyXAxisSettings();
        });

    xMax = new QDoubleSpinBox();
    xMax->setRange(-1e6, 1e6);
    //xMax->setValue(m_settings->xAxis.max);
    //xMax->setEnabled(!m_settings->xAxis.autoRange);
    xLayout->addRow(tr("最大值:"), xMax);
    connect(xMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
        m_settings->xAxis.max = xMax->value();
        applyXAxisSettings();
        });

    xStep = new QDoubleSpinBox();
    xStep->setRange(0, 1e6);
    xStep->setValue(m_settings->xAxis.step);
    xLayout->addRow(tr("步长:"), xStep);
    axisLayout->addRow(xGroup);
    connect(xStep, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, ([this](double) {
        m_settings->xAxis.step = xStep->value();
        applyXAxisSettings();
        }));

    // 标题字体
    QPushButton* xTitleFontBtn = new QPushButton(tr("选择字体"));
    xTitleFontLabel = new QLabel(m_settings->xAxis.titleFont.family());
    xTitleFontLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    xTitleFontLabel->adjustSize();
    xTitleFontLabel->setWordWrap(true);
    connect(xTitleFontBtn, &QPushButton::clicked, [this]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, m_settings->xAxis.titleFont, this);
        if (ok) {
            m_settings->xAxis.titleFont = font;
            applyXAxisSettings();
        }
        });
    QHBoxLayout* xTitleFontLayout = new QHBoxLayout();
    xTitleFontLayout->addWidget(xTitleFontLabel);
    xTitleFontLayout->addWidget(xTitleFontBtn);
    xLayout->addRow(tr("标题字体:"), xTitleFontLayout);

    // 标题颜色
    QPushButton* xTitleColorBtn = new QPushButton(tr("选择颜色"));
    xTitleColorLabel = new QLabel();
    xTitleColorLabel->setAutoFillBackground(true);
    xTitleColorLabel->setPalette(QPalette(m_settings->xAxis.titleColor));
    connect(xTitleColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->xAxis.titleColor, this);
        if (c.isValid()) {
            m_settings->xAxis.titleColor = c;
            applyXAxisSettings();
        }
        });
    QHBoxLayout* xTitleColorLayout = new QHBoxLayout();
    xTitleColorLayout->addWidget(xTitleColorLabel);
    xTitleColorLayout->addWidget(xTitleColorBtn);
    xLayout->addRow(tr("标题颜色:"), xTitleColorLayout);
    // 刻度标签字体
    QPushButton* xTickFontBtn = new QPushButton(tr("选择字体"));
    xTickFontLabel = new QLabel(m_settings->xAxis.tickFont.family());
    xTickFontLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    xTickFontLabel->adjustSize();
    xTickFontLabel->setWordWrap(true);

    connect(xTickFontBtn, &QPushButton::clicked, [this]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, m_settings->xAxis.tickFont, this);
        if (ok) {
            m_settings->xAxis.tickFont = font;
            applyXAxisSettings();
        }
        });
    QHBoxLayout* xTickFontLayout = new QHBoxLayout();
    xTickFontLayout->addWidget(xTickFontLabel);
    xTickFontLayout->addWidget(xTickFontBtn);
    xLayout->addRow(tr("刻度字体:"), xTickFontLayout);

    // 刻度标签颜色
    QPushButton* xTickColorBtn = new QPushButton(tr("选择颜色"));
    xTickColorLabel = new QLabel();
    xTickColorLabel->setAutoFillBackground(true);
    xTickColorLabel->setPalette(QPalette(m_settings->xAxis.tickColor));
    connect(xTickColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->xAxis.tickColor, this);
        if (c.isValid()) {
            m_settings->xAxis.tickColor = c;
            applyXAxisSettings();
        }
        });
    QHBoxLayout* xTickColorLayout = new QHBoxLayout();
    xTickColorLayout->addWidget(xTickColorLabel);
    xTickColorLayout->addWidget(xTickColorBtn);
    xLayout->addRow(tr("刻度颜色:"), xTickColorLayout);

    // Y轴类似（省略，实际应添加）
    QGroupBox* yGroup = new QGroupBox("Y轴");
    QFormLayout* yLayout = new QFormLayout(yGroup);
    yVisible = new QCheckBox();
    yVisible->setChecked(m_settings->yAxis.visible);
    yLayout->addRow(tr("显示:"), yVisible);
    connect(yVisible, &QCheckBox::toggled, this, [this](bool) {
        m_settings->yAxis.visible = yVisible->isChecked();
        applyYAxisSettings();
        });

    yTitle = new QLineEdit(m_settings->yAxis.title);
    yLayout->addRow(tr("标题:"), yTitle);
    connect(yTitle, &QLineEdit::textChanged, this, [this](const QString&) {
        m_settings->yAxis.title = yTitle->text();
        applyYAxisSettings();
        });
    yAutoRange = new QCheckBox();
    yLayout->addRow(tr("自动范围:"), yAutoRange);
    connect(yAutoRange, &QCheckBox::toggled, this, [this](bool) {
        m_settings->yAxis.autoRange = yAutoRange->isChecked();
        yMin->setEnabled(!yAutoRange->isChecked());
        yMax->setEnabled(!yAutoRange->isChecked());
        yStep->setEnabled(!yAutoRange->isChecked());
        applyYAxisSettings();
        });
    yMin = new QDoubleSpinBox();
    yMin->setRange(-1e6, 1e6);
    yMin->setValue(m_settings->yAxis.min);
    yMin->setEnabled(!m_settings->yAxis.autoRange);
    yLayout->addRow(tr("最小值:"), yMin);
    connect(yMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, ([this](double) {
        m_settings->yAxis.min = yMin->value();
        applyYAxisSettings();
        }));
    yMax = new QDoubleSpinBox();
    yMax->setRange(-1e6, 1e6);
    yMax->setValue(m_settings->yAxis.max);
    yMax->setEnabled(!m_settings->yAxis.autoRange);
    yLayout->addRow(tr("最大值:"), yMax);
    connect(yMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, ([this](double) {
        m_settings->yAxis.max = yMax->value();
        applyYAxisSettings();
        }));
    yStep = new QDoubleSpinBox();
    yStep->setRange(0, 1e6);
    yStep->setValue(m_settings->yAxis.step);
    yLayout->addRow(tr("步长:"), yStep);
    axisLayout->addRow(yGroup);
    connect(yStep, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, ([this](double) {
        m_settings->yAxis.step = yStep->value();
        applyYAxisSettings();
        }));

    // 标题字体
    QPushButton* yTitleFontBtn = new QPushButton(tr("选择字体"));
    yTitleFontLabel = new QLabel(m_settings->yAxis.titleFont.family());
    yTitleFontLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    yTitleFontLabel->adjustSize();
    yTitleFontLabel->setWordWrap(true);
    connect(yTitleFontBtn, &QPushButton::clicked, [this]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, m_settings->yAxis.titleFont, this);
        if (ok) {
            m_settings->yAxis.titleFont = font;
            applyYAxisSettings();
        }
        });
    QHBoxLayout* yTitleFontLayout = new QHBoxLayout();
    yTitleFontLayout->addWidget(yTitleFontLabel);
    yTitleFontLayout->addWidget(yTitleFontBtn);
    yLayout->addRow(tr("标题字体:"), yTitleFontLayout);

    // 标题颜色
    QPushButton* yTitleColorBtn = new QPushButton(tr("选择颜色"));
    yTitleColorLabel = new QLabel();
    yTitleColorLabel->setAutoFillBackground(true);
    yTitleColorLabel->setPalette(QPalette(m_settings->yAxis.titleColor));

    connect(yTitleColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->yAxis.titleColor, this);
        if (c.isValid()) {
            m_settings->yAxis.titleColor = c;
            applyYAxisSettings();
        }
        });
    QHBoxLayout* yTitleColorLayout = new QHBoxLayout();
    yTitleColorLayout->addWidget(yTitleColorLabel);
    yTitleColorLayout->addWidget(yTitleColorBtn);
    yLayout->addRow(tr("标题颜色:"), yTitleColorLayout);

    // 刻度标签字体
    QPushButton* yTickFontBtn = new QPushButton(tr("选择字体"));
    yTickFontLabel = new QLabel(m_settings->yAxis.tickFont.family());
    yTickFontLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    yTickFontLabel->adjustSize();
    yTickFontLabel->setWordWrap(true);
    connect(yTickFontBtn, &QPushButton::clicked, [this]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, m_settings->yAxis.tickFont, this);
        if (ok) {
            m_settings->yAxis.tickFont = font;
            applyYAxisSettings();
        }
        });
    QHBoxLayout* yTickFontLayout = new QHBoxLayout();
    yTickFontLayout->addWidget(yTickFontLabel);
    yTickFontLayout->addWidget(yTickFontBtn);
    yLayout->addRow(tr("刻度字体:"), yTickFontLayout);

    // 刻度标签颜色
    QPushButton* yTickColorBtn = new QPushButton(tr("选择颜色"));
    yTickColorLabel = new QLabel();
    yTickColorLabel->setAutoFillBackground(true);
    yTickColorLabel->setPalette(QPalette(m_settings->yAxis.tickColor));
    connect(yTickColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->yAxis.tickColor, this);
        if (c.isValid()) {
            m_settings->yAxis.tickColor = c;
            applyYAxisSettings();
        }
        });
    QHBoxLayout* yTickColorLayout = new QHBoxLayout();
    yTickColorLayout->addWidget(yTickColorLabel);
    yTickColorLayout->addWidget(yTickColorBtn);
    yLayout->addRow(tr("刻度颜色:"), yTickColorLayout);



    /////////////////////////////////////////////////////////////////////////////
    // 曲线样式标签页（可对每条线单独设置，这里简单给出全局默认）
    curveTab = new QWidget();
    QFormLayout* curveLayout = new QFormLayout(curveTab);
    // 线选择组合框
    lineCombo = new QComboBox();

    curveLayout->addWidget(lineCombo);
    // 创建一个组框，包含所有线属性控件
    QGroupBox* propGroup = new QGroupBox(tr("线条属性"));
    QFormLayout* propLayout = new QFormLayout(propGroup);
    lineStyleCombo = new QComboBox();
    lineStyleCombo->addItem("无", static_cast<int>(Qt::NoPen));
    lineStyleCombo->addItem("实线", static_cast<int>(Qt::SolidLine));
    lineStyleCombo->addItem("虚线", static_cast<int>(Qt::DashLine));
    lineStyleCombo->addItem("点线", static_cast<int>(Qt::DotLine));
    lineStyleCombo->addItem("点虚线", static_cast<int>(Qt::DashDotLine));
    lineStyleCombo->addItem("点点虚线", static_cast<int>(Qt::DashDotDotLine));
    lineStyleCombo->addItem("自定义", static_cast<int>(Qt::CustomDashLine));
    lineStyleCombo->addItem("点", static_cast<int>(Qt::CustomDashLine + 1));

    // 自定义虚线模式
        // 辅助函数：将用户输入的虚线模式字符串转为 QVector<qreal>

    customDashEdit = new QLineEdit();
    customDashEdit->setPlaceholderText("例如: 5 2 1 2");
    customDashEdit->setEnabled(false);
    propLayout->addRow(tr("自定义模式:"), customDashEdit);
   
    connect(customDashEdit, QOverload<const QString&>::of(&QLineEdit::textChanged), this, [this](const QString& ) {
        QVector<qreal> pattern = parseDashPattern(customDashEdit->text());
        m_settings->m_lines[lineCombo->currentIndex()].m_style.customDashPattern = pattern;       
        updateCurveStyle(lineCombo->currentIndex());
        });


    propLayout->addRow(tr("默认线型:"), lineStyleCombo);
    connect(lineStyleCombo, &QComboBox::currentTextChanged, this, [this](const QString&) {
        Qt::PenStyle style = static_cast<Qt::PenStyle>(lineStyleCombo->currentData().toInt());;
        if (style == Qt::CustomDashLine)
        {
            customDashEdit->setEnabled(true);
        }
        else
        {
            customDashEdit->setEnabled(false);
        }
        m_settings->m_lines[lineCombo->currentIndex()].m_style.lineStyle = style;
        updateCurveStyle(lineCombo->currentIndex());
        });

    lineWidthSpin = new QDoubleSpinBox();
    lineWidthSpin->setRange(1, 200);
    lineWidthSpin->setValue(1);
    lineWidthSpin->setSingleStep(0.1);
    propLayout->addRow(tr("默认线宽:"), lineWidthSpin);
    connect(lineWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
        m_settings->m_lines[lineCombo->currentIndex()].m_style.lineWidth = lineWidthSpin->value();
        updateCurveStyle(lineCombo->currentIndex());
        });

    // 线颜色
    QPushButton* lineColorBtn = new QPushButton(tr("选择颜色"));
    lineColorLabel = new QLabel();
    lineColorLabel->setAutoFillBackground(true);
    QHBoxLayout* lineColorLayout = new QHBoxLayout();
    lineColorLayout->addWidget(lineColorLabel);
    lineColorLayout->addWidget(lineColorBtn);
    propLayout->addRow(tr("线条颜色:"), lineColorLayout);
    connect(lineColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->m_lines[lineCombo->currentIndex()].m_style.lineColor, this);
        if (c.isValid()) {
            m_settings->m_lines[lineCombo->currentIndex()].m_style.lineColor = c;
            updateCurveStyle(lineCombo->currentIndex());
        }
        });

    pointWidthSpin = new QDoubleSpinBox();
    pointWidthSpin->setRange(1, 200);
    pointWidthSpin->setValue(1);
    pointWidthSpin->setSingleStep(0.1);
    propLayout->addRow(tr("默认点宽:"), pointWidthSpin);
    connect(pointWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, ([this](double) {
        m_settings->m_lines[lineCombo->currentIndex()].m_style.pointWidth = pointWidthSpin->value();
        updateCurveStyle(lineCombo->currentIndex());
        }));

    pointSizeSpin = new QDoubleSpinBox();
    pointSizeSpin->setRange(0, 200);
    pointSizeSpin->setValue(1);
    pointSizeSpin->setSingleStep(0.1);
    propLayout->addRow(tr("默认点大小:"), pointSizeSpin);
    connect(pointSizeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, ([this](double) {
        m_settings->m_lines[lineCombo->currentIndex()].m_style.pointSize = pointSizeSpin->value();
        updateCurveStyle(lineCombo->currentIndex());
        }));

    QPushButton* pointColorBtn = new QPushButton(tr("选择颜色"));
    pointColorLabel = new QLabel();
    pointColorLabel->setAutoFillBackground(true);
    QHBoxLayout* pointColorLayout = new QHBoxLayout();
    pointColorLayout->addWidget(pointColorLabel);
    pointColorLayout->addWidget(pointColorBtn);
    propLayout->addRow(tr("点颜色:"), pointColorLayout);
    connect(pointColorBtn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(m_settings->m_lines[lineCombo->currentIndex()].m_style.pointColor, this);
        if (c.isValid()) {
            m_settings->m_lines[lineCombo->currentIndex()].m_style.pointColor = c;
            updateCurveStyle(lineCombo->currentIndex());
        }
        });
    // 折线/曲线选择
    lineCurveCombo = new QComboBox();
    lineCurveCombo->addItem("折线", false);  // useCurve = false
    lineCurveCombo->addItem("曲线", true);   // useCurve = true
    // 可以设置默认值为第一项，或者从某个全局变量读取，这里假设所有线条当前 useCurve 一致，取第一条线的值作为默认
    bool defaultUseCurve = m_settings->m_lines.isEmpty() ? false : m_settings->m_lines[0].m_style.useCurve;
    lineCurveCombo->setCurrentIndex(defaultUseCurve ? 1 : 0);
    propLayout->addRow(tr("绘制形式:"), lineCurveCombo);
    connect(lineCurveCombo, &QComboBox::currentTextChanged, this, [this](const QString&) {
        m_settings->m_lines[lineCombo->currentIndex()].m_style.useCurve = (bool)lineCurveCombo->currentIndex();
        updateCurveStyle(lineCombo->currentIndex());
        });

    // 点样式组合框
    pointStyleCombo = new QComboBox();
    // 定义样式列表：每个条目为 (显示名称, QwtSymbol::Style, 是否填充)
    QList<std::tuple<QString, QwtSymbol::Style, bool>> pointStyles = {
        {"小点", QwtSymbol::Ellipse, true},
        {"X形交叉", QwtSymbol::XCross, false},
        {"加号十字形符号", QwtSymbol::Cross, false},
        {"未填充的圆圈", QwtSymbol::Ellipse, false},
        {"填充的圆圈", QwtSymbol::Ellipse, true},
        {"未填充的矩形", QwtSymbol::Rect, false},
        {"填充的矩形", QwtSymbol::Rect, true},
        {"未填充的三角形（尖端在底部）", QwtSymbol::DTriangle, false},
        {"填充的三角形", QwtSymbol::DTriangle, true},
        {"未填充的菱形", QwtSymbol::Diamond, false},
        {"填充的菱形", QwtSymbol::Diamond, true},
        {"未填充的星形", QwtSymbol::Star1, false},
        {"填充的星形", QwtSymbol::Star1, true}
    };
    int index = 0;
    for (const auto& item : pointStyles) {
        pointStyleCombo->addItem(std::get<0>(item), std::get<1>(item));
        pointStyleCombo->setItemData(index++, std::get<2>(item), Qt::UserRole + 1);
    }
    connect(pointStyleCombo, &QComboBox::currentTextChanged, this, [this](const QString&) {
        QwtSymbol::Style style = static_cast<QwtSymbol::Style>(pointStyleCombo->currentData().toInt());
        m_settings->m_lines[lineCombo->currentIndex()].m_style.pointStyle = static_cast<QwtSymbol::Style>(style);
        m_settings->m_lines[lineCombo->currentIndex()].m_style.pointFilled = pointStyleCombo->itemData(pointStyleCombo->currentIndex(), Qt::UserRole + 1).toBool();
        m_settings->m_lines[lineCombo->currentIndex()].m_style.fillPointStyle = (FillPointType)pointStyleCombo->currentIndex();
        updateCurveStyle(lineCombo->currentIndex());
        });
    propLayout->addRow(tr("点样式:"), pointStyleCombo);
    curveLayout->addWidget(propGroup);

    // 连接信号：线型改变时启用/禁用自定义虚线输入框
    //connect(lineStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //    [=](int index) {
    //        bool isCustom = (lineStyleCombo->itemData(index).toInt() == Qt::CustomDashLine);
    //        customDashEdit->setEnabled(isCustom);
    //    });

    bool value = connect(lineCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),this,
        [this](int idx) {
            int lineIndex = lineCombo->itemData(idx).toInt();
            updateCurveStyle(lineIndex);
        });

    // 初始化显示第一条线的设置
    if (!m_settings->m_lines.isEmpty())
        setCurrentCurve(0);

    tabWidget->addTab(globalTab, "全局");
    tabWidget->addTab(axisTab, "坐标系");
    tabWidget->addTab(curveTab, "线属性");

    xAutoRange->setChecked(m_settings->xAxis.autoRange);
    yAutoRange->setChecked(m_settings->yAxis.autoRange);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
}

void SimplePropertyBrowser::updateCurveStyle(int index)
{
    if (index < 0 || index >= m_settings->m_lines.size()) return;
    MTFLine& line = m_settings->m_lines[index];
    if (!line.curve) return;
    
    loadLineSettings(lineCombo->currentIndex()); 
   
    //DotLine,       DashDotLine,        DashDotDotLine,
    Qt::PenStyle lineStyle = line.m_style.lineStyle;
    if (lineStyle == (Qt::CustomDashLine + 1)) 
    {
        line.curve->setStyle(QwtPlotCurve::Dots);
    }
    else
    {
        // 线条
        QPen pen = line.curve->pen();
        pen.setWidth(line.m_style.lineWidth);
        pen.setColor(line.m_style.lineColor);
        pen.setStyle(line.m_style.lineStyle);
        if (line.m_style.lineStyle == Qt::CustomDashLine && !line.m_style.customDashPattern.isEmpty()) {
            pen.setDashPattern(line.m_style.customDashPattern);
        }
        line.curve->setPen(pen);
        line.curve->setStyle(QwtPlotCurve::Lines);
    }
    // 曲线/折线
    line.curve->setCurveAttribute(QwtPlotCurve::Fitted, line.m_style.useCurve);

    // 点符号
    if (line.curve->symbol())
    {
        line.curve->setSymbol(nullptr);
    }

    if ((line.m_style.pointStyle != QwtSymbol::NoSymbol)/* && (lineStyle != (Qt::CustomDashLine + 1))*/) {
        QwtSymbol* sym = new QwtSymbol(line.m_style.pointStyle);
        sym->setSize(line.m_style.pointSize);
        sym->setColor(line.m_style.pointColor);
        QPen symPen = sym->pen();
        symPen.setWidth(line.m_style.pointWidth);
        symPen.setColor(line.m_style.pointColor);
        sym->setPen(symPen);
        QBrush brush;
        brush.setStyle(line.m_style.pointFilled ? Qt::SolidPattern : Qt::NoBrush);
        brush.setColor(line.m_style.pointColor);
        sym->setBrush(brush);
        line.curve->setSymbol(sym);
    }
    else {
        line.curve->setSymbol(nullptr);
    }

    if (line.m_style.visible)
    {
        line.curve->setVisible(true);
    }
    else
    {
        line.curve->setVisible(false);
    }
    m_plot->replot();
}

void SimplePropertyBrowser::updateAllCurvesStyle()
{
    for (int i = 0; i < m_settings->m_lines.size(); ++i)
        updateCurveStyle(i);
}
void SimplePropertyBrowser::applyBackgroundColor()
{
    globalTab->blockSignals(true);
    bgColorLabel->setPalette(QPalette(m_settings->backgroundColor));
    globalTab->blockSignals(false);

    m_plot->setCanvasBackground(m_settings->backgroundColor);

}

void SimplePropertyBrowser::applyYAxisSettings()
{
    // Y轴
    if (m_settings->yAxis.visible)
    {
        m_plot->enableAxis(QwtPlot::yLeft);
        QwtText yTitle(m_settings->yAxis.title);
        yTitle.setFont(m_settings->yAxis.titleFont);
        yTitle.setColor(m_settings->yAxis.titleColor);
        m_plot->setAxisTitle(QwtPlot::yLeft, yTitle);
        m_plot->setAxisFont(QwtPlot::yLeft, m_settings->yAxis.tickFont);
        QwtScaleWidget* yScale = m_plot->axisWidget(QwtPlot::yLeft);
        if (yScale) {
            QPalette pal = yScale->palette();
            pal.setColor(QPalette::Text, m_settings->yAxis.tickColor);
            yScale->setPalette(pal);
        }
        if (!m_settings->yAxis.autoRange)
        {
            m_plot->setAxisScale(QwtPlot::yLeft, m_settings->yAxis.min, m_settings->yAxis.max, m_settings->yAxis.step);
            signalUpdateScaleDiv();
        }
        else
        {
            autoYScaleAxes();
        }
    }
    else 
    {
        m_plot->enableAxis(QwtPlot::yLeft, false);
    }

    //signalUpdateScaleDiv();

    axisTab->blockSignals(true);
    yVisible->setChecked(m_settings->yAxis.visible);
    yTitle->setText(m_settings->yAxis.title);
    QFont font = m_settings->yAxis.tickFont;
    yTitleFontLabel->setText(font.family());
    QColor color = m_settings->yAxis.titleColor;
    yTitleColorLabel->setPalette(QPalette(color));
    yAutoRange->setChecked(m_settings->yAxis.autoRange);
    yMin->setValue(m_settings->yAxis.min);
    yMax->setValue(m_settings->yAxis.max);
    yStep->setValue(m_settings->yAxis.step);
    font = m_settings->yAxis.tickFont;
    yTickFontLabel->setText(font.family());
    color = m_settings->yAxis.tickColor;
    yTickColorLabel->setPalette(QPalette(color));
    axisTab->blockSignals(false);

    m_plot->replot();
}
void SimplePropertyBrowser::applyXAxisSettings()
{
    // X轴
    if (m_settings->xAxis.visible) 
    {
        m_plot->enableAxis(QwtPlot::xBottom);
        QwtText xTitle(m_settings->xAxis.title);
        xTitle.setFont(m_settings->xAxis.titleFont);
        xTitle.setColor(m_settings->xAxis.titleColor);
        m_plot->setAxisTitle(QwtPlot::xBottom, xTitle);
        m_plot->setAxisFont(QwtPlot::xBottom, m_settings->xAxis.tickFont);
        QwtScaleWidget* xScale = m_plot->axisWidget(QwtPlot::xBottom);
        if (xScale) {
            QPalette pal = xScale->palette();
            pal.setColor(QPalette::Text, m_settings->xAxis.tickColor);
            xScale->setPalette(pal);
        }
        if (!m_settings->xAxis.autoRange)
        {
            m_plot->setAxisScale(QwtPlot::xBottom, m_settings->xAxis.min, m_settings->xAxis.max, m_settings->xAxis.step);
            signalUpdateScaleDiv();
        }
        else
        {
            autoXScaleAxes();
        }
    }
    else 
    {
        m_plot->enableAxis(QwtPlot::xBottom, false);
    }

    //signalUpdateScaleDiv();

    axisTab->blockSignals(true);
    xVisible->setChecked(m_settings->xAxis.visible);
    xTitle->setText(m_settings->xAxis.title);
    QFont font = m_settings->xAxis.tickFont;
    xTitleFontLabel->setText(font.family());
    QColor color = m_settings->xAxis.titleColor;
    xTitleColorLabel->setPalette(QPalette(color));
    xAutoRange->setChecked(m_settings->xAxis.autoRange);
    xMin->setValue(m_settings->xAxis.min);
    xMax->setValue(m_settings->xAxis.max);
    xStep->setValue(m_settings->xAxis.step);
    font = m_settings->xAxis.tickFont;
    xTickFontLabel->setText(font.family());
    color = m_settings->xAxis.tickColor;
    xTickColorLabel->setPalette(QPalette(color));
    axisTab->blockSignals(false);

    m_plot->replot();
}

void SimplePropertyBrowser::autoXScaleAxes()
{
    signalXScaleAxes();
    //double minX = std::numeric_limits<double>::max();
    //double maxX = std::numeric_limits<double>::lowest();
    //bool hasData = false;
    //for (const MTFLine& line : m_settings->m_lines) {
    //    if (!line.m_style.visible || line.data.isEmpty()) continue;
    //    hasData = true;
    //    for (const QPointF& p : line.data) {
    //        if (p.x() < minX) minX = p.x();
    //        if (p.x() > maxX) maxX = p.x();
    //    }
    //}
    //if (hasData) {
    //    double xMargin = (maxX - minX) * 0.05;
    //    if (xMargin <= 0) xMargin = 1.0;

    //    m_plot->setAxisScale(QwtPlot::xBottom, minX - xMargin, maxX + xMargin);
    //    QwtScaleDiv temp = m_plot->axisScaleDiv(QwtPlot::xBottom);
    //    QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);
    //    m_settings->xAxis.min = minX - xMargin;
    //    m_settings->xAxis.max = maxX + xMargin;

    //    m_settings->xAxis.step = ticks[1] - ticks[0];
    //}
    //else {
    //    m_settings->xAxis.min = m_defaultXMin;
    //    m_settings->xAxis.max = m_defaultXMax;
    //}
}
void SimplePropertyBrowser::autoYScaleAxes()
{
    signalYScaleAxes();
    //double minY = std::numeric_limits<double>::max();
    //double maxY = std::numeric_limits<double>::lowest();
    //bool hasData = false;
    //for (const MTFLine& line : m_settings->m_lines) {
    //    if (!line.m_style.visible || line.data.isEmpty()) continue;
    //    hasData = true;
    //    for (const QPointF& p : line.data) {
    //        if (p.y() < minY) minY = p.y();
    //        if (p.y() > maxY) maxY = p.y();
    //    }
    //}
    //if (hasData) {
    //    double yMargin = (maxY - minY) * 0.05;
    //    if (yMargin <= 0) yMargin = 0.1;
    //    m_plot->setAxisScale(QwtPlot::yLeft, std::max(0.0, minY - yMargin), maxY + yMargin);
    //    QwtScaleDiv temp = m_plot->axisScaleDiv(QwtPlot::xBottom);
    //    QList<double> ticks = temp.ticks(QwtScaleDiv::MajorTick);

    //    m_settings->yAxis.min = minY - yMargin;
    //    m_settings->yAxis.max = maxY + yMargin;
    //    temp = m_plot->axisScaleDiv(QwtPlot::yLeft);
    //    ticks = temp.ticks(QwtScaleDiv::MajorTick);
    //    m_settings->yAxis.step = ticks[1] - ticks[0];

    //}
    //else {
    //    m_settings->yAxis.min = m_defaultYMin;
    //    m_settings->yAxis.max = m_defaultYMax;
    //}
}

void SimplePropertyBrowser::applyGridSettings()
{
    globalTab->blockSignals(true);
    gridVisibleCheck->setChecked(m_settings->gridVisible);
    globalTab->blockSignals(false);
    if (m_settings->gridVisible) 
    {
        m_grid->enableX(true);
        m_grid->enableY(true);
        m_grid->setMajorPen(QPen(m_settings->gridMajorColor, 0, m_settings->gridMajorStyle));
        m_grid->setMinorPen(QPen(m_settings->gridMinorColor, 0, m_settings->gridMinorStyle));
        m_grid->attach(m_plot);
    }
    else {
        m_grid->detach();
    }
    m_plot->replot();
}

void SimplePropertyBrowser::applyTitleSettings()
{
    globalTab->blockSignals(true);
    titleEdit->setText(m_settings->title);
    titleFontLabel->setFont(m_settings->title);

    QFont font = m_settings->titleFont;
    titleFontLabel->setText(font.family());
    QColor color = m_settings->titleColor;
    titleColorLabel->setPalette(QPalette(color));
    globalTab->blockSignals(false);

    QwtText title(m_settings->title);
    title.setFont(m_settings->titleFont);
    title.setColor(m_settings->titleColor);
    m_plot->setTitle(title);
    m_plot->replot();
}

void SimplePropertyBrowser::applyLegendSettings()
{
    globalTab->blockSignals(true);
    QFont font = m_settings->legend.font;
    legendFontLabel->setText(font.family());
    QColor color = m_settings->legend.color;
    legendColorLabel->setPalette(QPalette(color));
    globalTab->blockSignals(false);

    legendVisibleCheck->setChecked(m_settings->legend.visible);
    legendPosCombo->setCurrentIndex(m_settings->legend.position);

    if (m_settings->legend.visible)
    {
        // 显示图例：如果之前移除了，需要重新插入；如果已存在，直接显示并更新样式
        if (!m_legend) {
            m_legend = new QwtLegend();
            // 可能需要对图例进行一些初始化设置
        }
        m_plot->insertLegend(m_legend, m_settings->legend.position);
        m_legend->setVisible(true);
        updateLegendItemsStyle();
    }
    else
    {
        // 隐藏图例：从 Plot 中移除，完全消失
        delete m_legend;
        m_plot->insertLegend(nullptr);
        m_legend = NULL;
        // 如果 m_legend 是你管理的成员，可以保留对象以便下次快速显示，
        // 或者直接删除并置空（取决于你的内存管理策略）
        // 这里假设保留 m_legend 对象，仅从 plot 中移除
    }
    m_plot->replot();
}
void SimplePropertyBrowser::updateLegendItemsStyle()
{
    if (!m_legend) return;
    QList<QwtLegendLabel*> labels = m_legend->findChildren<QwtLegendLabel*>();
    for (QwtLegendLabel* label : labels) {
        label->setFont(m_settings->legend.font);
        QPalette pal = label->palette();
        pal.setColor(QPalette::Text, m_settings->legend.color);
        label->setPalette(pal);
    }
    m_plot->replot();
}
void SimplePropertyBrowser::applyOrigin()
{
    QwtScaleDiv xDiv = m_plot->axisScaleDiv(QwtPlot::xBottom);
    QwtScaleDiv yDiv = m_plot->axisScaleDiv(QwtPlot::yLeft);


    globalTab->blockSignals(true);
    originCombo->setCurrentIndex(m_settings->origin);
    globalTab->blockSignals(false);
    switch (m_settings->origin) {
    case PlotSettings::BottomLeft:
    {
        // 取消反转
        m_plot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, false);
        m_plot->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, false);
        // 确保范围是正序
        if (xDiv.lowerBound() > xDiv.upperBound()) {
            m_plot->setAxisScale(QwtPlot::xBottom, xDiv.upperBound(), xDiv.lowerBound());
        }
        else
        {
            m_plot->setAxisScale(QwtPlot::xBottom, xDiv.lowerBound(), xDiv.upperBound());
        }
        if (yDiv.lowerBound() > yDiv.upperBound()) 
        {
            m_plot->setAxisScale(QwtPlot::yLeft, yDiv.upperBound(), yDiv.lowerBound());
        }
        else
        {
            m_plot->setAxisScale(QwtPlot::yLeft, yDiv.lowerBound(), yDiv.upperBound());
        }
    }
        break;
    case PlotSettings::BottomRight:
    {
        // 反转 X 轴，Y 轴不反转
        m_plot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, true);
        m_plot->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, false);
        // 设置范围为从小到大的值，但引擎会负责反转
        if (xDiv.lowerBound() > xDiv.upperBound()) {
            m_plot->setAxisScale(QwtPlot::xBottom, xDiv.lowerBound(), xDiv.upperBound());
        }
        else
        {
            m_plot->setAxisScale(QwtPlot::xBottom, xDiv.upperBound(), xDiv.lowerBound());
        }
        if (yDiv.lowerBound() > yDiv.upperBound()) 
        {
            m_plot->setAxisScale(QwtPlot::yLeft, yDiv.upperBound(), yDiv.lowerBound());
        }
        else
        {
            m_plot->setAxisScale(QwtPlot::yLeft, yDiv.lowerBound(), yDiv.upperBound());
        }
    }
        break;
    case PlotSettings::Center:
    {
        // 取消反转，设置对称范围
        m_plot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, false);
        m_plot->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, false);

        // 计算数据范围（与之前相同）
        //double xMin, xMax, yMin, yMax;
        //if (m_settings->m_lines.isEmpty()) {
        //    xMin = m_defaultXMin;
        //    xMax = m_defaultXMax;
        //    yMin = m_defaultYMin;
        //    yMax = m_defaultYMax;
        //}
        //else {
        //    xMin = std::numeric_limits<double>::max();
        //    xMax = std::numeric_limits<double>::lowest();
        //    yMin = std::numeric_limits<double>::max();
        //    yMax = std::numeric_limits<double>::lowest();
        //    bool hasData = false;
        //    for (const MTFLine& line : m_settings->m_lines) {
        //        if (!line.m_style.visible || line.data.isEmpty()) continue;
        //        hasData = true;
        //        for (const QPointF& p : line.data) {
        //            if (p.x() < xMin) xMin = p.x();
        //            if (p.x() > xMax) xMax = p.x();
        //            if (p.y() < yMin) yMin = p.y();
        //            if (p.y() > yMax) yMax = p.y();
        //        }
        //    }
        //    if (!hasData) {
        //        xMin = m_defaultXMin;
        //        xMax = m_defaultXMax;
        //        yMin = m_defaultYMin;
        //        yMax = m_defaultYMax;
        //    }
        //}
        double xRange = xDiv.upperBound() - xDiv.lowerBound() ;
        double yRange = yDiv.upperBound()- yDiv.lowerBound() ;
        m_plot->setAxisScale(QwtPlot::xBottom, -xRange/2, xRange/2);
        m_plot->setAxisScale(QwtPlot::yLeft, -yRange/2, yRange/2);
    }
    break;
    }

    m_plot->replot();
}

void SimplePropertyBrowser::updateLineCombo()
{
    lineCombo->clear();
    for (int i = 0; i < m_settings->m_lines.size(); ++i) {
        const MTFLine& line = m_settings->m_lines[i];
        QString displayName = QString("Line %1").arg(line.index);
        if (!line.label.isEmpty()) displayName += ": " + line.label;
        lineCombo->addItem(displayName, i); // 存储索引作为数据
    }
}
void SimplePropertyBrowser::setCurrentCurve(int index)
{
    lineCombo->setCurrentIndex(index);
    //loadLineSettings(index);
}

QVector<qreal> SimplePropertyBrowser::parseDashPattern (const QString& text)
{
    QVector<qreal> pattern;
    QStringList parts = text.split(' ', QString::SkipEmptyParts);
    for (const QString& part : parts) {
        bool ok;
        qreal v = part.toDouble(&ok);
        if (ok && v > 0) pattern.append(v);
    }
    if (pattern.size() % 2 != 0) pattern.append(pattern.last()); // 简单补全
    return pattern;
};