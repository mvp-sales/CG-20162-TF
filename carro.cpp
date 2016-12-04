#include "carro.h"

void deleteMat(double** ptr, int size) {
	for (int i = 0; i< size; i++) {
		delete [] ptr[i];
	}
	delete [] ptr;
}

//Setters
void Carro::setPosicao(Ponto pos) {
	_circ->centro = pos;
}
void Carro::setRaio(double raio) {
	_circ->raio = raio;
}
void Carro::setTexturas(GLuint texturas[5]) {
	for (int i = 0; i < 5; i++) {
		this->texturas[i] = texturas[i];
	}
}

void Carro::setVelTiro(double vtiro) {
	_velTiro = vtiro;
}
void Carro::setVelCarro(double vcarro) {
	_velCarro = vcarro;
}

void Carro::setAngRodas(double ang) {
	_angRodas = ang;
	if (_angRodas < -LIMITE_ANGULO)	_angRodas = -LIMITE_ANGULO;
	else if (_angRodas > LIMITE_ANGULO)	_angRodas = LIMITE_ANGULO;
}

//Getters
bool Carro::isPlayer() {
	return _isPlayer;
}

double Carro::getAngCanhaoH() {
	return _angCanhaoH;
}

double Carro::getAngCanhaoV() {
	return _angCanhaoV;
}

double Carro::getAngCarro() {
	return _angCarro;
}

double Carro::getAngRodas() {
	return _angRodas;
}

double Carro::getAngRodasGiro() {
	return _angRodasGiro;
}

Circulo Carro::getCirculo() {
	return *_circ;
}
Ponto Carro::getPosicao() {
	return _circ->centro;
}

double Carro::getRaio() {
	return _circ->raio;
}

GLuint Carro::getTextura(int i) {
	return texturas[i];
}

double* Carro::getProporcaoCanhao() {
	double* prop = new double[3];
	prop[0] = 0;
	prop[1] = 2;
	if (this->isPlayer())
		prop[2] = 3.59;
	else
		prop[2] = 2.59;
	return prop;
}

double Carro::getLengthCanhao() {
	return 3.2;
}

double Carro::getVerticalAdjCanhao() {
	return 0.45;
}

double* Carro::getEscala() {
	double* scale = new double[3];
	double raio = this->getRaio();
	if (this->isPlayer()) {
		scale[0] = raio * 1./5;
		scale[1] = raio * 1./6;
		scale[2] = raio * 1./5;
	}
	else {
		scale[0] = raio * 1./4;
		scale[1] = raio * 1./5;
		scale[2] = raio * 1./5;
	}
	return scale;
}

double* Carro::getProporcaoFarol() {
	double* prop = new double[3];
	double adjustX, adjustY, adjustZ;
	if (this->isPlayer()) {
		adjustX = 2;
		adjustY = 0.76;
		adjustZ = 3.15;
	}
	else {
		adjustX = 1.98;
		adjustY = 0.96;
		adjustZ = 3.16;
	}
	prop[0] = adjustX - 0.7;
	prop[1] = adjustY + 0.8;
	prop[2] = adjustZ - 0.8;
	return prop;
}

float* Carro::getProporcaoRodas() {
	float* prop = new float[3];
	float adjustX, adjustY, adjustZ;
	if (this->isPlayer()) {
		adjustX = 2;
		adjustY = 0.76;
		adjustZ = 3.15;
	}
	else {
		adjustX = 1.98;
		adjustY = 0.96;
		adjustZ = 3.16;
	}
	prop[0] = adjustX;
	prop[1] = adjustY;
	prop[2] = adjustZ;
	return prop;
}

double Carro::getVelCarro() {
	return _velCarro;
}

double Carro::getTurnRate() {
	if (this->isPlayer()) {
		float* wheels = this->getProporcaoRodas();
		double* front = this->calcularNovaPosicao(wheels[0], wheels[1], wheels[2]);
		double* back = this->calcularNovaPosicao(wheels[0], wheels[1], -wheels[2]);
		double distWheel = sqrt(pow(front[0] - back[0],2) + pow(front[1] - back[1],2) + pow(front[2] - back[2],2));
		double turnRate = this->getAngRodas() * this->getVelCarro() / distWheel;
		delete [] wheels;
		delete [] front;
		delete [] back;
		return turnRate;
	}
	return _angRodas/1000;
}
double Carro::getAltura() {
	return 2.7 * this->getCirculo().raio / 5;
}


//Métodos

bool Carro::colisaoCarro(Carro* self, std::list<CarroInimigo*>& carros) {
	for(std::list<CarroInimigo*>::iterator it = carros.begin(); it != carros.end(); ++it) {
		CarroInimigo* carro = *it;
		if (carro != self && colisaoCirc(carro->getCirculo(), self->getCirculo()))
			return true;
	}
	return false;
}

void Carro::desenharBodyPart() {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionChassis);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matColorAChassis);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matColorDChassis);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularChassis);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessChassis);

	glEnable(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, this->getTextura(TEX_BODYPART));

	bodypart->draw();

	glDisable(GL_TEXTURE_2D);
}

void Carro::desenharCanhao() {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionCanhao);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matColorACanhao);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matColorDCanhao);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularCanhao);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessCanhao);

	glEnable(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, this->getTextura(TEX_CHASSI));
	cannon->draw();

	glDisable(GL_TEXTURE_2D);
}

void Carro::desenharChassi() {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionChassis);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matColorAChassis);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matColorDChassis);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularChassis);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessChassis);

	glEnable(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, this->getTextura(TEX_CHASSI));
	chassi->draw();

	glDisable(GL_TEXTURE_2D);
}

void Carro::desenharEngine() {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionChassis);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matColorAChassis);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matColorDChassis);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularChassis);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessChassis);

	glEnable(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, this->getTextura(TEX_ENGINE));
	engine->draw();

	glDisable(GL_TEXTURE_2D);
}

void Carro::desenharInterior() {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionChassis);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matColorAChassis);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matColorDChassis);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularChassis);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessChassis);

	glEnable(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, this->getTextura(TEX_INTERIOR));
	interior->draw();

	glDisable(GL_TEXTURE_2D);
}

void Carro::desenharRoda(Obj* wheel) {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionRodas);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matColorARodas);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matColorDRodas);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularRodas);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessRodas);

	glEnable(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, this->getTextura(TEX_WHEEL));

	wheel->draw();

	glDisable(GL_TEXTURE_2D);
}

void Carro::virarRoda(double ang) {
	_angRodas += ang;
	if (_angRodas < -LIMITE_ANGULO) {
		_angRodas = -LIMITE_ANGULO;
	}
	else if (_angRodas > LIMITE_ANGULO) {
		_angRodas = LIMITE_ANGULO;
	}
}

void Carro::virarCanhaoH(double ang) {
	_angCanhaoH += ang;
	if (_angCanhaoH < -LIMITE_ANGULO) {
		_angCanhaoH = -LIMITE_ANGULO;
	}
	else if (_angCanhaoH > LIMITE_ANGULO) {
		_angCanhaoH = LIMITE_ANGULO;
	}
}

void Carro::virarCanhaoV(double ang) {
	_angCanhaoV += ang;
	if (_angCanhaoV < -LIMITE_ANGULO) {
		_angCanhaoV = -LIMITE_ANGULO;
	}
	else if (_angCanhaoV > 0) {
		_angCanhaoV = 0;
	}
}


void Carro::virarCarro(double taxa, GLdouble timeDiff) {
	_angCarro += taxa * timeDiff;
	if (_angCarro < 0) _angCarro = 360 + _angCarro;
	else if (_angCarro > 360) _angCarro = _angCarro - 360;
}

void Carro::girarRodas(int direction, GLdouble timeDiff) {
	_angRodasGiro += (1 / DEG2RAD) * direction * timeDiff * this->getVelCarro() / (this->getCirculo().raio * RODA_WIDTH);
	if (_angRodasGiro < 0)	_angRodasGiro += 360;
	else if (_angRodasGiro > 360)	_angRodasGiro -= 360;
}


void Carro::moverRanhuras(int direction, GLdouble timeDiff) {
	_posRanhuras += direction * timeDiff * this->getVelCarro()/(2 * M_PI * RODA_WIDTH * this->getCirculo().raio);
	if (_posRanhuras > DIST_RANHURAS)
		_posRanhuras = 0;
	else if ((_posRanhuras - DIST_RANHURAS) < -DIST_RANHURAS)
		_posRanhuras = DIST_RANHURAS;
}

double* Carro::calcularNovaPosicao(double x, double y, double z) {
	const double anguloCarro = this->getAngCarro();
	double* scale = this->getEscala();
	double transfMatrixRot1[4][4] = {
											{1.0, 0.0, 0.0, 0.0},
											{0, cos(90 * DEG2RAD), -sin(90 * DEG2RAD), 0},
											{0, sin(90 * DEG2RAD), cos(90 * DEG2RAD), 0},
											{0.0, 0.0, 0.0, 1.0}
										};
	double transfMatrixRot2[4][4] = {
											{cos(180 * DEG2RAD), 0.0, sin(180 * DEG2RAD), 0},
											{0.0, 1.0, 0.0, 0.0},
											{-sin(180 * DEG2RAD), 0.0, cos(180 * DEG2RAD), 0},
											{0.0, 0.0, 0.0, 1.0}
										};
	double transfMatrixCarroRot[4][4] = {
											{cos(anguloCarro * DEG2RAD), 0.0, sin(anguloCarro * DEG2RAD), 0},
											{0.0, 1.0, 0.0, 0.0},
											{-sin(anguloCarro * DEG2RAD), 0.0, cos(anguloCarro * DEG2RAD), 0},
											{0.0, 0.0, 0.0, 1.0}
										};
	double transfEscala[4][4] = {
		{scale[0], 0, 0, 0},
		{0, scale[1], 0, 0},
		{0, 0, scale[2], 0},
		{0, 0, 0, 1}
	};
	double transfTranslacao[4][4] = {
		{1, 0, 0, _circ->centro.x},
		{0, 1, 0, _circ->centro.y},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	double pontoRelativo[4] = {x, y, z, 1};

	//Ordem de operações: TSR1R2R3

	int i,j,k;
	double** m1 = new double*[4];
	for (i = 0; i < 4; i++) {
		m1[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m1[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m1[i][j] += transfMatrixRot1[i][k] * transfMatrixRot2[k][j];
			}
		}
	}

	double** m2 = new double*[4];
	for (i = 0; i < 4; i++) {
		m2[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m2[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m2[i][j] += m1[i][k] * transfMatrixCarroRot[k][j];
			}
		}
	}

	double** m3 = new double*[4];
	for (i = 0; i < 4; i++) {
		m3[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m3[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m3[i][j] += transfEscala[i][k] * m2[k][j];
			}
		}
	}

	double** m4 = new double*[4];
	for (i = 0; i < 4; i++) {
		m4[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m4[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m4[i][j] += transfTranslacao[i][k] * m3[k][j];
			}
		}
	}

	double* pontoGlobal = new double[4];
	for (i = 0; i < 4; i++) {
		pontoGlobal[i] = 0.0;
		for (j = 0; j < 4; j++) {
			pontoGlobal[i] += m4[i][j] * pontoRelativo[j];
		}
	}

	deleteMat(m1, 4);
	deleteMat(m2, 4);
	deleteMat(m3, 4);
	deleteMat(m4, 4);
	delete [] scale;
	return pontoGlobal;
}

double** Carro::calcularPontosCanhao() {
	//Determina a posição do canhão
	const double anguloCarro = this->getAngCarro();
	const double anguloCanhaoH = this->getAngCanhaoH();
	const double anguloCanhaoV = this->getAngCanhaoV();
	double* scale = this->getEscala();
	double* canhaoProp = this->getProporcaoCanhao();
	double transfMatrixRot1[4][4] = {
											{1.0, 0.0, 0.0, 0.0},
											{0, cos(90 * DEG2RAD), -sin(90 * DEG2RAD), 0},
											{0, sin(90 * DEG2RAD), cos(90 * DEG2RAD), 0},
											{0.0, 0.0, 0.0, 1.0}
										};
	double transfMatrixRot2[4][4] = {
											{cos(180 * DEG2RAD), 0.0, sin(180 * DEG2RAD), 0},
											{0.0, 1.0, 0.0, 0.0},
											{-sin(180 * DEG2RAD), 0.0, cos(180 * DEG2RAD), 0},
											{0.0, 0.0, 0.0, 1.0}
										};
	double transfMatrixCarroRot[4][4] = {
											{cos(anguloCarro * DEG2RAD), 0.0, sin(anguloCarro * DEG2RAD), 0},
											{0.0, 1.0, 0.0, 0.0},
											{-sin(anguloCarro * DEG2RAD), 0.0, cos(anguloCarro * DEG2RAD), 0},
											{0.0, 0.0, 0.0, 1.0}
										};
	double transfEscala[4][4] = {
		{scale[0], 0, 0, 0},
		{0, scale[1], 0, 0},
		{0, 0, scale[2], 0},
		{0, 0, 0, 1}
	};
	double transfTranslacao[4][4] = {
		{1, 0, 0, _circ->centro.x},
		{0, 1, 0, _circ->centro.y},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	double canhaoOrigemRelCarro[4] = {canhaoProp[0], canhaoProp[1], canhaoProp[2], 1};

	//Ordem de operações: TSR1R2R3

	int i,j,k;
	double** m1 = new double*[4];
	for (i = 0; i < 4; i++) {
		m1[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m1[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m1[i][j] += transfMatrixRot1[i][k] * transfMatrixRot2[k][j];
			}
		}
	}

	double** m2 = new double*[4];
	for (i = 0; i < 4; i++) {
		m2[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m2[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m2[i][j] += m1[i][k] * transfMatrixCarroRot[k][j];
			}
		}
	}

	double** m3 = new double*[4];
	for (i = 0; i < 4; i++) {
		m3[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m3[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m3[i][j] += transfEscala[i][k] * m2[k][j];
			}
		}
	}

	double** m4 = new double*[4];
	for (i = 0; i < 4; i++) {
		m4[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m4[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m4[i][j] += transfTranslacao[i][k] * m3[k][j];
			}
		}
	}

	double* origemCanhaoGlobal = new double[4];
	for (i = 0; i < 4; i++) {
		origemCanhaoGlobal[i] = 0.0;
		for (j = 0; j < 4; j++) {
			origemCanhaoGlobal[i] += m4[i][j] * canhaoOrigemRelCarro[j];
		}
	}

	deleteMat(m1, 4);
	deleteMat(m2, 4);
	deleteMat(m4, 4);

	//Determina a posição da saída do canhão
	double anguloTotal = anguloCarro + anguloCanhaoH;
	double transfMatrixRot4[][4] = {
										{cos(anguloCanhaoH * DEG2RAD), 0.0, sin(anguloCanhaoH * DEG2RAD), 0.0},
										{0.0, 1.0, 0.0, 0.0},
										{-sin(anguloCanhaoH * DEG2RAD), 0.0, cos(anguloCanhaoH * DEG2RAD), 0.0},
										{0.0, 0.0, 0.0, 1.0}
									};
	double transfMatrixRot5[][4] = {
										{1.0, 0.0, 0.0, 0},
										{0, cos(anguloCanhaoV * DEG2RAD), -sin(anguloCanhaoV * DEG2RAD), 0},
										{0, sin(anguloCanhaoV * DEG2RAD), cos(anguloCanhaoV * DEG2RAD), 0},
										{0.0, 0.0, 0.0, 1.0}
									};
	double transfTranslacaoFinal[][4] = {
										{1.0, 0.0, 0.0, origemCanhaoGlobal[0]},
										{0.0, 1.0, 0.0, origemCanhaoGlobal[1]},
										{0.0, 0.0, 1.0, origemCanhaoGlobal[2]},
										{0.0, 0.0, 0.0, 1.0}
	};

	double saidaCanhaoRelacaoCanhao[] = {-0.025, this->getVerticalAdjCanhao(), this->getLengthCanhao(), 1.0};
	double saidaCanhaoParalelaRelacaoCanhao[] = {-0.025, 0.0, this->getLengthCanhao(), 1.0};

	//Ordem das matrizes: TM3R4R5

	double** m5 = new double*[4];
	for (i = 0; i < 4; i++) {
		m5[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m5[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m5[i][j] += transfMatrixRot4[i][k] * transfMatrixRot5[k][j];
			}
		}
	}

	double** m6 = new double*[4];
	for (i = 0; i < 4; i++) {
		m6[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m6[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m6[i][j] += m3[i][k] * m5[k][j];
			}
		}
	}

	double** m7 = new double*[4];
	for (i = 0; i < 4; i++) {
		m7[i] = new double[4];
		for (j = 0; j < 4; j++) {
			m7[i][j] = 0.0;
			for(k = 0; k < 4; k++) {
				m7[i][j] += transfTranslacaoFinal[i][k] * m6[k][j];
			}
		}
	}

	double* saidaCanhaoGlobal = new double[4];
	for (i = 0; i < 4; i++) {
		saidaCanhaoGlobal[i] = 0.0;
		for (j = 0; j < 4; j++) {
			saidaCanhaoGlobal[i] += m7[i][j] * saidaCanhaoRelacaoCanhao[j];
		}
	}

	double* saidaCanhaoParalela = new double[4];
	for (i = 0; i < 4; i++) {
		saidaCanhaoParalela[i] = 0.0;
		for (j = 0; j < 4; j++) {
			saidaCanhaoParalela[i] += m7[i][j] * saidaCanhaoParalelaRelacaoCanhao[j];
		}
	}

	deleteMat(m3, 4);
	deleteMat(m5, 4);
	deleteMat(m6, 4);
	deleteMat(m7, 4);

	double** ptr = new double*[3];
	ptr[0] = saidaCanhaoGlobal;
	ptr[1] = origemCanhaoGlobal;
	ptr[2] = saidaCanhaoParalela;

	delete [] canhaoProp;
	delete [] scale;

	return ptr;
}

Tiro* Carro::atirar() {

	const double anguloCarro = this->getAngCarro();
	const double anguloCanhaoH = this->getAngCanhaoH();
	const double anguloCanhaoV = this->getAngCanhaoV();
	double** ptr = this->calcularPontosCanhao();
	double* scale = this->getEscala();

	Ponto p;
	p.x = ptr[0][0];
	p.y = ptr[0][1];
	p.z = ptr[0][2];

	Tiro* novoTiro = new Tiro(p, CANHAO_WIDTH * scale[0], _velTiro, anguloCarro + anguloCanhaoH + 90, -anguloCanhaoV);

	deleteMat(ptr, 3);
	delete [] scale;

	return novoTiro;

}

void Carro::desenhaAcopl2D(double width, double height, double proportion, int top, double angulo) {
	if (!top)
		top = -1;
	else
		top = 1;

	glPushMatrix();
	glTranslatef(width * -sin(angulo * DEG2RAD)/2, top*height*proportion/2, 0);
	glRotatef(angulo, 0, 0, 1);
	desenhaRetangulo(ACOPL_WIDTH, ACOPL_HEIGHT, corRodas[0], corRodas[1], corRodas[2]);
	glTranslatef(0, ACOPL_HEIGHT, 0);
	if (top == 1)
		glRotatef(_angRodas, 0, 0, 1);
	desenhaRetangulo(RODA_WIDTH, RODA_HEIGHT, corRodas[0], corRodas[1], corRodas[2]);

	int direction = angulo/abs(angulo);

	//Desenha as ranhuras
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
		glVertex2f(direction * _posRanhuras * RODA_WIDTH, 0);
		glVertex2f(direction * _posRanhuras * RODA_WIDTH, RODA_HEIGHT);
		glVertex2f(direction * (_posRanhuras - DIST_RANHURAS) * RODA_WIDTH, 0);
		glVertex2f(direction* (_posRanhuras - DIST_RANHURAS) * RODA_WIDTH, RODA_HEIGHT);
	glEnd();
	glPopMatrix();
}

void Carro::desenhar2D() {
	//O carro assume que o raio base é de 1
	glPushMatrix();
		//Escala ao círculo
		glScalef(_circ->raio*2, _circ->raio*2, 1);
		//Rotaciona o carro
		glRotatef(_angCarro, 0, 0, 1);
		//Desenha a base
		glPushMatrix();
			glTranslatef(0, -BASE_HEIGHT/2, 0);
			desenhaRetangulo(BASE_WIDTH, BASE_HEIGHT, corChassis[0], corChassis[1], corChassis[2]);
		glPopMatrix();

		//Desenha os acoplamentos e as rodas

		//Canto inferior esquerdo
		desenhaAcopl2D(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 0, 90);

		//Canto superior esquerdo
		desenhaAcopl2D(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 1, 90);

		//Canto superior direito
		desenhaAcopl2D(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 1, -90);

		//Canto inferior direito
		desenhaAcopl2D(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 0, -90);

		//Desenha o canhão
		glPushMatrix();
			glTranslatef(0, BASE_HEIGHT/2, 0);
			glRotatef(this->getAngCanhaoH(), 0, 0, 1);
			desenhaRetangulo(CANHAO_WIDTH, CANHAO_HEIGHT, corCanhao[0], corCanhao[1], corCanhao[2]);
		glPopMatrix();

	glPopMatrix();
}

void Carro::desenhar3D() {
	//O carro assume que o raio base é de 1
	glPushMatrix();
		//Escala ao círculo
		double* scale = this->getEscala();
		glScalef(scale[0], scale[1], scale[2]);
		delete [] scale;

		//Rotaciona e posiciona o carro
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		glRotatef(this->getAngCarro(), 0, 1, 0);

		//Desenha a base
		glColor3f(1,1,1);
		this->desenharBodyPart();
		this->desenharInterior();
		this->desenharEngine();
		this->desenharChassi();

		//Desenha as rodas dianteiras
		float* wheels = this->getProporcaoRodas();

		glPushMatrix();
			glTranslatef(wheels[0], wheels[1], wheels[2]);
			glRotatef(this->getAngRodas(), 0, 1, 0);
			glRotatef(this->getAngRodasGiro(), 1, 0, 0);
			glColor3f(1,1,1);
			this->desenharRoda(frontwheel);
		glPopMatrix();

		glPushMatrix();
			glScalef(-1, 1, 1);
			glTranslatef(wheels[0], wheels[1], wheels[2]);
			glRotatef(-this->getAngRodas(), 0, 1, 0);
			glRotatef(this->getAngRodasGiro(), -1, 0, 0);
			glColor3f(1,1,1);
			this->desenharRoda(frontwheel);
		glPopMatrix();

		//Desenha as rodas traseiras
		glPushMatrix();
			glTranslatef(wheels[0], wheels[1], -wheels[2]);
			glRotatef(this->getAngRodasGiro(), 1, 0, 0);
			glColor3f(1,1,1);
			this->desenharRoda(frontwheel);
		glPopMatrix();

		glPushMatrix();
			glScalef(-1, 1, 1);
			glTranslatef(wheels[0], wheels[1], -wheels[2]);
			glRotatef(this->getAngRodasGiro(), -1, 0, 0);
			glColor3f(1,1,1);
			this->desenharRoda(frontwheel);
		glPopMatrix();

		delete [] wheels;

		//Desenha o canhão
		glPushMatrix();
			double* prop = this->getProporcaoCanhao();
			glTranslatef(prop[0], prop[1], prop[2]);
			glRotatef(this->getAngCanhaoH(), 0, 1, 0);
			glRotatef(this->getAngCanhaoV(), 1, 0, 0);
			this->desenharCanhao();
			delete [] prop;
		glPopMatrix();

	glPopMatrix();
}





//Carro Jogador

Tiro* CarroJogador::atirar() {
	Tiro* t = Carro::atirar();
	t->setPlayerShot(true);
	return t;
}

void CarroJogador::andar(int direction, GLdouble timeDiff) {

	//Ajusta o posicionamento das ranhuras
	this->moverRanhuras(direction, timeDiff);

	//Gira a roda
	this->girarRodas(direction, timeDiff);

	//Anda com o carro
	double angulo = _angCarro;
	_circ->centro.y += direction * timeDiff * _velCarro * cos(angulo*DEG2RAD);
	_circ->centro.x += direction * timeDiff * _velCarro * -sin(angulo*DEG2RAD);
}




//Carro Inimigo

void CarroInimigo::atirar(std::list<Tiro*>& listTiros, GLdouble timeDiff) {
	const double timeRate = 1/(_fireRate * 1000);
	_fireTime += timeDiff / 1000;
	for (int tiros = (int) (_fireTime * _fireRate * 1000); tiros > 0; tiros--) {
		Tiro* t = Carro::atirar();
		t->setPlayerShot(false);
		listTiros.push_back(t);
		_fireTime -= timeRate;
	}

}

void CarroInimigo::alinharAngulo() {
	const double vetorBase[] = {1, 0};
	const double vetorCarro[] = {this->getPosicao().x, this->getPosicao().y};
	double escalar = vetorBase[0] * vetorCarro[0] + vetorBase[1] * vetorCarro[1];
	double normaCarro = sqrt(vetorCarro[0]*vetorCarro[0] + vetorCarro[1]*vetorCarro[1]);
	double normaBase = 1.0;
	double cos = escalar / (normaBase * normaCarro);
	double angulo = acos(cos) * (1 / DEG2RAD);
	if (vetorCarro[1] < 0)
		_angCarro = -angulo;
	else
		_angCarro = angulo;
	this->setRaioMovimento(sqrt(pow(vetorCarro[0], 2) + pow(vetorCarro[1], 2)));

	const double raio = this->getRaioMovimento();
	const double velAng = this->getVelCarro() / raio;

	this->setAngRodas(velAng * 1000 * 1/DEG2RAD);
}

Ponto CarroInimigo::andar(GLdouble timeDiff) {
	const double raio = this->getRaioMovimento();
	const double velAng = this->getVelCarro() / raio;

	//Adiciona ao tempo de ré
	if (this->getDirecaoMovimento() < 0)
		_reverseTime += timeDiff / 1000;

	//Ajusta o angulo das rodas
	this->setAngRodas(velAng * 1000 * 1/DEG2RAD);

	//Ajusta o posicionamento das ranhuras
	this->moverRanhuras(this->getDirecaoMovimento(), timeDiff);

	//Gira a roda
	this->girarRodas(this->getDirecaoMovimento(), timeDiff);

	_circ->centro.y += this->getDirecaoMovimento() * cos(_angCarro*DEG2RAD) * timeDiff * this->getVelCarro();
	_circ->centro.x += this->getDirecaoMovimento() * -sin(_angCarro*DEG2RAD) * timeDiff * this->getVelCarro();
	return this->getPosicao();
}








//Tiro

//Getters
Circulo Tiro::getCirculo() {
	return *_circ;
}
GLUquadric* Tiro::getEsfera() {
	return _esfera;
}
Ponto Tiro::getPosicao() {
	return _circ->centro;
}
bool Tiro::isPlayerShot() {
	return _isPlayerShot;
}

//Setters
void Tiro::setPlayerShot(bool playerShot) {
	_isPlayerShot = playerShot;
	if (!playerShot) {
		_circ->fill.r = 1.0;
		_circ->fill.g = 0.3;
		_circ->fill.b = 0.3;
	}
	else {
		_circ->fill.r = 1.0;
		_circ->fill.g = 1.0;
		_circ->fill.b = 0.0;
	}
}

//Métodos
bool Tiro::colisaoCarro(Carro* carro) {
	return (carro->isPlayer() != this->isPlayerShot()) && colisaoCirc(carro->getCirculo(), this->getCirculo());
}
void Tiro::updateTiro(double time) {
	_circ->centro.x += _velTiro * time * cos(_angV * DEG2RAD) * cos(_angH * DEG2RAD);
	_circ->centro.y += _velTiro * time * cos(_angV * DEG2RAD) * sin(_angH * DEG2RAD);
	_circ->centro.z += _velTiro * time * sin(_angV * DEG2RAD);
}
void Tiro::desenhar() {
	desenhaCirculo(_circ->raio, _circ->fill.r, _circ->fill.g, _circ->fill.b);
};

void Tiro::desenhar3D() {
	GLfloat matColor[] = {1.0, 1.0, 1.0,1};
	GLfloat matSpecular[] = {1.0, 1.0, 1.0, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	gluSphere(this->getEsfera(), this->getCirculo().raio, 30, 30);
}
