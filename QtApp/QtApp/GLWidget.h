#pragma once

#include <QtOpenGL>
#include <QWidget>
#include "qgl.h"
#include "qvector.h"
#include "qmatrix4x4.h"

#include "Object3D.h"

class GLWidget : public QGLWidget
{
	Q_OBJECT
public:
	GLWidget();
	~GLWidget();

	void addObject(Object3D);
	void clearObject();
	void setProjection(bool);
	void setNRange(int);
	void setLightHeight(int);
	void loadTextures(QString);
	void changeTranslateX(GLfloat);
	void changeTranslateY(GLfloat);
	void changeTranslateZ(GLfloat);
	void changeScale(GLfloat);
	void setBG(QString);

	GLfloat translateX, translateY, translateZ;
	GLfloat scale;
	GLfloat rotateX, rotateY, rotateZ;

signals:
	void resizeWindow(int, int);

private:
	bool projection;
	GLfloat nRange;
	GLuint texture[1];
	GLuint BGtexture;
	GLfloat lightHeight;
	QImage img;

	GLfloat lightPosition[4];

	QPoint lastPos;

	QVector<Object3D> objectList;

	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

};

