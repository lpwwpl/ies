#include "IESglwidget.h"


void PointCloudViewer::updateIES()
{
	generateSampleData();
	setupPointCloud();
}

//#include "IESglwidget.h"
//#include <QDebug>
//#include <QMouseEvent>
//#include <QWheelEvent>
//#include <QtMath>
//
//OpenGLWidget::OpenGLWidget(QWidget* parent)
//    : QOpenGLWidget(parent), gridSize(10.0f), gridDivisions(20),
//    cameraDistance(15.0f), minValue(0.0f), maxValue(1.0f)
//{
//    // 设置初始相机位置
//    cameraPosition = QVector3D(1.0f, 1.0f, 1.0f).normalized() * cameraDistance;
//    cameraTarget = QVector3D(0.0f, 0.0f, 0.0f);
//
//    // 启用鼠标跟踪
//    setMouseTracking(true);
//}
//
//OpenGLWidget::~OpenGLWidget()
//{
//    makeCurrent();
//    gridVAO.destroy();
//    gridVBO.destroy();
//    doneCurrent();
//}
//
//void OpenGLWidget::initializeGL()
//{
//    // 初始化OpenGL函数
//    initializeOpenGLFunctions();
//
//    // 设置清屏颜色和深度测试
//    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//    glEnable(GL_DEPTH_TEST);
//
//    // 设置网格着色器
//    setupShaderProgram(gridShaderProgram,
//        ":/resources/shaders/grid.vert",
//        ":/resources/shaders/grid.frag");
//
//    // 设置色度条着色器
//    setupShaderProgram(colorBarShaderProgram,
//        ":/resources/shaders/colorbar.vert",
//        ":/resources/shaders/colorbar.frag");
//
//    // 初始化网格VAO和VBO
//    gridVAO.create();
//    gridVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
//    gridVBO.create();
//
//    // 生成网格数据
//    QVector<float> gridVertices;
//    float halfSize = gridSize / 2.0f;
//    float step = gridSize / gridDivisions;
//
//    // XY平面网格
//    for (int i = 0; i <= gridDivisions; ++i) {
//        float x = -halfSize + i * step;
//        gridVertices << x << -halfSize << 0.0f;
//        gridVertices << x << halfSize << 0.0f;
//        gridVertices << -halfSize << x << 0.0f;
//        gridVertices << halfSize << x << 0.0f;
//    }
//
//    // XZ平面网格
//    for (int i = 0; i <= gridDivisions; ++i) {
//        float x = -halfSize + i * step;
//        gridVertices << x << 0.0f << -halfSize;
//        gridVertices << x << 0.0f << halfSize;
//        gridVertices << -halfSize << 0.0f << x;
//        gridVertices << halfSize << 0.0f << x;
//    }
//
//    // YZ平面网格
//    for (int i = 0; i <= gridDivisions; ++i) {
//        float y = -halfSize + i * step;
//        gridVertices << 0.0f << y << -halfSize;
//        gridVertices << 0.0f << y << halfSize;
//        gridVertices << 0.0f << -halfSize << y;
//        gridVertices << 0.0f << halfSize << y;
//    }
//
//    // 上传网格数据到GPU
//    gridVAO.bind();
//    gridVBO.bind();
//    gridVBO.allocate(gridVertices.constData(), gridVertices.size() * sizeof(float));
//
//    gridShaderProgram.bind();
//    gridShaderProgram.enableAttributeArray(0);
//    gridShaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
//
//    gridVAO.release();
//    gridVBO.release();
//    gridShaderProgram.release();
//}
//
//void OpenGLWidget::setupShaderProgram(QOpenGLShaderProgram& program,
//    const QString& vertexShader,
//    const QString& fragmentShader)
//{
//    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShader))
//        qDebug() << "Vertex shader error:" << program.log();
//
//    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader))
//        qDebug() << "Fragment shader error:" << program.log();
//
//    if (!program.link())
//        qDebug() << "Shader program link error:" << program.log();
//}
//
//void OpenGLWidget::resizeGL(int w, int h)
//{
//    glViewport(0, 0, w, h);
//    projection.setToIdentity();
//    projection.perspective(45.0f, static_cast<float>(w) / h, 0.1f, 100.0f);
//}
//
//void OpenGLWidget::paintGL()
//{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    // 更新视图矩阵
//    view.setToIdentity();
//    view.lookAt(cameraPosition, cameraTarget, QVector3D(0.0f, 1.0f, 0.0f));
//
//    // 绘制网格
//    drawGrids();
//
//    // 绘制色度条
//    drawColorBar();
//}
//
//void OpenGLWidget::drawGrids()
//{
//    gridShaderProgram.bind();
//    gridVAO.bind();
//
//    // 设置模型视图投影矩阵
//    QMatrix4x4 mvp = projection * view * model;
//    gridShaderProgram.setUniformValue("mvp", mvp);
//
//    // 设置网格颜色
//    gridShaderProgram.setUniformValue("color", QVector4D(0.5f, 0.5f, 0.5f, 1.0f));
//
//    // 绘制网格线
//    glDrawArrays(GL_LINES, 0, 3 * 4 * (gridDivisions + 1)); // 3个平面，每个平面有2组线，每组(gridDivisions+1)条线
//
//    gridVAO.release();
//    gridShaderProgram.release();
//}
//
//void OpenGLWidget::drawColorBar()
//{
//    // 方法1: 使用OpenGL绘制色度条
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    glOrtho(0, width(), 0, height(), -1, 1);
//
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadIdentity();
//
//    // 禁用深度测试以确保色度条绘制在最前面
//    glDisable(GL_DEPTH_TEST);
//
//    // 绘制色度条背景
//    int barWidth = 30;
//    int barHeight = 200;
//    int barX = width() - barWidth - 10;
//    int barY = height() - barHeight - 10;
//
//    // 使用渐变着色
//    glBegin(GL_QUADS);
//    for (int i = 0; i < barHeight; ++i) {
//        float t = static_cast<float>(i) / barHeight;
//        QColor color = getColorFromValue(minValue + t * (maxValue - minValue));
//        glColor3f(color.redF(), color.greenF(), color.blueF());
//        glVertex2i(barX, barY + i);
//        glVertex2i(barX + barWidth, barY + i);
//        glVertex2i(barX + barWidth, barY + i + 1);
//        glVertex2i(barX, barY + i + 1);
//    }
//    glEnd();
//
//    // 绘制边框
//    glColor3f(1.0f, 1.0f, 1.0f);
//    glBegin(GL_LINE_LOOP);
//    glVertex2i(barX, barY);
//    glVertex2i(barX + barWidth, barY);
//    glVertex2i(barX + barWidth, barY + barHeight);
//    glVertex2i(barX, barY + barHeight);
//    glEnd();
//
//    // 恢复深度测试和矩阵状态
//    glEnable(GL_DEPTH_TEST);
//    glPopMatrix();
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
//
//    // 方法2: 使用QPainter绘制文本标签（需要混合）
//    QPainter painter(this);
//    painter.setPen(Qt::white);
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
//
//    // 绘制最小值标签
//    painter.drawText(width() - barWidth - 40, height() - 10, QString::number(minValue, 'f', 2));
//
//    // 绘制最大值标签
//    painter.drawText(width() - barWidth - 40, height() - barHeight - 20, QString::number(maxValue, 'f', 2));
//
//    // 绘制标题
//    painter.drawText(width() - barWidth - 10, height() - barHeight - 40, "Value");
//
//    painter.end();
//}
//
//QColor OpenGLWidget::getColorFromValue(float value)
//{
//    // 将值映射到颜色（使用热力图颜色映射）
//    float normalized = (value - minValue) / (maxValue - minValue);
//
//    if (normalized < 0.25f) {
//        // 蓝色到青色
//        return QColor::fromRgbF(0.0f, normalized * 4.0f, 1.0f);
//    }
//    else if (normalized < 0.5f) {
//        // 青色到绿色
//        return QColor::fromRgbF(0.0f, 1.0f, 1.0f - (normalized - 0.25f) * 4.0f);
//    }
//    else if (normalized < 0.75f) {
//        // 绿色到黄色
//        return QColor::fromRgbF((normalized - 0.5f) * 4.0f, 1.0f, 0.0f);
//    }
//    else {
//        // 黄色到红色
//        return QColor::fromRgbF(1.0f, 1.0f - (normalized - 0.75f) * 4.0f, 0.0f);
//    }
//}
//
//void OpenGLWidget::mousePressEvent(QMouseEvent* event)
//{
//    lastMousePos = event->pos();
//}
//
//void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
//{
//    int dx = event->x() - lastMousePos.x();
//    int dy = event->y() - lastMousePos.y();
//
//    if (event->buttons() & Qt::LeftButton) {
//        // 旋转视图
//        QVector3D right = QVector3D::crossProduct(
//            (cameraTarget - cameraPosition).normalized(),
//            QVector3D(0.0f, 1.0f, 0.0f)).normalized();
//
//        QVector3D up = QVector3D(0.0f, 1.0f, 0.0f);
//
//        // 计算旋转角度
//        float xAngle = -dx * 0.5f;
//        float yAngle = -dy * 0.5f;
//
//        // 创建旋转四元数
//        QQuaternion xRot = QQuaternion::fromAxisAndAngle(up, xAngle);
//        QQuaternion yRot = QQuaternion::fromAxisAndAngle(right, yAngle);
//        QQuaternion rotation = yRot * xRot;
//
//        // 应用旋转
//        cameraPosition = cameraTarget + rotation.rotatedVector(cameraPosition - cameraTarget);
//    }
//
//    lastMousePos = event->pos();
//    update();
//}
//
//void OpenGLWidget::wheelEvent(QWheelEvent* event)
//{
//    // 缩放视图
//    float delta = event->angleDelta().y() / 120.0f;
//    cameraDistance *= (1.0f - delta * 0.1f);
//    cameraDistance = qMax(1.0f, qMin(cameraDistance, 50.0f));
//
//    QVector3D direction = (cameraPosition - cameraTarget).normalized();
//    cameraPosition = cameraTarget + direction * cameraDistance;
//
//    update();
//}
//
//void OpenGLWidget::setData(const QVector<QVector3D>& points, const QVector<float>& values)
//{
//    dataPoints = points;
//    dataValues = values;
//
//    // 计算值的范围
//    if (!values.isEmpty()) {
//        minValue = maxValue = values[0];
//        for (float value : values) {
//            if (value < minValue) minValue = value;
//            if (value > maxValue) maxValue = value;
//        }
//    }
//
//    update();
//}