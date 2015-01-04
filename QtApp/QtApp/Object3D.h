#pragma once

#include "qvector.h"
#include "qvector3d.h"
#include "qvector4d.h"

class Object3D
{
public:
	Object3D();
	Object3D(QVector<QVector3D>, QVector<QVector3D>, QVector<QVector4D>);
	~Object3D();

	QVector<QVector3D> getFaceTri(int);
	QVector<QVector3D> getFaceQuad(int);
	int getFaceCountTri();
	int getFaceCountQuad();

private:
	QVector<QVector3D> vertex;
	QVector<QVector3D> face_triangle;
	QVector<QVector4D> face_quad;
};

