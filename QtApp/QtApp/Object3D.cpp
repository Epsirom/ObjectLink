#include "Object3D.h"


Object3D::Object3D()
{
}


Object3D::Object3D(QVector<QVector3D> vertexList, QVector<QVector3D> face1, QVector<QVector4D> face2)
{
	vertex = vertexList;
	face_triangle = face1;
	face_quad = face2;
}


Object3D::~Object3D()
{
}

QVector<QVector3D> Object3D::getFaceTri(int index)
{
	QVector<QVector3D> face;
	face.push_back(vertex.at(face_triangle.at(index).x()));
	face.push_back(vertex.at(face_triangle.at(index).y()));
	face.push_back(vertex.at(face_triangle.at(index).z()));
	return face;
}

QVector<QVector3D> Object3D::getFaceQuad(int index)
{
	QVector<QVector3D> face;
	face.push_back(vertex.at(face_quad.at(index).w()));
	face.push_back(vertex.at(face_quad.at(index).x()));
	face.push_back(vertex.at(face_quad.at(index).y()));
	face.push_back(vertex.at(face_quad.at(index).z()));
	return face;
}

int Object3D::getFaceCountTri()
{
	return face_triangle.count();
}

int Object3D::getFaceCountQuad()
{
	return face_quad.count();
}

