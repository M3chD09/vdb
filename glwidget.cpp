#include "glwidget.h"

#include <cmath>
#include <fstream>
#include <iostream>

GLWidget::GLWidget()
{
    timerUpdate = new QTimer(this);
    timerCal = new QTimer(this);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    delete program;
    delete vshader;
    delete fshader;
    delete timerUpdate;
    delete timerCal;
    vbo.destroy();
    vao.destroy();
    doneCurrent();
}

void GLWidget::updateTopology()
{
    if (!tool.moveToNextPosture()) {
        timerUpdate->stop();
        timerCal->stop();
        return;
    }
    auto isInside = [&](const Vector3D<float>& p) {
        return tool.isInside(p);
    };
    topology.subtract(tool.getBBox(), isInside);
}

void GLWidget::calTopology()
{
    topology.calculateVoxels(coords, sizes);
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);

    program = new QOpenGLShaderProgram;
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shader.geom");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag");
    program->link();

    locModel = program->uniformLocation("model");
    locView = program->uniformLocation("view");
    locProjection = program->uniformLocation("projection");

    program->bind();
    program->setUniformValue("objectColor", objectColor);
    program->setUniformValue("lightColor", lightColor);
    program->setUniformValue("lightPos", lightPos);

    topology.calculateVoxels(coords, sizes);
    auto leafCount = (unsigned int)coords.size();

    if (!vao.isCreated()) {
        vao.create();
    }
    vao.bind();
    if (!vbo.isCreated()) {
        vbo.create();
        vbo.bind();
        vbo.allocate(leafCount * 4 * sizeof(float));
    }

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3);
    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, leafCount * 3 * sizeof(float), 1);
    vao.release();
}

void GLWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();

    matModel.setToIdentity();
    matModel.rotate(camera.rotation);
    matModel.rotate(-90.0f, QVector3D(1.0f, 0, 0));
    matModel.rotate(-90.0f, QVector3D(0, 0, 1.0f));
    program->setUniformValue(locModel, matModel);

    matView.setToIdentity();
    matView.lookAt(camera.pos, camera.pos + camera.front, camera.up);
    program->setUniformValue(locView, matView);

    matProjection.setToIdentity();
    matProjection.perspective(camera.fov, qreal(width()) / qreal(height()), 0.1f, 100.f);
    program->setUniformValue(locProjection, matProjection);

    auto leafCount = (unsigned int)coords.size();

    vao.bind();
    vbo.allocate(leafCount * 4 * sizeof(float));
    vbo.write(0, coords.data(), leafCount * 3 * sizeof(float));
    vbo.write(leafCount * 3 * sizeof(float), sizes.data(), leafCount * sizeof(float));
    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3);
    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, leafCount * 3 * sizeof(float), 1);
    glDrawArrays(GL_POINTS, 0, leafCount);
    vao.release();

    program->disableAttributeArray(0);
}

void GLWidget::resizeGL(int w, int h)
{
    matProjection.setToIdentity();
    matProjection.perspective(camera.fov, qreal(w) / qreal(h), 0.1f, 100.f);
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    camera.keyPressEvent(event);

    if (event->key() == Qt::Key_Z) {
        connect(timerUpdate, &QTimer::timeout, this, QOverload<>::of(&GLWidget::updateTopology));
        timerUpdate->start(1);
        connect(timerCal, &QTimer::timeout, this, QOverload<>::of(&GLWidget::calTopology));
        timerCal->start(1);
    }
    if (event->key() == Qt::Key_P) {
        timerUpdate->stop();
        timerCal->stop();
    }
    if (event->key() == Qt::Key_R) {
        timerUpdate->stop();
        timerCal->stop();
        tool.reset();
        topology.initialize();
        topology.calculateVoxels(coords, sizes);
    }

    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    camera.mouseMoveEvent(event);
    update();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    camera.wheelEvent(event);
    update();
}
