#include "camera.h"

void makeIdentityf(GLfloat m[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = i == j;
        }
    }
}

void normalize(GLdouble o[3]) {
    GLdouble norm = sqrt(o[0]*o[0]+o[1]*o[1]+o[2]*o[2]);
    o[0] /= norm;
    o[1] /= norm;
    o[2] /= norm;   
}

void crossProduct(
        GLdouble u[3], 
        GLdouble v[3], 
        GLdouble o[3])
{
    o[0] = u[1]*v[2] - u[2]*v[1];
    o[1] = u[2]*v[0] - u[0]*v[2];
    o[2] = u[0]*v[1] - u[1]*v[0];
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
    glFrustum(-w/2, w/2, -h/2, h/2, nearZ, farZ);
    glMatrixMode (GL_MODELVIEW);
}
void Camera::lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx,
          GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy,
          GLdouble upz)
{
    GLdouble forward[3], side[3], up[3];
    GLfloat m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    normalize(forward);

    /* Side = forward x up */
    crossProduct(forward, up, side);
    normalize(side);

    /* Recompute up as: up = side x forward */
    crossProduct(side, forward, up);

    makeIdentityf(m);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixf(&m[0][0]);
    glTranslated(-eyex, -eyey, -eyez);
}