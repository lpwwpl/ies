#pragma once
#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPainter>
#include <QTime>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>
#include <unordered_map>
#include <functional>

// ����QCustomPlotͷ�ļ�
#include "qcustomplot.h"

#include "IESLoader.h"

// ColorBar ���
class ColorBar : public QWidget {
    Q_OBJECT
public:
    ColorBar(QWidget* parent = nullptr) : QWidget(parent), m_minValue(0), m_maxValue(1) {
        setFixedWidth(40);
        setMinimumHeight(200);
    }

    void setRange(float min, float max) {
        m_minValue = min;
        m_maxValue = max;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // ������ɫ��
        QLinearGradient gradient(0, height(), 0, 0);
        gradient.setColorAt(0.0, QColor(0, 0, 255));   // ��ɫ
        gradient.setColorAt(0.25, QColor(0, 255, 255)); // ��ɫ
        gradient.setColorAt(0.5, QColor(0, 255, 0));   // ��ɫ
        gradient.setColorAt(0.75, QColor(255, 255, 0)); // ��ɫ
        gradient.setColorAt(1.0, QColor(255, 0, 0));   // ��ɫ

        painter.fillRect(10, 10, 20, height() - 20, gradient);

        // ���ƿ̶�
        painter.setPen(Qt::white);
        QFont font;
        font.setPointSize(8);
        painter.setFont(font);

        float range = m_maxValue - m_minValue;
        for (int i = 0; i <= 10; ++i) {
            float value = m_minValue + range * i / 10.0f;
            int y = height() - 20 - (height() - 40) * i / 10 + 10;
            painter.drawLine(30, y, 35, y);
            painter.drawText(36, y + 4, QString::number(value, 'f', 2));
        }
    }

private:
    float m_minValue, m_maxValue;
};
// �������ݽṹ
struct PointCloud {
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals; // ��������
    QVector<float> values; // ÿ����ı���ֵ��������ɫӳ��
    QVector<unsigned int> indices; // ��������
    float minValue, maxValue;
    bool hasNormals;
    bool hasIndices;
};

// ƽ������ö��
enum PlaneType {
    NO_PLANE,
    XY_PLANE,
    YZ_PLANE,
    XZ_PLANE
};

// ��ʾģʽö��
enum DisplayMode {
    POINTS_MODE,
    SHADED_MODE,
    MESH_MODE
};

// OpenGL���ӻ����
class PointCloudViewer : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit PointCloudViewer(QWidget* parent = nullptr)
        : QOpenGLWidget(parent), m_pointSize(3.0f),
        m_xyPlanePos(0.0f), m_yzPlanePos(0.0f), m_xzPlanePos(0.0f),
        m_selectedPlane(NO_PLANE), m_planeThreshold(0.05f),
        m_displayMode(POINTS_MODE), m_lightPosition(2.0f, 2.0f, 2.0f) {
        // ����ʾ����������
        //generateSampleData();
    }

    ~PointCloudViewer() {
        makeCurrent();
        m_vao.destroy();
        m_vbo.destroy();
        m_nbo.destroy();
        m_ibo.destroy();
        doneCurrent();
    }
    void updateIES();
    void setPointSize(float size) {
        m_pointSize = size;
        update();
    }

    void setPlanePosition(PlaneType type, float position) {
        switch (type) {
        case XY_PLANE:
            m_xyPlanePos = position;
            break;
        case YZ_PLANE:
            m_yzPlanePos = position;
            break;
        case XZ_PLANE:
            m_xzPlanePos = position;
            break;
        default:
            break;
        }
        update();
        emit planePositionChanged(type, position);
    }

    void setSelectedPlane(PlaneType type) {
        m_selectedPlane = type;
        update();
    }

    void setPlaneThreshold(float threshold) {
        m_planeThreshold = threshold;
        update();
    }

    void setDisplayMode(DisplayMode mode) {
        m_displayMode = mode;
        update();
    }

    void setLightPosition(const QVector3D& position) {
        m_lightPosition = position;
        update();
    }

    float getMinValue() const { return m_cloud.minValue; }
    float getMaxValue() const { return m_cloud.maxValue; }
    float getXYPlanePosition() const { return m_xyPlanePos; }
    float getYZPlanePosition() const { return m_yzPlanePos; }
    float getXZPlanePosition() const { return m_xzPlanePos; }

    // ��ȡƽ���ϵĵ�����
    QVector<QPointF> getPlanePoints(PlaneType planeType, float position) const {
        QVector<QPointF> points;

        for (int i = 0; i < m_cloud.vertices.size(); ++i) {
            const QVector3D& vertex = m_cloud.vertices[i];
            float value = m_cloud.values[i];

            bool onPlane = false;
            QPointF point;

            switch (planeType) {
            case XY_PLANE:
                if (std::abs(vertex.z() - position) <= m_planeThreshold) {
                    point = QPointF(vertex.x(), vertex.y());
                    onPlane = true;
                }
                break;
            case YZ_PLANE:
                if (std::abs(vertex.x() - position) <= m_planeThreshold) {
                    point = QPointF(vertex.y(), vertex.z());
                    onPlane = true;
                }
                break;
            case XZ_PLANE:
                if (std::abs(vertex.y() - position) <= m_planeThreshold) {
                    point = QPointF(vertex.x(), vertex.z());
                    onPlane = true;
                }
                break;
            default:
                break;
            }

            if (onPlane) {
                points.append(point);
            }
        }

        return points;
    }

    // ��ȡƽ���ϵĵ����ݼ���ֵ
    QVector<QPointF> getPlanePointsWithValues(PlaneType planeType, float position, QVector<float>& values) const {
        QVector<QPointF> points;
        values.clear();

        for (int i = 0; i < m_cloud.vertices.size(); ++i) {
            const QVector3D& vertex = m_cloud.vertices[i];
            float value = m_cloud.values[i];

            bool onPlane = false;
            QPointF point;

            switch (planeType) {
            case XY_PLANE:
                if (std::abs(vertex.z() - position) <= m_planeThreshold) {
                    point = QPointF(vertex.x(), vertex.y());
                    onPlane = true;
                }
                break;
            case YZ_PLANE:
                if (std::abs(vertex.x() - position) <= m_planeThreshold) {
                    point = QPointF(vertex.y(), vertex.z());
                    onPlane = true;
                }
                break;
            case XZ_PLANE:
                if (std::abs(vertex.y() - position) <= m_planeThreshold) {
                    point = QPointF(vertex.x(), vertex.z());
                    onPlane = true;
                }
                break;
            default:
                break;
            }

            if (onPlane) {
                points.append(point);
                values.append(value);
            }
        }

        return points;
    }

signals:
    void pointSizeChanged(float size);
    void xyPlanePositionChanged(float position);
    void yzPlanePositionChanged(float position);
    void xzPlanePositionChanged(float position);
    void planePositionChanged(PlaneType type, float position);

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);

        // �������ɫ��
        m_pointProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
            "#version 330 core\n"
            "layout(location = 0) in vec3 position;\n"
            "layout(location = 1) in float value;\n"
            "uniform mat4 mvp_matrix;\n"
            "uniform float pointSize;\n"
            "uniform float minValue;\n"
            "uniform float maxValue;\n"
            "uniform float xyPlanePos;\n"
            "uniform float yzPlanePos;\n"
            "uniform float xzPlanePos;\n"
            "uniform int selectedPlane;\n"
            "uniform float planeThreshold;\n"
            "out float fragValue;\n"
            "void main() {\n"
            "    bool visible = true;\n"
            "    \n"
            "    // ����ѡ���ƽ�����ͽ��вü�\n"
            "    if (selectedPlane == 1) { // XYƽ��\n"
            "        if (abs(position.z - xyPlanePos) > planeThreshold) visible = false;\n"
            "    } else if (selectedPlane == 2) { // YZƽ��\n"
            "        if (abs(position.x - yzPlanePos) > planeThreshold) visible = false;\n"
            "    } else if (selectedPlane == 3) { // XZƽ��\n"
            "        if (abs(position.y - xzPlanePos) > planeThreshold) visible = false;\n"
            "    }\n"
            "    \n"
            "    if (visible) {\n"
            "        gl_Position = mvp_matrix * vec4(position, 1.0);\n"
            "        gl_PointSize = pointSize;\n"
            "        fragValue = (value - minValue) / (maxValue - minValue);\n"
            "    } else {\n"
            "        gl_Position = vec4(0.0, 0.0, 0.0, 0.0);\n"
            "        gl_PointSize = 0.0;\n"
            "    }\n"
            "}");

        m_pointProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
            "#version 330 core\n"
            "in float fragValue;\n"
            "out vec4 color;\n"
            "vec4 jetColormap(float value) {\n"
            "    if (value < 0.0) return vec4(0.0, 0.0, 0.5, 1.0);\n"
            "    else if (value < 0.25) return vec4(0.0, 0.0, 1.0, 1.0);\n"
            "    else if (value < 0.5) return vec4(0.0, 1.0, 1.0, 1.0);\n"
            "    else if (value < 0.75) return vec4(1.0, 1.0, 0.0, 1.0);\n"
            "    else if (value <= 1.0) return vec4(1.0, 0.0, 0.0, 1.0);\n"
            "    else return vec4(0.5, 0.0, 0.0, 1.0);\n"
            "}\n"
            "void main() {\n"
            "    color = jetColormap(fragValue);\n"
            "}");
        m_pointProgram.link();

        // ������ɫ����������ɫ��������ʾ��
        m_shadedProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
            "#version 330 core\n"
            "layout(location = 0) in vec3 position;\n"
            "layout(location = 1) in vec3 normal;\n"
            "layout(location = 2) in float value;\n"
            "uniform mat4 mvp_matrix;\n"
            "uniform mat4 model_matrix;\n"
            "uniform vec3 lightPos;\n"
            "uniform float minValue;\n"
            "uniform float maxValue;\n"
            "uniform float xyPlanePos;\n"
            "uniform float yzPlanePos;\n"
            "uniform float xzPlanePos;\n"
            "uniform int selectedPlane;\n"
            "uniform float planeThreshold;\n"
            "out vec3 FragPos;\n"
            "out vec3 Normal;\n"
            "out vec3 LightPos;\n"
            "out float fragValue;\n"
            "void main() {\n"
            "    bool visible = true;\n"
            "    \n"
            "    // ����ѡ���ƽ�����ͽ��вü�\n"
            "    if (selectedPlane == 1) { // XYƽ��\n"
            "        if (abs(position.z - xyPlanePos) > planeThreshold) visible = false;\n"
            "    } else if (selectedPlane == 2) { // YZƽ��\n"
            "        if (abs(position.x - yzPlanePos) > planeThreshold) visible = false;\n"
            "    } else if (selectedPlane == 3) { // XZƽ��\n"
            "        if (abs(position.y - xzPlanePos) > planeThreshold) visible = false;\n"
            "    }\n"
            "    \n"
            "    if (visible) {\n"
            "        FragPos = vec3(model_matrix * vec4(position, 1.0));\n"
            "        Normal = mat3(transpose(inverse(model_matrix))) * normal;\n"
            "        LightPos = lightPos;\n"
            "        fragValue = (value - minValue) / (maxValue - minValue);\n"
            "        gl_Position = mvp_matrix * vec4(position, 1.0);\n"
            "    } else {\n"
            "        gl_Position = vec4(0.0, 0.0, 0.0, 0.0);\n"
            "    }\n"
            "}");

        m_shadedProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
            "#version 330 core\n"
            "in vec3 FragPos;\n"
            "in vec3 Normal;\n"
            "in vec3 LightPos;\n"
            "in float fragValue;\n"
            "out vec4 color;\n"
            "vec4 jetColormap(float value) {\n"
            "    if (value < 0.0) return vec4(0.0, 0.0, 0.5, 1.0);\n"
            "    else if (value < 0.25) return vec4(0.0, 0.0, 1.0, 1.0);\n"
            "    else if (value < 0.5) return vec4(0.0, 1.0, 1.0, 1.0);\n"
            "    else if (value < 0.75) return vec4(1.0, 1.0, 0.0, 1.0);\n"
            "    else if (value <= 1.0) return vec4(1.0, 0.0, 0.0, 1.0);\n"
            "    else return vec4(0.5, 0.0, 0.0, 1.0);\n"
            "}\n"
            "void main() {\n"
            "    // ������\n"
            "    float ambientStrength = 0.3;\n"
            "    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);\n"
            "    \n"
            "    // ������\n"
            "    vec3 norm = normalize(Normal);\n"
            "    vec3 lightDir = normalize(LightPos - FragPos);\n"
            "    float diff = max(dot(norm, lightDir), 0.0);\n"
            "    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);\n"
            "    \n"
            "    // ���淴��\n"
            "    float specularStrength = 0.5;\n"
            "    vec3 viewDir = normalize(-FragPos);\n"
            "    vec3 reflectDir = reflect(-lightDir, norm);\n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
            "    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);\n"
            "    \n"
            "    vec4 objectColor = jetColormap(fragValue);\n"
            "    vec3 result = (ambient + diffuse + specular) * vec3(objectColor);\n"
            "    color = vec4(result, 1.0);\n"
            "}");
        m_shadedProgram.link();

        // ���õ�������
        setupPointCloud();
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
        m_projection.setToIdentity();
        m_projection.perspective(45.0f, float(w) / h, 0.1f, 100.0f);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ����ģ����ͼ����
        QMatrix4x4 view;
        view.translate(0.0f, 0.0f, -5.0f);
        view.rotate(m_rotation);

        QMatrix4x4 model;
        model.setToIdentity();

        QMatrix4x4 mvp = m_projection * view * model;

        // ������ʾģʽѡ����Ʒ�ʽ
        if (m_displayMode == POINTS_MODE) {
            // ���Ƶ���
            m_pointProgram.bind();
            m_pointProgram.setUniformValue("mvp_matrix", mvp);
            m_pointProgram.setUniformValue("pointSize", m_pointSize);
            m_pointProgram.setUniformValue("minValue", m_cloud.minValue);
            m_pointProgram.setUniformValue("maxValue", m_cloud.maxValue);
            m_pointProgram.setUniformValue("xyPlanePos", m_xyPlanePos);
            m_pointProgram.setUniformValue("yzPlanePos", m_yzPlanePos);
            m_pointProgram.setUniformValue("xzPlanePos", m_xzPlanePos);
            m_pointProgram.setUniformValue("selectedPlane", static_cast<int>(m_selectedPlane));
            m_pointProgram.setUniformValue("planeThreshold", m_planeThreshold);

            m_vao.bind();
            glDrawArrays(GL_POINTS, 0, m_cloud.vertices.size());
            m_vao.release();
            m_pointProgram.release();
        }
        else if (m_displayMode == SHADED_MODE && m_cloud.hasNormals) {
            // ������ɫ����
            m_shadedProgram.bind();
            m_shadedProgram.setUniformValue("mvp_matrix", mvp);
            m_shadedProgram.setUniformValue("model_matrix", model);
            m_shadedProgram.setUniformValue("lightPos", m_lightPosition);
            m_shadedProgram.setUniformValue("minValue", m_cloud.minValue);
            m_shadedProgram.setUniformValue("maxValue", m_cloud.maxValue);
            m_shadedProgram.setUniformValue("xyPlanePos", m_xyPlanePos);
            m_shadedProgram.setUniformValue("yzPlanePos", m_yzPlanePos);
            m_shadedProgram.setUniformValue("xzPlanePos", m_xzPlanePos);
            m_shadedProgram.setUniformValue("selectedPlane", static_cast<int>(m_selectedPlane));
            m_shadedProgram.setUniformValue("planeThreshold", m_planeThreshold);

            m_vao.bind();
            glDrawArrays(GL_POINTS, 0, m_cloud.vertices.size());
            m_vao.release();
            m_shadedProgram.release();
        }
        else if (m_displayMode == MESH_MODE && m_cloud.hasIndices) {
            // ��������
            m_shadedProgram.bind();
            m_shadedProgram.setUniformValue("mvp_matrix", mvp);
            m_shadedProgram.setUniformValue("model_matrix", model);
            m_shadedProgram.setUniformValue("lightPos", m_lightPosition);
            m_shadedProgram.setUniformValue("minValue", m_cloud.minValue);
            m_shadedProgram.setUniformValue("maxValue", m_cloud.maxValue);
            m_shadedProgram.setUniformValue("xyPlanePos", m_xyPlanePos);
            m_shadedProgram.setUniformValue("yzPlanePos", m_yzPlanePos);
            m_shadedProgram.setUniformValue("xzPlanePos", m_xzPlanePos);
            m_shadedProgram.setUniformValue("selectedPlane", static_cast<int>(m_selectedPlane));
            m_shadedProgram.setUniformValue("planeThreshold", m_planeThreshold);

            m_vao.bind();
            glDrawElements(GL_TRIANGLES, m_cloud.indices.size(), GL_UNSIGNED_INT, 0);
            m_vao.release();
            m_shadedProgram.release();
        }

        // ����������
        drawCoordinateAxes(view);
    }

    void mousePressEvent(QMouseEvent* event) override {
        m_lastPos = event->pos();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        int dx = event->x() - m_lastPos.x();
        int dy = event->y() - m_lastPos.y();

        if (event->buttons() & Qt::LeftButton) {
            QVector3D axis = QVector3D(dy, dx, 0.0f).normalized();
            float angle = QVector3D(dy, dx, 0.0f).length() * 0.5f;
            m_rotation = QQuaternion::fromAxisAndAngle(axis, angle) * m_rotation;
            update();
        }
        else if (event->buttons() & Qt::RightButton) {
            // ������Դλ��
            m_lightPosition.setX(m_lightPosition.x() + dx * 0.01f);
            m_lightPosition.setY(m_lightPosition.y() - dy * 0.01f);
            update();
        }
        m_lastPos = event->pos();
    }

    void wheelEvent(QWheelEvent* event) override {
        m_pointSize += event->angleDelta().y() * 0.01f;
        m_pointSize = std::max(0.5f, std::min(m_pointSize, 10.0f));
        emit pointSizeChanged(m_pointSize);
        update();
    }

private:
    void generateSampleData() {
        // ����ʾ������ (�����ϵĵ�)
        const long numPoints = IESLoader::instance().intensity.size();
        m_cloud.vertices.resize(numPoints);
        m_cloud.normals.resize(numPoints);
        m_cloud.values.resize(numPoints);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        m_cloud.minValue = FLT_MAX;
        m_cloud.maxValue = -FLT_MAX;

        for (long i = 0; i < numPoints; ++i) {           

            m_cloud.vertices[i] = IESLoader::instance().coords[i];
            // ���㷨�� (��ԭ�㵽��ķ���)
            m_cloud.normals[i] = m_cloud.vertices[i].normalized();

            m_cloud.values[i] = IESLoader::instance().intensity[i];

            m_cloud.minValue = std::min(m_cloud.minValue, m_cloud.values[i]);
            m_cloud.maxValue = std::max(m_cloud.maxValue, m_cloud.values[i]);
        }

        m_cloud.hasNormals = true;

        // ���ɼ򵥵��������� (��������ʾ)
        //generateMeshIndices();
        m_cloud.hasIndices = true;
    }

    void generateMeshIndices() {
        // �򵥵����������㷨 (��������ʾ)
        // ʵ��Ӧ����Ӧ��ʹ�ø����ӵ��㷨��Poisson�ؽ���Delaunay���ǻ�

        const int gridSize = 20;
        const int numPoints = gridSize * gridSize;

        // ������������
        QVector<QVector3D> gridVertices;
        QVector<QVector3D> gridNormals;
        QVector<float> gridValues;

        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                float x = (i / float(gridSize - 1)) * 2.0f - 1.0f;
                float y = (j / float(gridSize - 1)) * 2.0f - 1.0f;
                float z = sin(x * M_PI) * cos(y * M_PI) * 0.5f;

                gridVertices.append(QVector3D(x, y, z));
                gridNormals.append(QVector3D(-cos(x * M_PI) * sin(y * M_PI) * M_PI * 0.5f,
                    -sin(x * M_PI) * sin(y * M_PI) * M_PI * 0.5f,
                    1.0f).normalized());

                float value = QVector3D(x, y, z).length();
                gridValues.append(value);
            }
        }

        // ������������
        for (int i = 0; i < gridSize - 1; ++i) {
            for (int j = 0; j < gridSize - 1; ++j) {
                int topLeft = i * gridSize + j;
                int topRight = i * gridSize + j + 1;
                int bottomLeft = (i + 1) * gridSize + j;
                int bottomRight = (i + 1) * gridSize + j + 1;

                // ��һ��������
                m_cloud.indices.append(topLeft);
                m_cloud.indices.append(bottomLeft);
                m_cloud.indices.append(topRight);

                // �ڶ���������
                m_cloud.indices.append(topRight);
                m_cloud.indices.append(bottomLeft);
                m_cloud.indices.append(bottomRight);
            }
        }

        // ������������ӵ������� (��������ʾ����ģʽ)
        m_cloud.vertices = gridVertices;
        m_cloud.normals = gridNormals;
        m_cloud.values = gridValues;
    }

    void setupPointCloud() {
        m_vao.create();
        m_vao.bind();

        // �������㻺����
        m_vbo.create();
        m_vbo.bind();
        m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

        // �����������ݻ����� (λ�� + ���� + ֵ)
        QVector<float> vertexData;
        for (int i = 0; i < m_cloud.vertices.size(); ++i) {
            vertexData << m_cloud.vertices[i].x()
                << m_cloud.vertices[i].y()
                << m_cloud.vertices[i].z()
                << m_cloud.normals[i].x()
                << m_cloud.normals[i].y()
                << m_cloud.normals[i].z()
                << m_cloud.values[i];
        }

        m_vbo.allocate(vertexData.constData(), vertexData.size() * sizeof(float));

        // ���ö�������
        m_pointProgram.bind();
        m_pointProgram.enableAttributeArray(0);
        m_pointProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 7 * sizeof(float));

        m_pointProgram.enableAttributeArray(1);
        m_pointProgram.setAttributeBuffer(1, GL_FLOAT, 6 * sizeof(float), 1, 7 * sizeof(float));

        m_shadedProgram.bind();
        m_shadedProgram.enableAttributeArray(0);
        m_shadedProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 7 * sizeof(float));

        m_shadedProgram.enableAttributeArray(1);
        m_shadedProgram.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 7 * sizeof(float));

        m_shadedProgram.enableAttributeArray(2);
        m_shadedProgram.setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(float), 1, 7 * sizeof(float));

        // �������������� (����������ʾ)
        if (m_cloud.hasIndices) {
            m_ibo.create();
            m_ibo.bind();
            m_ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
            m_ibo.allocate(m_cloud.indices.constData(), m_cloud.indices.size() * sizeof(unsigned int));
        }

        m_vao.release();
        m_vbo.release();
        if (m_cloud.hasIndices) {
            m_ibo.release();
        }
    }

    void drawCoordinateAxes(const QMatrix4x4& view) {
        // ��ʱʹ�ù̶����߻���������
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(m_projection.data());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(view.data());

        glLineWidth(2.0f);
        glBegin(GL_LINES);

        // X�� - ��ɫ
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.5f, 0.0f, 0.0f);

        // Y�� - ��ɫ
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.5f, 0.0f);

        // Z�� - ��ɫ
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.5f);

        glEnd();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

private:
    PointCloud m_cloud;

    QOpenGLShaderProgram m_pointProgram;
    QOpenGLShaderProgram m_shadedProgram;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo{ QOpenGLBuffer::VertexBuffer };
    QOpenGLBuffer m_nbo{ QOpenGLBuffer::VertexBuffer };
    QOpenGLBuffer m_ibo{ QOpenGLBuffer::IndexBuffer };

    QMatrix4x4 m_projection;
    QQuaternion m_rotation;
    QPoint m_lastPos;

    float m_pointSize;
    float m_xyPlanePos, m_yzPlanePos, m_xzPlanePos;
    PlaneType m_selectedPlane;
    float m_planeThreshold;
    DisplayMode m_displayMode;
    QVector3D m_lightPosition;
};

// ʹ��QCustomPlot����ƽ�����ݵ����
class PlaneDataPlotter : public QWidget {
    Q_OBJECT
public:
    PlaneDataPlotter(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);

        // ��������QCustomPlot������ʾ����ƽ�������
        m_xyPlot = new QCustomPlot;
        m_yzPlot = new QCustomPlot;
        m_xzPlot = new QCustomPlot;

        // ����ͼ�����
        m_xyPlot->plotLayout()->insertRow(0);
        m_xyPlot->plotLayout()->addElement(0, 0, new QCPTextElement(m_xyPlot, "XY Plane", QFont("sans", 12, QFont::Bold)));

        m_yzPlot->plotLayout()->insertRow(0);
        m_yzPlot->plotLayout()->addElement(0, 0, new QCPTextElement(m_yzPlot, "YZ Plane", QFont("sans", 12, QFont::Bold)));

        m_xzPlot->plotLayout()->insertRow(0);
        m_xzPlot->plotLayout()->addElement(0, 0, new QCPTextElement(m_xzPlot, "XZ Plane", QFont("sans", 12, QFont::Bold)));

        // ������ɫӳ��
        setupColorMap(m_xyPlot);
        setupColorMap(m_yzPlot);
        setupColorMap(m_xzPlot);

        // ��ӵ�����
        layout->addWidget(m_xyPlot);
        layout->addWidget(m_yzPlot);
        layout->addWidget(m_xzPlot);

        setLayout(layout);
    }

    void updatePlaneData(PlaneType type, const QVector<QPointF>& points, const QVector<float>& values, float minValue, float maxValue) {
        QCustomPlot* plot = nullptr;

        switch (type) {
        case XY_PLANE:
            plot = m_xyPlot;
            break;
        case YZ_PLANE:
            plot = m_yzPlot;
            break;
        case XZ_PLANE:
            plot = m_xzPlot;
            break;
        default:
            return;
        }

        if (!plot) return;

        // ��ȡ��ɫӳ��
        QCPColorMap* colorMap = qobject_cast<QCPColorMap*>(plot->plottable(0));
        if (!colorMap) return;

        // �����������
        colorMap->data()->clear();

        // �������ݷ�Χ
        if (!points.isEmpty()) {
            // �������ݷ�Χ
            double xMin = std::numeric_limits<double>::max();
            double xMax = std::numeric_limits<double>::lowest();
            double yMin = std::numeric_limits<double>::max();
            double yMax = std::numeric_limits<double>::lowest();

            for (const QPointF& point : points) {
                xMin = std::min(xMin, static_cast<double>(point.x()));
                xMax = std::max(xMax, static_cast<double>(point.x()));
                yMin = std::min(yMin, static_cast<double>(point.y()));
                yMax = std::max(yMax, static_cast<double>(point.y()));
            }

            // �������ݷ�Χ
            int nx = 100, ny = 100; // ����ֱ���
            colorMap->data()->setSize(nx, ny);
            colorMap->data()->setRange(QCPRange(xMin, xMax), QCPRange(yMin, yMax));

            // ������������
            double dx = (xMax - xMin) / (nx - 1);
            double dy = (yMax - yMin) / (ny - 1);

            // ʹ������ڲ�ֵ�������
            for (int xIndex = 0; xIndex < nx; ++xIndex) {
                for (int yIndex = 0; yIndex < ny; ++yIndex) {
                    double x = xMin + xIndex * dx;
                    double y = yMin + yIndex * dy;

                    // �ҵ�����ĵ��ֵ
                    double minDist = std::numeric_limits<double>::max();
                    float value = 0;

                    for (int i = 0; i < points.size(); ++i) {
                        double dist = std::pow(points[i].x() - x, 2) + std::pow(points[i].y() - y, 2);
                        if (dist < minDist) {
                            minDist = dist;
                            value = values[i];
                        }
                    }

                    colorMap->data()->setCell(xIndex, yIndex, value);
                }
            }

            // ����������ɫӳ������ݷ�Χ
            colorMap->setDataRange(QCPRange(minValue, maxValue));

            // ���»���
            plot->replot();
        }
    }

private:
    void setupColorMap(QCustomPlot* plot) {
        // ������ɫӳ��
        QCPColorMap* colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);
        //plot->(colorMap);

        // ������ɫ����
        QCPColorGradient gradient;
        gradient.setColorStopAt(0.0, QColor(0, 0, 255));   // ��ɫ
        gradient.setColorStopAt(0.25, QColor(0, 255, 255)); // ��ɫ
        gradient.setColorStopAt(0.5, QColor(0, 255, 0));   // ��ɫ
        gradient.setColorStopAt(0.75, QColor(255, 255, 0)); // ��ɫ
        gradient.setColorStopAt(1.0, QColor(255, 0, 0));   // ��ɫ
        colorMap->setGradient(gradient);

        // ������ɫ����
        QCPColorScale* colorScale = new QCPColorScale(plot);
        plot->plotLayout()->addElement(1, 1, colorScale);
        colorScale->setType(QCPAxis::atRight);
        colorMap->setColorScale(colorScale);
        colorScale->axis()->setLabel("Value");

        // �������ǩ
        plot->xAxis->setLabel("X");
        plot->yAxis->setLabel("Y");

        // ���ý���
        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        // ��ʼ���ݷ�Χ
        colorMap->data()->setSize(100, 100);
        colorMap->data()->setRange(QCPRange(-1, 1), QCPRange(-1, 1));
        colorMap->setDataRange(QCPRange(0, 1));

        // ��ʼ���һЩ����
        for (int x = 0; x < 100; ++x) {
            for (int y = 0; y < 100; ++y) {
                colorMap->data()->setCell(x, y, 0);
            }
        }

        // ����������
        plot->rescaleAxes();
    }

private:
    QCustomPlot* m_xyPlot;
    QCustomPlot* m_yzPlot;
    QCustomPlot* m_xzPlot;
};

//// ������
//class MainWindow : public QMainWindow {
//    Q_OBJECT
//public:
//    MainWindow() {
//        // �������벿���Ͳ���
//        QWidget* centralWidget = new QWidget;
//        QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
//
//        // ���������壨OpenGL��ͼ�Ϳ�����壩
//        QWidget* leftPanel = new QWidget;
//        QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
//
//        // ����OpenGL��ͼ
//        m_viewer = new PointCloudViewer;
//        leftLayout->addWidget(m_viewer, 3);
//
//        // �����������
//        QWidget* controlPanel = createControlPanel();
//        leftLayout->addWidget(controlPanel, 1);
//
//        // �����Ҳ���壨ƽ������ͼ��
//        m_planePlotter = new PlaneDataPlotter;
//
//        // ��ӵ�������
//        mainLayout->addWidget(leftPanel, 2);
//        mainLayout->addWidget(m_planePlotter, 1);
//
//        setCentralWidget(centralWidget);
//
//        // ��ʼ����ɫ����Χ
//        m_colorBar->setRange(m_viewer->getMinValue(), m_viewer->getMaxValue());
//
//        // �����źźͲ�
//        connect(m_viewer, &PointCloudViewer::planePositionChanged, this, &MainWindow::updatePlaneData);
//
//        // ��ʼ����ƽ������
//        updateAllPlaneData();
//    }
//
//private:
//    QWidget* createControlPanel() {
//        QWidget* panel = new QWidget;
//        QVBoxLayout* layout = new QVBoxLayout(panel);
//        layout->setAlignment(Qt::AlignTop);
//
//        // ��ɫ��
//        m_colorBar = new ColorBar;
//        layout->addWidget(m_colorBar);
//
//        // ��ʾģʽ����
//        QGroupBox* displayModeGroup = new QGroupBox(QStringLiteral("��ʾģʽ"));
//        QVBoxLayout* displayModeLayout = new QVBoxLayout(displayModeGroup);
//
//        QRadioButton* pointsModeRadio = new QRadioButton(QStringLiteral("��ģʽ"));
//        QRadioButton* shadedModeRadio = new QRadioButton(QStringLiteral("��ɫģʽ"));
//        QRadioButton* meshModeRadio = new QRadioButton(QStringLiteral("����ģʽ"));
//
//        pointsModeRadio->setChecked(true);
//
//        QButtonGroup* displayModeGroupBtn = new QButtonGroup(this);
//        displayModeGroupBtn->addButton(pointsModeRadio, 0);
//        displayModeGroupBtn->addButton(shadedModeRadio, 1);
//        displayModeGroupBtn->addButton(meshModeRadio, 2);
//
//        connect(displayModeGroupBtn, QOverload<int>::of(&QButtonGroup::idClicked), this, [this](int id) {
//            m_viewer->setDisplayMode(static_cast<DisplayMode>(id));
//            });
//
//        displayModeLayout->addWidget(pointsModeRadio);
//        displayModeLayout->addWidget(shadedModeRadio);
//        displayModeLayout->addWidget(meshModeRadio);
//        layout->addWidget(displayModeGroup);
//
//        // ���С����
//        QGroupBox* pointSizeGroup = new QGroupBox(QStringLiteral("���С"));
//        QVBoxLayout* pointSizeLayout = new QVBoxLayout(pointSizeGroup);
//
//        QSlider* pointSizeSlider = new QSlider(Qt::Horizontal);
//        pointSizeSlider->setRange(1, 100);
//        pointSizeSlider->setValue(30);
//        connect(pointSizeSlider, &QSlider::valueChanged, this, [this](int value) {
//            m_viewer->setPointSize(value / 10.0f);
//            });
//        connect(m_viewer, &PointCloudViewer::pointSizeChanged, this, [pointSizeSlider](float size) {
//            pointSizeSlider->setValue(size * 10);
//            });
//
//        pointSizeLayout->addWidget(pointSizeSlider);
//        layout->addWidget(pointSizeGroup);
//
//        // ƽ��ѡ�����
//        QGroupBox* planeSelectGroup = new QGroupBox(QStringLiteral("ƽ��ѡ��"));
//        QVBoxLayout* planeSelectLayout = new QVBoxLayout(planeSelectGroup);
//
//        QRadioButton* noPlaneRadio = new QRadioButton(QStringLiteral("��ƽ��"));
//        QRadioButton* xyPlaneRadio = new QRadioButton(QStringLiteral("XYƽ��"));
//        QRadioButton* yzPlaneRadio = new QRadioButton(QStringLiteral("YZƽ��"));
//        QRadioButton* xzPlaneRadio = new QRadioButton(QStringLiteral("XZƽ��"));
//
//        noPlaneRadio->setChecked(true);
//
//        QButtonGroup* planeGroup = new QButtonGroup(this);
//        planeGroup->addButton(noPlaneRadio, 0);
//        planeGroup->addButton(xyPlaneRadio, 1);
//        planeGroup->addButton(yzPlaneRadio, 2);
//        planeGroup->addButton(xzPlaneRadio, 3);
//
//        connect(planeGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [this](int id) {
//            m_viewer->setSelectedPlane(static_cast<PlaneType>(id));
//            });
//
//        planeSelectLayout->addWidget(noPlaneRadio);
//        planeSelectLayout->addWidget(xyPlaneRadio);
//        planeSelectLayout->addWidget(yzPlaneRadio);
//        planeSelectLayout->addWidget(xzPlaneRadio);
//        layout->addWidget(planeSelectGroup);
//
//        // ƽ����ֵ����
//        QGroupBox* thresholdGroup = new QGroupBox(QStringLiteral("ƽ����ֵ"));
//        QVBoxLayout* thresholdLayout = new QVBoxLayout(thresholdGroup);
//
//        QSlider* thresholdSlider = new QSlider(Qt::Horizontal);
//        thresholdSlider->setRange(1, 100);
//        thresholdSlider->setValue(5);
//        connect(thresholdSlider, &QSlider::valueChanged, this, [this](int value) {
//            m_viewer->setPlaneThreshold(value / 100.0f);
//            });
//
//        thresholdLayout->addWidget(thresholdSlider);
//        layout->addWidget(thresholdGroup);
//
//        // XYƽ�����
//        QGroupBox* xyPlaneGroup = new QGroupBox(QStringLiteral("XYƽ��λ��"));
//        QVBoxLayout* xyPlaneLayout = new QVBoxLayout(xyPlaneGroup);
//
//        QSlider* xyPlaneSlider = new QSlider(Qt::Horizontal);
//        xyPlaneSlider->setRange(-100, 100);
//        xyPlaneSlider->setValue(0);
//        connect(xyPlaneSlider, &QSlider::valueChanged, this, [this](int value) {
//            m_viewer->setPlanePosition(XY_PLANE, value / 100.0f);
//            });
//        connect(m_viewer, &PointCloudViewer::xyPlanePositionChanged, this, [xyPlaneSlider](float pos) {
//            xyPlaneSlider->setValue(pos * 100);
//            });
//
//        xyPlaneLayout->addWidget(xyPlaneSlider);
//        layout->addWidget(xyPlaneGroup);
//
//        // YZƽ�����
//        QGroupBox* yzPlaneGroup = new QGroupBox(QStringLiteral("YZƽ��λ��"));
//        QVBoxLayout* yzPlaneLayout = new QVBoxLayout(yzPlaneGroup);
//
//        QSlider* yzPlaneSlider = new QSlider(Qt::Horizontal);
//        yzPlaneSlider->setRange(-100, 100);
//        yzPlaneSlider->setValue(0);
//        connect(yzPlaneSlider, &QSlider::valueChanged, this, [this](int value) {
//            m_viewer->setPlanePosition(YZ_PLANE, value / 100.0f);
//            });
//        connect(m_viewer, &PointCloudViewer::yzPlanePositionChanged, this, [yzPlaneSlider](float pos) {
//            yzPlaneSlider->setValue(pos * 100);
//            });
//
//        yzPlaneLayout->addWidget(yzPlaneSlider);
//        layout->addWidget(yzPlaneGroup);
//
//        // XZƽ�����
//        QGroupBox* xzPlaneGroup = new QGroupBox(QStringLiteral("XZƽ��λ��"));
//        QVBoxLayout* xzPlaneLayout = new QVBoxLayout(xzPlaneGroup);
//
//        QSlider* xzPlaneSlider = new QSlider(Qt::Horizontal);
//        xzPlaneSlider->setRange(-100, 100);
//        xzPlaneSlider->setValue(0);
//        connect(xzPlaneSlider, &QSlider::valueChanged, this, [this](int value) {
//            m_viewer->setPlanePosition(XZ_PLANE, value / 100.0f);
//            });
//        connect(m_viewer, &PointCloudViewer::xzPlanePositionChanged, this, [xzPlaneSlider](float pos) {
//            xzPlaneSlider->setValue(pos * 100);
//            });
//
//        xzPlaneLayout->addWidget(xzPlaneSlider);
//        layout->addWidget(xzPlaneGroup);
//
//        // ��Դλ�ÿ���
//        QGroupBox* lightGroup = new QGroupBox(QStringLiteral("��Դλ��"));
//        QVBoxLayout* lightLayout = new QVBoxLayout(lightGroup);
//
//        QLabel* lightInfo = new QLabel(QStringLiteral("�Ҽ��϶�������Դλ��"));
//        lightLayout->addWidget(lightInfo);
//
//        layout->addWidget(lightGroup);
//
//        // ����ƽ�����ݰ�ť
//        QPushButton* updateButton = new QPushButton(QStringLiteral("����ƽ������"));
//        connect(updateButton, &QPushButton::clicked, this, &MainWindow::updateAllPlaneData);
//        layout->addWidget(updateButton);
//
//        layout->addStretch();
//
//        return panel;
//    }
//
//private slots:
//    void updatePlaneData(PlaneType type, float position) {
//        QVector<float> values;
//        QVector<QPointF> points = m_viewer->getPlanePointsWithValues(type, position, values);
//
//        if (!points.isEmpty()) {
//            m_planePlotter->updatePlaneData(type, points, values,
//                m_viewer->getMinValue(), m_viewer->getMaxValue());
//        }
//    }
//
//    void updateAllPlaneData() {
//        updatePlaneData(XY_PLANE, m_viewer->getXYPlanePosition());
//        updatePlaneData(YZ_PLANE, m_viewer->getYZPlanePosition());
//        updatePlaneData(XZ_PLANE, m_viewer->getXZPlanePosition());
//    }
//
//private:
//    PointCloudViewer* m_viewer;
//    ColorBar* m_colorBar;
//    PlaneDataPlotter* m_planePlotter;
//};


//#ifndef OPENGLWIDGET_H
//#define OPENGLWIDGET_H
//
//#include <QOpenGLWidget>
//#include <QOpenGLFunctions>
//#include <QOpenGLShaderProgram>
//#include <QOpenGLBuffer>
//#include <QOpenGLVertexArrayObject>
//#include <QMatrix4x4>
//#include <QVector3D>
//#include <QVector2D>
//#include <QColor>
//#include <QLinearGradient>
//#include <QPainter>
//#include <QMouseEvent>
//#include <QWheelEvent>
//
//// ColorBar ���
//class ColorBar : public QWidget {
//    Q_OBJECT
//public:
//    ColorBar(QWidget* parent = nullptr) : QWidget(parent), m_minValue(0), m_maxValue(1) {
//        setFixedWidth(40);
//        setMinimumHeight(200);
//    }
//
//    void setRange(float min, float max) {
//        m_minValue = min;
//        m_maxValue = max;
//        update();
//    }
//
//protected:
//    void paintEvent(QPaintEvent*) override {
//        QPainter painter(this);
//        painter.setRenderHint(QPainter::Antialiasing);
//
//        // ������ɫ��
//        QLinearGradient gradient(0, height(), 0, 0);
//        gradient.setColorAt(0.0, QColor(0, 0, 255));   // ��ɫ
//        gradient.setColorAt(0.25, QColor(0, 255, 255)); // ��ɫ
//        gradient.setColorAt(0.5, QColor(0, 255, 0));   // ��ɫ
//        gradient.setColorAt(0.75, QColor(255, 255, 0)); // ��ɫ
//        gradient.setColorAt(1.0, QColor(255, 0, 0));   // ��ɫ
//
//        painter.fillRect(10, 10, 20, height() - 20, gradient);
//
//        // ���ƿ̶�
//        painter.setPen(Qt::white);
//        QFont font;
//        font.setPointSize(8);
//        painter.setFont(font);
//
//        float range = m_maxValue - m_minValue;
//        for (int i = 0; i <= 10; ++i) {
//            float value = m_minValue + range * i / 10.0f;
//            int y = height() - 20 - (height() - 40) * i / 10 + 10;
//            painter.drawLine(30, y, 35, y);
//            painter.drawText(36, y + 4, QString::number(value, 'f', 2));
//        }
//    }
//
//private:
//    float m_minValue, m_maxValue;
//};
//
//class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
//{
//    Q_OBJECT
//
//public:
//    explicit OpenGLWidget(QWidget* parent = nullptr);
//    ~OpenGLWidget();
//
//    void setData(const QVector<QVector3D>& points, const QVector<float>& values);
//
//protected:
//    void initializeGL() override;
//    void resizeGL(int w, int h) override;
//    void paintGL() override;
//
//    void mousePressEvent(QMouseEvent* event) override;
//    void mouseMoveEvent(QMouseEvent* event) override;
//    void wheelEvent(QWheelEvent* event) override;
//
//private:
//    void drawGrids();
//    void drawColorBar();
//    void setupShaderProgram(QOpenGLShaderProgram& program, const QString& vertexShader, const QString& fragmentShader);
//    QColor getColorFromValue(float value);
//
//    // �������
//    float gridSize;
//    int gridDivisions;
//
//    // ��ͼ����
//    QMatrix4x4 projection;
//    QMatrix4x4 view;
//    QMatrix4x4 model;
//    QVector3D cameraPosition;
//    QVector3D cameraTarget;
//    float cameraDistance;
//    QPoint lastMousePos;
//
//    // ��ɫ������
//    QOpenGLShaderProgram gridShaderProgram;
//    QOpenGLShaderProgram colorBarShaderProgram;
//
//    // ���㻺����
//    QOpenGLBuffer gridVBO;
//    QOpenGLVertexArrayObject gridVAO;
//
//    // ����
//    QVector<QVector3D> dataPoints;
//    QVector<float> dataValues;
//    float minValue, maxValue;
//};
//
//#endif // OPENGLWIDGET_H