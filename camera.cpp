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
	if (this->camAngleYZ >= MAX_ANGLE_VERTICAL) this->camAngleYZ = MAX_ANGLE_VERTICAL - 0.1;
	else if (this->camAngleYZ < 0) this->camAngleYZ = 0;
}
void Camera::giroHorizontal(int value) {
	this->camAngleXY += value;
	if (this->camAngleXY > MAX_ANGLE_HORIZONTAL) this->camAngleXY = MAX_ANGLE_HORIZONTAL;
	else if (this->camAngleXY < -MAX_ANGLE_HORIZONTAL) this->camAngleXY = -MAX_ANGLE_HORIZONTAL;
}

void Camera::adjustCamera(CarroJogador* jogador) {

	if (jogador == NULL)
		return;

  if (this->getCurrentCamera() == 1) {
    //Câmera no cockpit

    double ex = jogador->getPosicao().x;
    double ey = jogador->getPosicao().y;
    double ez = 0.9 * jogador->getAltura();

    double px = ex - sin(jogador->getAngCarro() * DEG2RAD);
    double py = ey + cos(jogador->getAngCarro() * DEG2RAD);
    double pz = 0.9 * jogador->getAltura();

    this->lookAt(ex, ey, ez,
         px, py, pz,
         0, 0, 1);
  }
  else if (this->getCurrentCamera() == 2) {
    //Câmera do canhão

    double** cannonPoints = jogador->calcularPontosCanhao();
    double* saida = cannonPoints[0];
    double* origem = cannonPoints[1];
    double* saidaParalela = cannonPoints[2];

    double distancia = 0.3*jogador->getAltura();
    double alpha = 90 + jogador->getAngCanhaoV();
    double zOffset = distancia * sin(alpha * DEG2RAD);
    double horizontalDistance = distancia * cos(alpha * DEG2RAD);
    double theta = jogador->getAngCarro() + jogador->getAngCanhaoH();
    double xOffset = horizontalDistance * sin(theta * DEG2RAD);
    double yOffset = horizontalDistance * cos(theta * DEG2RAD);

    this->lookAt(origem[0] + xOffset, origem[1] - yOffset, origem[2]+ zOffset,
         saidaParalela[0] + xOffset, saidaParalela[1] - yOffset, saidaParalela[2] + zOffset,
         0, 0, 1);

    deleteMat(cannonPoints, 3);
  }
  else if (this->getCurrentCamera() == 3) {
    //Câmera atrás do carro, seguindo sua posição

    const float distCamera = 6 * jogador->getAltura();
    float horDistCamera = distCamera * cos(this->getAngleVertical() * DEG2RAD);
    float vertDistCamera = distCamera * sin (this->getAngleVertical() * DEG2RAD);

    float theta = jogador->getAngCarro() + this->getAngleHorizontal() + 180;

    float xOffsetCamera = horDistCamera * sin(theta * DEG2RAD);
    float yOffsetCamera = horDistCamera * cos(theta * DEG2RAD);

    double px = jogador->getPosicao().x;
    double py = jogador->getPosicao().y;
    double pz = jogador->getAltura();

    double ex = px - xOffsetCamera;
    double ey = py + yOffsetCamera;
    double ez = pz + vertDistCamera;

    this->lookAt(ex, ey, ez,
         px, py, pz,
         0, 0, 1);
  }
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
