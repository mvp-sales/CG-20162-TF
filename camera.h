#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#define MAX_ANGLE_VERTICAL 90
#define MAX_ANGLE_HORIZONTAL 180

class Camera {
	private:
		int cameraVision;
		double nearZ;
		double farZ;
		double camAngle;
		int camAngleXY;
		int camAngleYZ;
	public:
		Camera(int cameraVision, double camAngle, double nearZ, double farZ) {
			this->cameraVision = cameraVision;
			this->nearZ = nearZ;
			this->farZ = farZ;
			this->camAngle = camAngle;
			this->camAngleXY = 0;
			this->camAngleYZ = 60;
		}
		~Camera();
		double getAngle();
		double getNearZ();
		double getFarZ();
		void setAngle(double angle);
		int getCurrentCamera();
		int getAngleVertical();
		int getAngleHorizontal();
		void giroVertical(int value);
		void giroHorizontal(int value);
		void toggleCamera(int cameraVision);
		void updateCamera(int w, int h);
		void lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez,
					GLdouble centerx, GLdouble centery, GLdouble centerz,
					GLdouble upx, GLdouble upy, GLdouble upz);
};

#endif
