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
//    // ���ó�ʼ���λ��
//    cameraPosition = QVector3D(1.0f, 1.0f, 1.0f).normalized() * cameraDistance;
//    cameraTarget = QVector3D(0.0f, 0.0f, 0.0f);
//
//    // ����������
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
//    // ��ʼ��OpenGL����
//    initializeOpenGLFunctions();
//
//    // ����������ɫ����Ȳ���
//    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//    glEnable(GL_DEPTH_TEST);
//
//    // ����������ɫ��
//    setupShaderProgram(gridShaderProgram,
//        ":/resources/shaders/grid.vert",
//        ":/resources/shaders/grid.frag");
//
//    // ����ɫ������ɫ��
//    setupShaderProgram(colorBarShaderProgram,
//        ":/resources/shaders/colorbar.vert",
//        ":/resources/shaders/colorbar.frag");
//
//    // ��ʼ������VAO��VBO
//    gridVAO.create();
//    gridVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
//    gridVBO.create();
//
//    // ������������
//    QVector<float> gridVertices;
//    float halfSize = gridSize / 2.0f;
//    float step = gridSize / gridDivisions;
//
//    // XYƽ������
//    for (int i = 0; i <= gridDivisions; ++i) {
//        float x = -halfSize + i * step;
//        gridVertices << x << -halfSize << 0.0f;
//        gridVertices << x << halfSize << 0.0f;
//        gridVertices << -halfSize << x << 0.0f;
//        gridVertices << halfSize << x << 0.0f;
//    }
//
//    // XZƽ������
//    for (int i = 0; i <= gridDivisions; ++i) {
//        float x = -halfSize + i * step;
//        gridVertices << x << 0.0f << -halfSize;
//        gridVertices << x << 0.0f << halfSize;
//        gridVertices << -halfSize << 0.0f << x;
//        gridVertices << halfSize << 0.0f << x;
//    }
//
//    // YZƽ������
//    for (int i = 0; i <= gridDivisions; ++i) {
//        float y = -halfSize + i * step;
//        gridVertices << 0.0f << y << -halfSize;
//        gridVertices << 0.0f << y << halfSize;
//        gridVertices << 0.0f << -halfSize << y;
//        gridVertices << 0.0f << halfSize << y;
//    }
//
//    // �ϴ��������ݵ�GPU
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
//    // ������ͼ����
//    view.setToIdentity();
//    view.lookAt(cameraPosition, cameraTarget, QVector3D(0.0f, 1.0f, 0.0f));
//
//    // ��������
//    drawGrids();
//
//    // ����ɫ����
//    drawColorBar();
//}
//
//void OpenGLWidget::drawGrids()
//{
//    gridShaderProgram.bind();
//    gridVAO.bind();
//
//    // ����ģ����ͼͶӰ����
//    QMatrix4x4 mvp = projection * view * model;
//    gridShaderProgram.setUniformValue("mvp", mvp);
//
//    // ����������ɫ
//    gridShaderProgram.setUniformValue("color", QVector4D(0.5f, 0.5f, 0.5f, 1.0f));
//
//    // ����������
//    glDrawArrays(GL_LINES, 0, 3 * 4 * (gridDivisions + 1)); // 3��ƽ�棬ÿ��ƽ����2���ߣ�ÿ��(gridDivisions+1)����
//
//    gridVAO.release();
//    gridShaderProgram.release();
//}
//
//void OpenGLWidget::drawColorBar()
//{
//    // ����1: ʹ��OpenGL����ɫ����
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    glOrtho(0, width(), 0, height(), -1, 1);
//
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadIdentity();
//
//    // ������Ȳ�����ȷ��ɫ������������ǰ��
//    glDisable(GL_DEPTH_TEST);
//
//    // ����ɫ��������
//    int barWidth = 30;
//    int barHeight = 200;
//    int barX = width() - barWidth - 10;
//    int barY = height() - barHeight - 10;
//
//    // ʹ�ý�����ɫ
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
//    // ���Ʊ߿�
//    glColor3f(1.0f, 1.0f, 1.0f);
//    glBegin(GL_LINE_LOOP);
//    glVertex2i(barX, barY);
//    glVertex2i(barX + barWidth, barY);
//    glVertex2i(barX + barWidth, barY + barHeight);
//    glVertex2i(barX, barY + barHeight);
//    glEnd();
//
//    // �ָ���Ȳ��Ժ;���״̬
//    glEnable(GL_DEPTH_TEST);
//    glPopMatrix();
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
//
//    // ����2: ʹ��QPainter�����ı���ǩ����Ҫ��ϣ�
//    QPainter painter(this);
//    painter.setPen(Qt::white);
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
//
//    // ������Сֵ��ǩ
//    painter.drawText(width() - barWidth - 40, height() - 10, QString::number(minValue, 'f', 2));
//
//    // �������ֵ��ǩ
//    painter.drawText(width() - barWidth - 40, height() - barHeight - 20, QString::number(maxValue, 'f', 2));
//
//    // ���Ʊ���
//    painter.drawText(width() - barWidth - 10, height() - barHeight - 40, "Value");
//
//    painter.end();
//}
//
//QColor OpenGLWidget::getColorFromValue(float value)
//{
//    // ��ֵӳ�䵽��ɫ��ʹ������ͼ��ɫӳ�䣩
//    float normalized = (value - minValue) / (maxValue - minValue);
//
//    if (normalized < 0.25f) {
//        // ��ɫ����ɫ
//        return QColor::fromRgbF(0.0f, normalized * 4.0f, 1.0f);
//    }
//    else if (normalized < 0.5f) {
//        // ��ɫ����ɫ
//        return QColor::fromRgbF(0.0f, 1.0f, 1.0f - (normalized - 0.25f) * 4.0f);
//    }
//    else if (normalized < 0.75f) {
//        // ��ɫ����ɫ
//        return QColor::fromRgbF((normalized - 0.5f) * 4.0f, 1.0f, 0.0f);
//    }
//    else {
//        // ��ɫ����ɫ
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
//        // ��ת��ͼ
//        QVector3D right = QVector3D::crossProduct(
//            (cameraTarget - cameraPosition).normalized(),
//            QVector3D(0.0f, 1.0f, 0.0f)).normalized();
//
//        QVector3D up = QVector3D(0.0f, 1.0f, 0.0f);
//
//        // ������ת�Ƕ�
//        float xAngle = -dx * 0.5f;
//        float yAngle = -dy * 0.5f;
//
//        // ������ת��Ԫ��
//        QQuaternion xRot = QQuaternion::fromAxisAndAngle(up, xAngle);
//        QQuaternion yRot = QQuaternion::fromAxisAndAngle(right, yAngle);
//        QQuaternion rotation = yRot * xRot;
//
//        // Ӧ����ת
//        cameraPosition = cameraTarget + rotation.rotatedVector(cameraPosition - cameraTarget);
//    }
//
//    lastMousePos = event->pos();
//    update();
//}
//
//void OpenGLWidget::wheelEvent(QWheelEvent* event)
//{
//    // ������ͼ
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
//    // ����ֵ�ķ�Χ
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