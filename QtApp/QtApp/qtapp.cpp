#include "qtapp.h"

#include "Object3D.h"

#if _MSC_VER >= 1600  
	#pragma execution_character_set("utf-8")
#endif  

QtApp::QtApp(QWidget *parent) : QWidget(parent)
{
	glWidget = new GLWidget();

	ui.setupUi(this);
	ui.gridLayout->addWidget(glWidget);

	glWidget->setFixedSize(width() - 180, height() - 100);
	QObject::connect(glWidget, SIGNAL(resizeWindow(int,int)), this, SLOT(resizeWindow(int,int)));

	keyframe frame;
	frame.frame = 1;
	keyframes.push_back(frame);

	setWindowTitle("网络模型的真实感显示");
}

QtApp::~QtApp()
{

}

void QtApp::changeTotalFrames()
{
	int count = ui.spinBox->value();
	if (keyframes[keyframes.size() - 1].frame > count){
		QMessageBox::information(NULL, "错误", "存在越界的关键帧");
		ui.spinBox->setValue(keyframes[keyframes.size() - 1].frame);
	}
	else{
		ui.horizontalSlider->setMaximum(count);
		QString str="";
		if (isKeyframe(ui.spinBox_2->value()))
			str.append("关键帧");
		else 
			str.append("普通帧");
		ui.label->setText(QString("/%1(%2)").arg(count).arg(str));
	}
}

bool QtApp::isKeyframe(int n)
{
	for (int i = 0; i<keyframes.size();i++)
	if (keyframes[i].frame == n) return true;
	return false;
}

void QtApp::prevFrame()
{
	showFrame(ui.spinBox_2->value() - 1);
}

void QtApp::nextFrame()
{
	showFrame(ui.spinBox_2->value() + 1);
}

void QtApp::prevKeyframe()
{
	for (int i = 0; i < keyframes.size(); i++)
	{
		if (keyframes[i].frame >= ui.spinBox_2->value()){
			if (i>0) showFrame(keyframes[i - 1].frame);
			else QMessageBox::information(NULL, "错误", "关键帧不存在");
			return;
		}
	}
	showFrame(keyframes[keyframes.size() - 1].frame);
}

void QtApp::nextKeyframe()
{
	for (int i = 0; i < keyframes.size(); i++)
	{
		if (keyframes[i].frame > ui.spinBox_2->value()){
			if (i<keyframes.size()) showFrame(keyframes[i].frame);
			else QMessageBox::information(NULL, "错误", "关键帧不存在");
			return;
		}
	}
	QMessageBox::information(NULL, "错误", "关键帧不存在");
}

void QtApp::saveKeyframe()
{
	int n = ui.spinBox_2->value();
	keyframe frame;
	frame.frame = n;
	frame.rotateX = glWidget->rotateX;
	frame.rotateY = glWidget->rotateY;
	frame.rotateZ = glWidget->rotateZ;
	frame.scale = glWidget->scale;
	frame.translateX = glWidget->translateX;
	frame.translateY = glWidget->translateY;
	frame.translateZ = glWidget->translateZ;
	//To do
	//save state...
	QVector<keyframe>::Iterator iter;
	for (iter = keyframes.begin(); iter != keyframes.end();) {
		if (iter->frame == n){
			*iter = frame;
			QMessageBox::information(NULL, "success", "保存关键帧成功");
			ui.label->setText(QString("/%1(%2)").arg(ui.spinBox->value()).arg("关键帧"));
			return;
		}else if (iter->frame > n){
			keyframes.insert(iter, frame);
			QMessageBox::information(NULL, "success", "保存关键帧成功");
			ui.label->setText(QString("/%1(%2)").arg(ui.spinBox->value()).arg("关键帧"));
			return;
		}
		else
			iter++;
	}
	keyframes.push_back(frame);
	QMessageBox::information(NULL, "success", "保存关键帧成功");
	ui.label->setText(QString("/%1(%2)").arg(ui.spinBox->value()).arg("关键帧"));
}

void QtApp::delKeyframe()
{
	int n = ui.spinBox_2->value();
	QVector<keyframe>::Iterator iter;
	for (iter = keyframes.begin()+1; iter != keyframes.end();) {
		if (iter->frame == n){
			keyframes.erase(iter);
			QMessageBox::information(NULL, "success", "删除关键帧成功");
			ui.label->setText(QString("/%1(%2)").arg(ui.spinBox->value()).arg("普通帧"));
			return;
		}
		else
			iter++;
	}
	QMessageBox::information(NULL, "错误", "删除关键帧失败");
}

void QtApp::createVideo()
{
	int w, h;
	w = glWidget->width();
	h = glWidget->height();
	if ((w % 4) != 0)
	{
		int dw = 4 - w % 4;
		w = w + dw;
	}
	unsigned char* imgBuffer = (unsigned char*)malloc(w * h * 3);
	glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, imgBuffer); 
	cv::Mat mat = cv::Mat(h, w, CV_8UC3, imgBuffer);
	cv::VideoWriter output("demo.avi", CV_FOURCC('M','J','P','G'), 10, mat.size(), true);
	if(!output.isOpened())
		QMessageBox::information(NULL, "error", "error");

	int nframe = ui.spinBox->value();
	/*char str[100];
	sprintf(str, "%d", nframe);
	QMessageBox::information(NULL, "nframe", str);*/
	for(int i = 1; i <= nframe; i++)
	{
		ui.progressBar->setValue((double)i/nframe*100);
		showFrame(i);
		imgBuffer = (unsigned char*)malloc(w * h * 3);
		glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, imgBuffer); 

		cv::Mat mat = cv::Mat(h, w, CV_8UC3, imgBuffer);
		cv::Point2f center = cv::Point2f(mat.cols / 2, mat.rows / 2);  // 旋转中心  
		cv::Mat rotateMat;   
		rotateMat = getRotationMatrix2D(center, 180, 1);  
  
		cv::Mat rotateImg;  
		warpAffine(mat, rotateImg, rotateMat, mat.size());
		flip(rotateImg, mat, 1);

		output << mat;
	}
}

void QtApp::showFrame(int n)
{
	if (n > ui.spinBox->value())
		QMessageBox::information(NULL, "错误", "超过最大帧数");
	else if (n < 1)
		QMessageBox::information(NULL, "错误", "显示帧数需大于0");
	else{
		ui.horizontalSlider->setValue(n);
		ui.spinBox_2->setValue(n);
		QString str = "";
		if (isKeyframe(n)) str.append("关键帧");
		else str.append("普通帧");
		ui.label->setText(QString("/%1(%2)").arg(ui.spinBox->value()).arg(str));
		if (isKeyframe(n)){
			for (int i = 0; i < keyframes.size(); i++)
			{
				if (keyframes[i].frame == n){
					glWidget->rotateX = keyframes[i].rotateX;
					glWidget->rotateY = keyframes[i].rotateY;
					glWidget->rotateZ = keyframes[i].rotateZ;
					glWidget->scale = keyframes[i].scale;
					glWidget->translateX = keyframes[i].translateX;
					glWidget->translateY = keyframes[i].translateY;
					glWidget->translateZ = keyframes[i].translateZ;
				}
			}
		}
		else{
			keyframe next,prev;
			next.frame = 0;
			for (int i = 0; i < keyframes.size(); i++)
			{
				if (keyframes[i].frame > n){
					if (i < keyframes.size()){
						next = keyframes[i];
						break;
					}
					else return;
				}
			}
			if (next.frame == 0) return;
			prev = keyframes[keyframes.size() - 1];
			for (int i = 0; i < keyframes.size(); i++)
			{
				if (keyframes[i].frame >= n){
					if (i>0){
						prev = keyframes[i - 1];
						break;
					}
					else return;
				}
			}
			float delta = (float)(n - prev.frame) / (next.frame - prev.frame);
			glWidget->rotateX = prev.rotateX+(next.rotateX - prev.rotateX) * delta;
			glWidget->rotateY = prev.rotateY + (next.rotateY - prev.rotateY) * delta;
			glWidget->rotateZ = prev.rotateZ + (next.rotateZ - prev.rotateZ) * delta;
			glWidget->scale = prev.scale + (next.scale - prev.scale) * delta;
			glWidget->translateX = prev.translateX + (next.translateX - prev.translateX) * delta;
			glWidget->translateY = prev.translateY + (next.translateY - prev.translateY) * delta;
			glWidget->translateZ = prev.translateZ + (next.translateZ - prev.translateZ) * delta;
		}
		glWidget->updateGL();
	}
}

void QtApp::resizeEvent(QResizeEvent * event)
{
	//glWidget->setFixedSize(width() - 180, height() - 40);
}

void QtApp::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Escape:							//Ese为退出程序键
		close();
		break;
	case Qt::Key_A:
		glWidget->changeTranslateX(-1);
		break;
	case Qt::Key_D:
		glWidget->changeTranslateX(1);
		break;
	case Qt::Key_W:
		glWidget->changeTranslateY(1);
		break;
	case Qt::Key_S:
		glWidget->changeTranslateY(-1);
		break;
	case Qt::Key_E:
		glWidget->changeTranslateZ(1);
		break;
	case Qt::Key_Q:
		glWidget->changeTranslateZ(-1);
		break;
	case Qt::Key_Equal:
		glWidget->changeScale(0.1);
		break;
	case Qt::Key_Minus:
		glWidget->changeScale(-0.1);
		break;
	}
}

void QtApp::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {

	}
	else if (event->button() == Qt::RightButton) {

	}
}


// slots
void QtApp::OpenFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		"打开OBJ文件", QDir::currentPath(), "Object(*.obj *.OBJ)");

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::information(NULL, "错误", "读取OBJ文件失败");
		return;
	}

	QVector<QVector3D> vertex;
	QVector<QVector3D> face_triangle;
	QVector<QVector4D> face_quad;

	QTextStream stream(&file);
	QString line;
	while (!stream.atEnd()) {
		line = stream.readLine();
		QStringList info = line.split(' ');
		if (info[0] == "v") {
			vertex.push_back(QVector3D(info[1].toFloat(), info[2].toFloat(), info[3].toFloat()));
		}
		else if(info[0] == "f" && info.size() == 4) {
			face_triangle.push_back(QVector3D(info[1].toInt() - 1, info[2].toInt() - 1, info[3].toInt() - 1));
		}
		else if (info[0] == "f" && info.size() == 5) {
			face_quad.push_back(QVector4D(info[1].toInt() - 1, info[2].toInt() - 1, info[3].toInt() - 1, info[4].toInt() - 1));
		}
	}

	glWidget->addObject(Object3D(vertex, face_triangle, face_quad));

	file.close();
}

void QtApp::ClearObject()
{
	glWidget->clearObject();
}

void QtApp::OrthographicProjection()
{
	glWidget->setProjection(true);
}

void QtApp::PerspectiveProjection()
{
	glWidget->setProjection(false);
}

void QtApp::loadTextures()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		"打开纹理图片", QDir::currentPath(), "picture(*.jpg *.png *.bmp)");
	glWidget->loadTextures(fileName);
}

void QtApp::nRangeChanged(int nRange)
{
	glWidget->setNRange(nRange);
}

void QtApp::lightHeightChanged(int height)
{
	glWidget->setLightHeight(height);
}

void QtApp::chooseBG()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		"打开背景文件", QDir::currentPath(), "picture(*.jpg *.png *.bmp)");

	glWidget->setBG(fileName);
}

void QtApp::resizeWindow(int w, int h)
{
	setMinimumSize(w, h);
	resize(w, h);
}
