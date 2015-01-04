#ifndef QTAPP_H
#define QTAPP_H

#include <QtOpenGL>
#include <QWidget>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include "ui_qtapp.h"

#include "GLWidget.h"

#if _MSC_VER >= 1600

       #pragma execution_character_set("utf-8")

 #endif

struct keyframe{
	int frame;
	GLfloat translateX, translateY, translateZ;
	GLfloat scale;
	GLfloat rotateX, rotateY, rotateZ;
};

class QtApp : public QWidget
{
	Q_OBJECT

public:
	explicit QtApp(QWidget *parent = 0);
	~QtApp();
	bool isKeyframe(int n);

	public slots:
	void OpenFile();
	void ClearObject();
	void OrthographicProjection();
	void PerspectiveProjection();
	void loadTextures();
	void nRangeChanged(int);
	void lightHeightChanged(int);
	void chooseBG();
	void resizeWindow(int, int);
	void changeTotalFrames();
	void showFrame(int);
	void prevFrame();
	void nextFrame();
	void prevKeyframe();
	void nextKeyframe();
	void saveKeyframe();
	void delKeyframe();
	void createVideo();

protected:
	void resizeEvent(QResizeEvent * event);
	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *event);

private:
	Ui::QtAppClass ui;
	GLWidget *glWidget;
	QVector<keyframe> keyframes;
};

#endif // QTAPP_H
