#include "GLWidget.h"
#include "QMessageBox.h"
#include "LightDetect.h"
#include <glut.h>

GLWidget::GLWidget()
{
	projection = true;
	nRange = 20;
	translateX = 0.0;
	translateY = 0.0;
	translateZ = 0.0;
	scale = 1.0;
	rotateX = 0.0;
	rotateY = 0.0;
	rotateZ = 0.0;
	lightHeight = 30.0;

	lightPosition[0] = 0;
	lightPosition[1] = 0;
	lightPosition[2] = lightHeight;
	lightPosition[3] = 1.0;
}


GLWidget::~GLWidget()
{
}


void GLWidget::addObject(Object3D object)
{
	objectList.clear();
	objectList.push_back(object);
	translateX = 0.0;
	translateY = 0.0;
	translateZ = 0.0;
	scale = 1.0;
	rotateX = 0.0;
	rotateY = 0.0;
	rotateZ = 0.0;
	updateGL();
}


void GLWidget::clearObject()
{
	objectList.clear();
	updateGL();
}


void GLWidget::setProjection(bool flag)
{
	projection = flag;
	resizeGL(width(), height());
}


void GLWidget::setNRange(int range)
{
	nRange = 50.0 * range / 100;
	resizeGL(width(), height());
}


void GLWidget::setLightHeight(int height)
{
	lightHeight = 20.0 * height / 100;
	lightPosition[3] = lightHeight;
	updateGL();
}


void GLWidget::changeTranslateX(GLfloat x)
{
	translateX += x;
	updateGL();
}


void GLWidget::changeTranslateY(GLfloat x)
{
	translateY += x;
	updateGL();
}


void GLWidget::changeTranslateZ(GLfloat x)
{
	translateZ += x;
	updateGL();
}


void GLWidget::changeScale(GLfloat s)
{
	scale += s;
	updateGL();
}


void GLWidget::loadTextures(QString fileName)
{
	QImage tex, buf;
	if (!buf.load(fileName))
	{
		QImage dummy(128, 128, QImage::Format_RGB32);
		dummy.fill(Qt::white);
		buf = dummy;
	}
	tex = QGLWidget::convertToGLFormat(buf);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	updateGL();
}

void GLWidget::initializeGL()
{
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);								//设置阴影平滑模式
	glClearColor(0,0,0,0);								//改变窗口的背景颜色
	glClearDepth(10);									//设置深度缓存
	glEnable(GL_DEPTH_TEST);							//允许深度测试
	glDepthFunc(GL_LEQUAL);								//设置深度测试类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//进行透视校正

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	glEnable(GL_MULTISAMPLE);

	loadTextures("");
}

void GLWidget::setBG(QString fileName)
{
	img.load(fileName);
	if (!img.isNull()){
		img = QGLWidget::convertToGLFormat(img);
		setFixedSize(img.width(), img.height());
		emit resizeWindow(img.width() + 180, img.height() + 100);

		glGenTextures(1, &BGtexture);
		glBindTexture(GL_TEXTURE_2D, BGtexture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		cv::Mat mat(img.height(), img.width(), CV_8UC4, const_cast<uchar*>(img.bits()), img.bytesPerLine());
		cv::Mat mat3b(mat.size(), CV_8UC3);
		cv::cvtColor(mat, mat3b, CV_BGRA2BGR);
		LightDetect detect(mat3b, 3);
		cv::Vec2b lp = detect.getLightDirection();
		lightPosition[0] = lp[0];
		lightPosition[1] = lp[1];

		updateGL();
	}
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.0);      /* up is in postivie Y direction */
	glBindTexture(GL_TEXTURE_2D, BGtexture);
	glBegin(GL_QUADS);
	float step = 0.08;
	float zindex = 0;
	float dx = img.width() / 2;
	float dy = img.height() / 2;
	glTexCoord2f(0,0);
	glVertex3f(-dx*step, -dy*step, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-dx*step, dy*step, 0);
	glTexCoord2f(1,1);
	glVertex3f(dx*step, dy*step, 0);
	glTexCoord2f(1, 0);
	glVertex3f(dx*step, -dy*step, 0);
	glEnd();

	glTranslatef(translateX, translateY, translateZ);
	glScalef(scale, scale, scale);
	glRotatef(rotateX / 16.0, 1.0, 0.0, 0.0);
	glRotatef(rotateY / 16.0, 0.0, 1.0, 0.0);
	glRotatef(rotateZ / 16.0, 0.0, 0.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	for (int i = 0; i < objectList.size(); i++) {
		glBegin(GL_TRIANGLES);
		for (int j = 0; j < objectList[i].getFaceCountTri(); j++) {
			QVector<QVector3D> face = objectList[i].getFaceTri(j);
			for (int k = 0; k < face.size(); k++) {
				glTexCoord2f(0.5f + 0.5f * face[k].x(), 0.5f + 0.5f * face[k].z());
				glVertex3f(face[k].x(), face[k].y(), face[k].z());
			}
		}
		glEnd();

		glBegin(GL_QUADS);
		for (int j = 0; j < objectList[i].getFaceCountQuad(); j++) {
			QVector<QVector3D> face = objectList[i].getFaceQuad(j);
			for (int k = 0; k < face.size(); k++) {
				glTexCoord2f(k == 0 || k == 3 ? 0.0 : 1.0, k == 0 || k == 1 ? 0.0 : 1.0);
				glVertex3f(face[k].x(), face[k].y(), face[k].z());
			}
		}
		glEnd();
	}
	//glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
}

void GLWidget::resizeGL(int width, int height)
{
	if (0 == height)
		height = 1;									//防止一条边为0
	glViewport(0, 0, (GLint)width, (GLint)height);	//重置当前视口，本身不是重置窗口的，只不过是这里被Qt

	glMatrixMode(GL_PROJECTION);					//选择投影矩阵
	glLoadIdentity();								//重置选择好的投影矩阵

	if (projection) {
		if (width <= height)
			glOrtho(-nRange, nRange, -nRange*height / width, nRange*height / width, -nRange, nRange);
		else
			glOrtho(-nRange*width / height, nRange*width / height, -nRange, nRange, -nRange, nRange);
	}
	else {
		GLdouble znear = 0.7, zfar = 10;
		if (width <= height)
			glFrustum(-nRange, nRange, -nRange*height / width, nRange*height / width, znear, zfar);
		else
			glFrustum(-nRange*width / height, nRange*width / height, -nRange, nRange, znear, zfar);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateGL();
}


void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}


void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(rotateX + 8 * dy);
		setYRotation(rotateY + 8 * dx);
	}
	else if (event->buttons() & Qt::RightButton) {
		setXRotation(rotateX + 8 * dy);
		setZRotation(rotateZ + 8 * dx);
	}
	lastPos = event->pos();
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != rotateX) {
		rotateX = angle;
		updateGL();
	}
}

void GLWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != rotateY) {
		rotateY = angle;
		updateGL();
	}
}

void GLWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != rotateZ) {
		rotateZ = angle;
		updateGL();
	}
}
