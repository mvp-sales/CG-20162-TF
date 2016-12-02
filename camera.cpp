#include "camera.h"

double Camera::getFarZ() {
	return farZ;
}

double Camera::getNearZ() {
	return nearZ;
}

double Camera::getAngle() {
	return camAngle;
}

int Camera::getAngleVertical() {
	return this->camAngleYZ;
}

int Camera::getAngleHorizontal() {
	return this->camAngleXY;
}

int Camera::getCurrentCamera() {
	return cameraVision;
}

void Camera::setAngle(double angle) {
	camAngle = angle;
}

void Camera::giroVertical(int value) {
	this->camAngleYZ += value;
	if (this->camAngleYZ > MAX_ANGLE_VERTICAL) this->camAngleYZ = MAX_ANGLE_VERTICAL;
	else if (this->camAngleYZ < -MAX_ANGLE_VERTICAL) this->camAngleYZ = -MAX_ANGLE_VERTICAL;
}
void Camera::giroHorizontal(int value) {
	this->camAngleXY += value;
	if (this->camAngleXY > MAX_ANGLE_HORIZONTAL) this->camAngleXY = MAX_ANGLE_HORIZONTAL;
	else if (this->camAngleXY < -MAX_ANGLE_HORIZONTAL) this->camAngleXY = -MAX_ANGLE_HORIZONTAL;
}

void Camera::toggleCamera(int cameraVision) {
	this->cameraVision = cameraVision;
}
void Camera::updateCamera(int w, int h) {
	glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(this->getAngle(), (GLfloat)w / (GLfloat)h, this->getNearZ(), this->getFarZ());
    glMatrixMode (GL_MODELVIEW);
}
void Camera::lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx,
          GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy,
          GLdouble upz)
{
    gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
}
