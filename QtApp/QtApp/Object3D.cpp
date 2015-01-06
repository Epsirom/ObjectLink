#include "Object3D.h"

#include <glut.h>


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

void Object3D::render()
{
	glBegin(GL_TRIANGLES);
	for (int j = 0; j < getFaceCountTri(); j++) {
		QVector<QVector3D> face = getFaceTri(j);
		for (int k = 0; k < face.size(); k++) {
			glTexCoord2f(0.5f + 0.5f * face[k].x(), 0.5f + 0.5f * face[k].z());
			glVertex3f(face[k].x(), face[k].y(), face[k].z());
		}
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int j = 0; j < getFaceCountQuad(); j++) {
		QVector<QVector3D> face = getFaceQuad(j);
		for (int k = 0; k < face.size(); k++) {
			glTexCoord2f(k == 0 || k == 3 ? 0.0 : 1.0, k == 0 || k == 1 ? 0.0 : 1.0);
			glVertex3f(face[k].x(), face[k].y(), face[k].z());
		}
	}
	glEnd();
}

void Object3D::render(const GLdouble model_view[16], const GLdouble projection[16], const GLint viewport[4])
{
	GLdouble pt[3] = {0};
	GLdouble ptp[3] = {0};

	glBegin(GL_TRIANGLES);
	for (int j = 0; j < getFaceCountTri(); j++) {
		QVector<QVector3D> face = getFaceTri(j);
		for (int k = 0; k < face.size(); k++) {
			glTexCoord2f(0.5f + 0.5f * face[k].x(), 0.5f + 0.5f * face[k].z());
			pt[0] = face[k].x(); pt[1] = face[k].y(); pt[2] = face[k].z();
			gluProject(pt[0], pt[1], pt[2], model_view, projection, viewport, ptp, ptp+1, ptp+2);
			glVertex3f(ptp[0], ptp[1], ptp[2]);
		}
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int j = 0; j < getFaceCountQuad(); j++) {
		QVector<QVector3D> face = getFaceQuad(j);
		for (int k = 0; k < face.size(); k++) {
			glTexCoord2f(k == 0 || k == 3 ? 0.0 : 1.0, k == 0 || k == 1 ? 0.0 : 1.0);
			pt[0] = face[k].x(); pt[1] = face[k].y(); pt[2] = face[k].z();
			gluProject(pt[0], pt[1], pt[2], model_view, projection, viewport, ptp, ptp+1, ptp+2);
			glVertex3f(ptp[0], ptp[1], ptp[2]);
		}
	}
	glEnd();
}

void Object3D::render(const GLdouble mv[16])
{
	GLdouble x, y, z;
	GLfloat xp, yp, zp, wp;

	glBegin(GL_TRIANGLES);
	for (int j = 0; j < getFaceCountTri(); j++) {
		QVector<QVector3D> face = getFaceTri(j);
		for (int k = 0; k < face.size(); k++) {
			glTexCoord2f(0.5f + 0.5f * face[k].x(), 0.5f + 0.5f * face[k].z());
			x = face[k].x(); y = face[k].y(); z = face[k].z();
			xp = mv[0] * x + mv[4] * y + mv[8] * z + mv[12];
			yp = mv[1] * x + mv[5] * y + mv[9] * z + mv[13];
			zp = mv[2] * x + mv[6] * y + mv[10] * z + mv[14];
			wp = mv[3] * x + mv[7] * y + mv[11] * z + mv[15];

			xp /= wp;
			yp /= wp;
			zp /= wp;
			glVertex3f(xp, yp, zp);
		}
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int j = 0; j < getFaceCountQuad(); j++) {
		QVector<QVector3D> face = getFaceQuad(j);
		for (int k = 0; k < face.size(); k++) {
			glTexCoord2f(k == 0 || k == 3 ? 0.0 : 1.0, k == 0 || k == 1 ? 0.0 : 1.0);
			x = face[k].x(); y = face[k].y(); z = face[k].z();
			xp = mv[0] * x + mv[4] * y + mv[8] * z + mv[12];
			yp = mv[1] * x + mv[5] * y + mv[9] * z + mv[13];
			zp = mv[2] * x + mv[6] * y + mv[10] * z + mv[14];
			wp = mv[3] * x + mv[7] * y + mv[11] * z + mv[15];

			xp /= wp;
			yp /= wp;
			zp /= wp;
			glVertex3f(xp, yp, zp);
		}
	}
	glEnd();
}
