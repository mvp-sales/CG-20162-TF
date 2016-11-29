#include "carro.h"

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

GLuint Carro::getTextura(int i) {
	return texturas[i];
}

double Carro::getVelCarro() {
	return _velCarro;
}

double Carro::getTurnRate() {
	return _angRodas/1000;
}
double Carro::getAltura() {
	return 1.0;
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

void Carro::desenharModelo() {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionChassis);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matColorChassis);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularChassis);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessChassis);
	chassi->draw();
}

void Carro::desenharRoda() {
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionRodas);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matColorRodas);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecularRodas);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininessRodas);

	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, this->getTextura(TEX_WHEEL));

	rodas->draw();

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

void Carro::virarCanhao(double ang) {
	_angCanhaoH += ang;
	if (_angCanhaoH < -LIMITE_ANGULO) {
		_angCanhaoH = -LIMITE_ANGULO;
	}
	else if (_angCanhaoH > LIMITE_ANGULO) {
		_angCanhaoH = LIMITE_ANGULO;
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

Tiro* Carro::atirar() {
	//Determina a posição do canhão
	const double anguloCarro = this->getAngCarro();
	const double anguloCanhao = this->getAngCanhaoH();
	const double transfMatrixCarro[][3] = {
											{cos(anguloCarro * DEG2RAD), -sin(anguloCarro * DEG2RAD), _circ->centro.x},
											{sin(anguloCarro * DEG2RAD), cos(anguloCarro * DEG2RAD), _circ->centro.y},
											{0.0, 0.0, 1.0}
										};
	const double origemCanhaoRelacaoCarro[3] = {0.0, BASE_HEIGHT * _circ->raio, 1.0};
	double origemCanhaoGlobal[3];
	int i,j;
	for (i = 0; i < 3; i++) {
		origemCanhaoGlobal[i] = 0.0;
		for (j = 0; j < 3; j++) {
			origemCanhaoGlobal[i] += transfMatrixCarro[i][j]*origemCanhaoRelacaoCarro[j];
		}
	}


	//Determina a posição da saída do canhão
	double angTotal = anguloCarro + anguloCanhao;
	const double transfMatrix[][3] = {
										{cos(angTotal * DEG2RAD), -sin(angTotal * DEG2RAD), origemCanhaoGlobal[0]},
										{sin(angTotal * DEG2RAD), cos(angTotal * DEG2RAD), origemCanhaoGlobal[1]},
										{0.0, 0.0, 1.0}
									};
	const double saidaCanhaoRelacaoCanhao[3] = {0.0, CANHAO_HEIGHT * 2 * _circ->raio, 1.0};
	double saidaCanhaoGlobal[3];
	for (i = 0; i < 3; i++) {
		saidaCanhaoGlobal[i] = 0.0;
		for (j = 0; j < 3; j++) {
			saidaCanhaoGlobal[i] += transfMatrix[i][j]*saidaCanhaoRelacaoCanhao[j];
		}
	}

	Ponto p;
	p.x = saidaCanhaoGlobal[0];
	p.y = saidaCanhaoGlobal[1];

	return new Tiro(p, CANHAO_WIDTH * _circ->raio, _velTiro, angTotal + 90);

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

void Carro::desenhaAcopl3D(double width, double height, double proportion, int top, double angulo) {
	if (!top)
		top = -1;
	else
		top = 1;

	double acoplRadius = ACOPL_WIDTH / 2.0;
	double acoplLength = ACOPL_HEIGHT;
	double wheelRadius = RODA_WIDTH / 2.0;
	double wheelLength = RODA_HEIGHT;

	glPushMatrix();
	//Desenha o acoplamento
	glTranslatef(width * -sin(angulo * DEG2RAD)/2, top*height*proportion/2, 0);
	glRotatef(angulo, 0, 0, 1);
	glColor3f(corRodas[0], corRodas[1], corRodas[2]);
	DrawCylinder(acoplRadius, acoplLength);

	//Desenha a roda
	glTranslatef(0, acoplLength, 0);
	if (top == 1)
		glRotatef(this->getAngRodas(), 0, 0, 1);
	glRotatef(this->getAngRodasGiro(), 0, 1, 0);
	DrawCylinder(wheelRadius, wheelLength);
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
		if (this->isPlayer())
			glScalef(_circ->raio / 5, _circ->raio / 6, _circ->raio / 5);
		else
			glScalef(_circ->raio / 4, _circ->raio / 5, _circ->raio / 5);

		//Rotaciona e posiciona o carro
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		glRotatef(_angCarro, 0, 1, 0);

		//Desenha a base
		glPushMatrix();
			glColor3f(corChassis[0], corChassis[1], corChassis[2]);
			this->desenharModelo();
			//glutSolidCube(1);
		glPopMatrix();

		//Desenha as rodas dianteiras
		glPushMatrix();
			glTranslatef(2, 0.76, 3.15);
			glRotatef(this->getAngRodas(), 0, 1, 0);
			glRotatef(this->getAngRodasGiro(), 1, 0, 0);
			glColor3f(corRodas[0], corRodas[1], corRodas[2]);
			this->desenharRoda();
		glPopMatrix();

		glPushMatrix();
			glScalef(-1, 1, 1);
			glTranslatef(2, 0.76, 3.15);
			glRotatef(-this->getAngRodas(), 0, 1, 0);
			glRotatef(this->getAngRodasGiro(), -1, 0, 0);
			glColor3f(corRodas[0], corRodas[1], corRodas[2]);
			this->desenharRoda();
		glPopMatrix();

		//Desenha o canhão
		glPushMatrix();
			//glTranslatef(0, BASE_HEIGHT/2, BASE_LENGTH / 2);
			//Rotação horizontal
			//glRotatef(this->getAngCanhaoH(), 0, 0, 1);
			//Rotação vertical
			//glRotatef(this->getAngCanhaoV(), 1, 0, 0);
			//glColor3f(corCanhao[0], corCanhao[1], corCanhao[2]);
			//DrawCylinder(CANHAO_WIDTH / 2.0, CANHAO_HEIGHT);
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
	_circ->centro.x += _velTiro * time * cos(_ang * DEG2RAD);
	_circ->centro.y += _velTiro * time * sin(_ang * DEG2RAD);
}
void Tiro::desenhar() {
	desenhaCirculo(_circ->raio, _circ->fill.r, _circ->fill.g, _circ->fill.b);
};
