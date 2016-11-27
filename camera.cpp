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

int Camera::getCurrentCamera() {
	return cameraVision;
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
