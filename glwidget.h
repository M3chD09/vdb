#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "Tool.h"
#include "Topology.h"
#include "Vector3D.h"
#include "camera.h"

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>

#include <cstdint>
#include <vector>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    GLWidget();
    ~GLWidget();

protected:
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;

    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOpenGLShader* vshader = nullptr;
    QOpenGLShader* fshader = nullptr;
    QOpenGLShaderProgram* program = nullptr;

    QVector3D objectColor = QVector3D(0.0f, 0.737f, 0.813f);
    QVector3D lightColor = QVector3D(1.0f, 1.0f, 1.0f);
    QVector3D lightPos = QVector3D(2000.0f, 1200.0f, 1000.0f);

    int locModel;
    int locView;
    int locProjection;
    QMatrix4x4 matModel;
    QMatrix4x4 matView;
    QMatrix4x4 matProjection;

    QTimer* timerUpdate = nullptr;
    QTimer* timerCal = nullptr;

    Camera camera;
    Topology<> topology;
    Tool tool;

    std::vector<Vector3D<float>> coords;
    std::vector<float> sizes;

    void updateTopology(void);
    void calTopology(void);
};

#endif // GLWIDGET_H
