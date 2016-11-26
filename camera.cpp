#include "camera.h"

int Camera::getCurrentCamera() {
	return cameraVision;
}
void Camera::toggleCamera(int cameraVision) {
	this->cameraVision = cameraVision;
}
void Camera::updateCamera(int w, int h) {
	glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45, (GLfloat)w / (GLfloat)h, 1, 150);
    glMatrixMode (GL_MODELVIEW);
}
void Camera::lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx,
          GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy,
          GLdouble upz)
{
    gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
}
