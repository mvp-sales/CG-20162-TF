#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

void makeIdentityf(GLfloat m[4][4]);
void normalize(GLdouble o[3]);
void crossProduct(GLdouble u[3], GLdouble v[3], GLdouble o[3]);

class Camera {
	private:
		int cameraVision;
		double nearZ;
		double farZ;
	public:
		Camera(int cameraVision, double nearZ, double farZ) {
			this->cameraVision = cameraVision;
			this->nearZ = nearZ;
			this->farZ = farZ;
		}
		~Camera();
		int getCurrentCamera();
		void toggleCamera(int cameraVision);
		void updateCamera(int w, int h);
		void lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, 
					GLdouble centerx, GLdouble centery, GLdouble centerz, 
					GLdouble upx, GLdouble upy, GLdouble upz);
};

#endif