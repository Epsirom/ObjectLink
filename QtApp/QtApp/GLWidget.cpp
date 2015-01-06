#include "GLWidget.h"
#include "QMessageBox.h"
#include "LightDetect.h"
#include <glut.h>

GLWidget::GLWidget()
{
	projection = true;
	nRange = 50;
	translateX = 0.0;
	translateY = 0.0;
	translateZ = 0.0;
	scale = 1.0;
	rotateX = 0.0;
	rotateY = 0.0;
	rotateZ = 0.0;
	lightHeight = nRange * nRange;//-13.0;

	lightPosition[0] = -50;
	lightPosition[1] = -50;
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
	glClearStencil(0);
	glEnable(GL_DEPTH_TEST);							//允许深度测试
	glDepthFunc(GL_LEQUAL);								//设置深度测试类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//进行透视校正

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_RESCALE_NORMAL);


	loadTextures("");

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	GLfloat mat_specular[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat mat_shininess[] = {50.0};
	GLfloat lmodel_ambient[] = {0.3, 0.3, 0.3, 1.0};
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	*/

	GLfloat matAmbient[] = {0.6, 0.6, 0.6, 1.0};
	GLfloat matDiffuse[]   = {0.35, 0.35, 0.35, 1.0};
	GLfloat matAmbDif[]   = {0.5, 0.5, 0.5, 1.0};
	GLfloat matSpecular[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat shine[] = {5.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDif);  // 将背景颜色和散射颜色设置成同一颜色
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);

	GLfloat lightAmbient[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat lightDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightSpecular[4] = {0.0, 0.0, 0.0, 1.0 };
	/*GLfloat posTmp[4] = {};
	posTmp[0] = lightPosition[0] + translateX;
	posTmp[1] = lightPosition[1] + translateY;
	posTmp[2] = lightPosition[2] + translateZ;
	posTmp[3] = lightPosition[3];*/
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv( GL_LIGHT0, GL_AMBIENT, lightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse );
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.0);      /* up is in postivie Y direction */

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
		cv::Vec2s lp = detect.getLightDirection();
		lightPosition[0] = lp[0] * viewWidth;
		lightPosition[1] = lp[1] * viewHeight;
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

		updateGL();
	}
}

enum {
  X, Y, Z, W
};
enum {
  A, B, C, D
};

void
shadowMatrix(GLfloat shadowMat[4][4],
  GLfloat groundplane[4],
  GLfloat lightpos[4])
{
  GLfloat dot;

  /* Find dot product between light position vector and ground plane normal. */
  dot = groundplane[X] * lightpos[X] +
    groundplane[Y] * lightpos[Y] +
    groundplane[Z] * lightpos[Z] +
    groundplane[W] * lightpos[W];

  shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
  shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
  shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
  shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];

  shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
  shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
  shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
  shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];

  shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
  shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
  shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
  shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];

  shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
  shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
  shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
  shadowMat[3][3] = dot - lightpos[W] * groundplane[W];

  GLfloat r = shadowMat[3][3];
  for (int i = 0; i < 4; ++i)
  {
	  for (int j = 0; j < 4; ++j)
	  {
		  shadowMat[i][j] /= r;
	  }
  }
}

void
findPlane(GLfloat plane[4],
  GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
  GLfloat vec0[3], vec1[3];

  /* Need 2 vectors to find cross product. */
  vec0[X] = v1[X] - v0[X];
  vec0[Y] = v1[Y] - v0[Y];
  vec0[Z] = v1[Z] - v0[Z];

  vec1[X] = v2[X] - v0[X];
  vec1[Y] = v2[Y] - v0[Y];
  vec1[Z] = v2[Z] - v0[Z];

  /* find cross product to get A, B, and C of plane equation */
  plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
  plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
  plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];

  plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}

void GLWidget::paintGL()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	static GLfloat floorVertices[4][3] = {
	  { 100, -19.9, -100 },
	  { -100, -19.9, -100 },
	  { -100, -39.9, 100 },
	  { 100, -39.9, 100 }
	};
	static GLfloat floorPlane[4] = {1, 2, 1, -10};
	static GLfloat floorShadow[4][4];

#if 0
	// render the shadow rect
	glColor3f(0, 1, 0);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i)
	{
		glVertex3f(floorVertices[i][0], floorVertices[i][1], floorVertices[i][2]);
	}
	glEnd();
#endif
	
	glColor3f(1.0, 1.0, 1.0);

#if 0
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
	glVertex3f(lightPosition[0], lightPosition[1], lightPosition[2]);
    glEnd();
#endif

	glPushMatrix();

	//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	//glPushMatrix();
	//glLoadIdentity();
	/*
	GLfloat mat_specular[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat mat_shininess[] = {50.0};
	GLfloat lmodel_ambient[] = {0.3, 0.3, 0.3, 1.0};
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	GLfloat lightAmbient[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat posTmp[4] = {};
	posTmp[0] = lightPosition[0] + translateX;
	posTmp[1] = lightPosition[1] + translateY;
	posTmp[2] = lightPosition[2] + translateZ;
	posTmp[3] = lightPosition[3];
	glLightfv(GL_LIGHT0, GL_POSITION, posTmp);
	glLightfv( GL_LIGHT0, GL_AMBIENT, lightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse );
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	*/
	//glPopMatrix();

	//glLoadIdentity();

	if (!img.isNull()) 
	{
		glDisable(GL_LIGHTING);
		glBindTexture(GL_TEXTURE_2D, BGtexture);
		glBegin(GL_QUADS);
		float step = 0.08;
		float zindex = 0;
		float dx = img.width() / 2;
		float dy = img.height() / 2;
		glTexCoord2f(0,0);
		glVertex3f(-viewWidth, -viewHeight, -1000);
		glTexCoord2f(0, 1);
		glVertex3f(-viewWidth, viewHeight, -1000);
		glTexCoord2f(1,1);
		glVertex3f(viewWidth, viewHeight, -1000);
		glTexCoord2f(1, 0);
		glVertex3f(viewWidth, -viewHeight, -1000);
		glEnd();
		glEnable(GL_LIGHTING);
	}


    /*glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 3, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);*/

	/*
	glLoadIdentity();
	glTranslatef(lightPosition[0], lightPosition[1], lightPosition[2]);
	glBegin(GL_QUADS);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, -1, -1);
	glEnd();
	*/
	
	GLdouble model_view[16];

	glLoadIdentity();
	glTranslatef(translateX, translateY, translateZ);
	glRotatef(rotateX / 16.0, 1.0, 0.0, 0.0);
	glRotatef(rotateY / 16.0, 0.0, 1.0, 0.0);
	glRotatef(rotateZ / 16.0, 0.0, 0.0, 1.0);
	glScalef(scale, scale, scale);
	
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

	glLoadIdentity();

	
	//glDisable(GL_DEPTH_TEST);
	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	//glPushAttrib(GL_LIGHTING_BIT);
	//glDisable(GL_LIGHTING);
	for (int i = 0; i < objectList.size(); i++) {
		objectList[i].render(model_view);
	}
	//glEnable(GL_LIGHTING);
	//glPopAttrib();
	//glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	
    //glStencilFunc(GL_LESS, 2, 0xffffffff);  /* draw if ==1 */
    //glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);


    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_LIGHTING);  /* Force the 50% black. */
    //glColor4f(0.0, 0.0, 0.0, 0.5);
	
    //glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    //glEnable(GL_DEPTH_TEST);

	//glStencilFunc(GL_EQUAL, 1, 0xffffffff);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	
	glEnable(GL_STENCIL_TEST);
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_GREATER, 3, 0xffffffff);
    //glStencilFunc(GL_LESS, 2, 0xffffffff); 
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glPushMatrix();
    /* Project the shadow. */
	glDisable(GL_LIGHTING);
	//glLoadIdentity();
	//glRotatef(rotateZ / 16.0, 0.0, 0.0, -1.0);
	//glRotatef(rotateY / 16.0, 0.0, -1.0, 0.0);
	//glRotatef(rotateX / 16.0, -1.0, 0.0, 0.0);
	//glTranslatef(-translateX, -translateY, -translateZ);
	
	//glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
 /*
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
 
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	viewport[2] = viewWidth * 2;
	viewport[3] = viewHeight * 2;
	*/
	//GLdouble fv[4][3];
	//GLfloat fvf[4][3];
	/*
	for (int i = 0; i < 4; ++i)
	{
		gluProject(floorVertices[i][0], floorVertices[i][1], floorVertices[i][2], model_view, projection, viewport, fv[i], fv[i]+1, fv[i]+2);
		for (int j = 0; j < 3; ++j)
		{
			fvf[i][j] = fv[i][j];
		}
	}

	findPlane(floorPlane, fvf[1], fvf[2], fvf[3]);
	*/


	glLoadIdentity();
	
	findPlane(floorPlane, floorVertices[1], floorVertices[2], floorVertices[3]);
	shadowMatrix(floorShadow, floorPlane, lightPosition);
    glMultMatrixf((GLfloat *) floorShadow);
	

	glEnable(GL_BLEND); //Enable blending.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

	glColor4f(0.0, 0.0, 0.0, 0.2);
    for (int i = 0; i < objectList.size(); i++) {
		objectList[i].render(model_view);//model_view, projection, viewport);
	}

	glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);

	glDisable(GL_STENCIL_TEST);


    glPopMatrix();

    //glDisable(GL_BLEND);
    //glEnable(GL_LIGHTING);

    //glDisable(GL_STENCIL_TEST);

    //glPushMatrix();
    //glDisable(GL_LIGHTING);
    //glColor3f(1.0, 1.0, 0.0);
    //glEnable(GL_LIGHTING);
    //glPopMatrix();

  glPopMatrix();
}

void GLWidget::resizeGL(int width, int height)
{
	if (0 == height)
		height = 1;									//防止一条边为0
	glViewport(0, 0, (GLint)width, (GLint)height);	//重置当前视口，本身不是重置窗口的，只不过是这里被Qt

	glMatrixMode(GL_PROJECTION);					//选择投影矩阵
	glLoadIdentity();								//重置选择好的投影矩阵

	if (projection) {
		if (width <= height) {
			viewWidth = nRange;
			viewHeight = nRange*height / width;
		} else {
			viewWidth = nRange*width / height;
			viewHeight = nRange;
		}
		glOrtho(-viewWidth, viewWidth, -viewHeight, viewHeight, -nRange * nRange, nRange * nRange);
	}
	else {
		GLdouble znear = 0.7, zfar = 10;
		if (width <= height) {
			viewWidth = nRange;
			viewHeight = nRange*height / width;
		} else {
			viewWidth = nRange*width / height;
			viewHeight = nRange;
		}
		glFrustum(-viewWidth, viewWidth, -viewHeight, viewHeight, znear, zfar);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	initializeGL();
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
