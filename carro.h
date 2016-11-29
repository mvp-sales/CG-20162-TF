#ifndef CARRO_H
#define CARRO_H

#include "figuras.h"
#include "Obj.h"
#include <list>

#define LIMITE_ANGULO 45
#define BASE_WIDTH 0.35
#define BASE_HEIGHT 0.65
#define BASE_LENGTH 0.25
#define ACOPL_PROPORTION 0.6
#define ACOPL_WIDTH 0.08
#define ACOPL_HEIGHT 0.1
#define RODA_WIDTH 0.25
#define RODA_HEIGHT 0.1
#define CANHAO_WIDTH 0.1
#define CANHAO_HEIGHT 0.2

#define DIST_RANHURAS 0.5

#define TEX_BODYPART 0
#define TEX_CHASSI 1
#define TEX_ENGINE 2
#define TEX_INTERIOR 3
#define TEX_WHEEL 4

typedef struct {
	double inimigoFireRate;
	double inimigoVelCarro;
	double inimigoVelTiro;
} EnemyAttr;

class Carro;
class CarroInimigo;

class Tiro {
	private:
		Circulo* _circ;
		Esfera* _esfera;
		float _ang;
		float _velTiro;
		bool _isPlayerShot;
	public:
		Tiro(Ponto p, double raio, double velTiro, double ang) {
			_circ = (Circulo*) malloc(sizeof(Circulo));
			_circ->centro = p;
			_circ->raio = raio;
			_velTiro = velTiro;
			_ang = ang;
			_esfera = CreateSphere(raio, 16);
		};
		~Tiro() {
			delete _circ;
		};
		Circulo getCirculo();
		Esfera* getEsfera();
		Ponto getPosicao();
		bool isPlayerShot();
		void setPlayerShot(bool playerShot);
		bool colisaoCarro(Carro* carro);
		void updateTiro(double time);
		void desenhar();
		//void desenhar3D();
};

class Carro {
	protected:
		Circulo* _circ;
		double _velTiro;
		double _velCarro;
		double _angCarro;
		double _angCanhaoH;
		double _angCanhaoV;
		double _angRodas;
		double _angRodasGiro;
		double _posRanhuras;
		bool _isPlayer;
		GLfloat corChassis[3];
		GLfloat matEmissionChassis[4];
		GLfloat matColorAChassis[4];
		GLfloat matColorDChassis[4];
		GLfloat matSpecularChassis[4];
		GLfloat matShininessChassis[1];
		GLfloat corRodas[3];
		GLfloat matEmissionRodas[4];
		GLfloat matColorARodas[4];
		GLfloat matColorDRodas[4];
		GLfloat matSpecularRodas[4];
		GLfloat matShininessRodas[1];
		GLfloat corCanhao[3];
		GLfloat matEmissionCanhao[4];
		GLfloat matColorACanhao[4];
		GLfloat matColorDCanhao[4];
		GLfloat matSpecularCanhao[4];
		GLfloat matShininessCanhao[1];
		GLuint texturas[5];
		Obj *chassi, *interior, *engine, *bodypart, *frontwheel, *backwheel, *cannon;

		void desenhaAcopl2D(double width, double height, double proportion, int top, double angulo);
		void desenhaAcopl3D(double width, double height, double proportion, int top, double angulo);

	public:
		Carro() {
			_circ = (Circulo*) malloc(sizeof(Circulo));
			_angCarro = 0;
			_angCanhaoH = 0;
			_angCanhaoV = 0;
			_angRodas = 0;
			_angRodasGiro = 0;
			_posRanhuras = DIST_RANHURAS/2;
		};
		~Carro() {
			delete _circ;
		}
		bool isPlayer();
		double getAltura();
		double getAngRodas();
		double getAngRodasGiro();
		double getAngCarro();
		double getAngCanhaoH();
		double getAngCanhaoV();
		Circulo getCirculo();
		Ponto getPosicao();
		double getVelCarro();
		double getTurnRate();
		GLuint getTextura(int i);
		bool colisaoCarro(Carro* self, std::list<CarroInimigo*>& carros);
		void desenharBodyPart();
		void desenharChassi();
		void desenharEngine();
		void desenharInterior();
		void desenharRoda(Obj* wheel);
		void setAngRodas(double ang);
		void setPosicao(Ponto pos);
		void setRaio(double raio);
		void setTexturas(GLuint texturas[5]);
		void setVelTiro(double vtiro);
		void setVelCarro(double vcarro);
		void girarRodas(int direction, GLdouble timeDiff);
		void virarRoda(double ang);
		void virarCanhao(double ang);
		void virarCarro(double taxa, GLdouble timeDiff);
		void moverRanhuras(int direction, GLdouble timeDiff);
		Tiro* atirar();
		void desenhar2D();
		void desenhar3D();
};

class CarroJogador : public Carro {
	public:
		CarroJogador() : Carro() {
			corChassis[0] = 0.2;
			corChassis[1] = 0.9;
			corChassis[2] = 0.4;
			corRodas[0] = 0.2;
			corRodas[1] = 0.6;
			corRodas[2] =  0.7;
			corCanhao[0] = 0.1;
			corCanhao[1] = 0.7;
			corCanhao[2] = 0.1;
			for (int i = 0; i < 4; i++) {
				if (i < 3) {
					matColorAChassis[i] = 0.2;
					matColorDChassis[i] = 1;
					matColorARodas[i] = 0.2;
					matColorDRodas[i] = 0.2;
					matColorACanhao[i] = 0.2;
					matColorDCanhao[i] = 0.2;
					matEmissionCanhao[i] = 0.3;
					matEmissionRodas[i] = 0.3;
					matEmissionChassis[i] = 0.3;
				}
				else {
					matColorAChassis[i] = 1;
					matColorDChassis[i] = 1;
					matColorARodas[i] = 1;
					matColorDRodas[i] = 1;
					matColorACanhao[i] = 1;
					matColorDCanhao[i] = 1;
					matEmissionCanhao[i] = 1;
					matEmissionRodas[i] = 1;
					matEmissionChassis[i] = 1;
					matShininessCanhao[0] = 100;
					matShininessChassis[0] = 100;
					matShininessRodas[0] = 100;
				}
				matSpecularCanhao[i] = 1;
				matSpecularChassis[i] = 1;
				matSpecularRodas[i] = 1;
			}
			_isPlayer = true;
			bodypart = new Obj();
			bodypart->loadFile("Modelos/InterceptorFiles/InterceptorBodyPart.obj");
			chassi = new Obj();
			chassi->loadFile("Modelos/InterceptorFiles/InterceptorChassi.obj");
			engine = new Obj();
			engine->loadFile("Modelos/InterceptorFiles/InterceptorEngine.obj");
			interior = new Obj();
			interior->loadFile("Modelos/InterceptorFiles/InterceptorInterior.obj");
			frontwheel = new Obj();
			frontwheel->loadFile("Modelos/InterceptorFiles/InterceptorFrontWheel.obj");
			backwheel = new Obj();
			backwheel->loadFile("Modelos/InterceptorFiles/InterceptorBackWheel.obj");
		}
		Tiro* atirar();
		void andar(int direction, GLdouble timeDiff);
};

class CarroInimigo : public Carro {
	private:
		double _fireRate;
		double _fireTime;
		double _raioMovimento;
		double _reverseFlag;
		double _reverseTime;
	public:
		CarroInimigo() : Carro() {
			corChassis[0] = 0.9;
			corChassis[1] = 0.2;
			corChassis[2] = 0.2;
			corRodas[0] = 0.7;
			corRodas[1] = 0.4;
			corRodas[2] =  0.2;
			corCanhao[0] = 0.7;
			corCanhao[1] = 0.1;
			corCanhao[2] = 0.1;
			for (int i = 0; i < 4; i++) {
				if (i < 3) {
					matColorAChassis[i] = 0.2;
					matColorDChassis[i] = 1;
					matColorARodas[i] = 0.2;
					matColorDRodas[i] = 0.2;
					matColorACanhao[i] = 0.2;
					matColorDCanhao[i] = 0.2;
					matEmissionCanhao[i] = 0.3;
					matEmissionRodas[i] = 0.3;
					matEmissionChassis[i] = 0.3;
				}
				else {
					matColorAChassis[i] = 1;
					matColorDChassis[i] = 1;
					matColorARodas[i] = 1;
					matColorDRodas[i] = 1;
					matColorACanhao[i] = 1;
					matColorDCanhao[i] = 1;
					matEmissionCanhao[i] = 1;
					matEmissionRodas[i] = 1;
					matEmissionChassis[i] = 1;
					matShininessCanhao[0] = 100;
					matShininessChassis[0] = 100;
					matShininessRodas[0] = 100;
				}
				matSpecularCanhao[i] = 1;
				matSpecularChassis[i] = 1;
				matSpecularRodas[i] = 1;
			}
			_fireTime = 0;
			_isPlayer = false;
			_reverseFlag = 1;
			_reverseTime = 0;
			bodypart = new Obj();
			bodypart->loadFile("Modelos/BuggyParts/BuggyBodyPart.obj");
			chassi = new Obj();
			chassi->loadFile("Modelos/BuggyParts/BuggyChassi.obj");
			engine = new Obj();
			engine->loadFile("Modelos/BuggyParts/BuggyEngine.obj");
			interior = new Obj();
			interior->loadFile("Modelos/BuggyParts/BuggyInterior.obj");
			frontwheel = new Obj();
			frontwheel->loadFile("Modelos/BuggyParts/BuggyFrontWheel.obj");
			backwheel = new Obj();
			backwheel->loadFile("Modelos/BuggyParts/BuggyBackWheel.obj");
		}
		void reverterMovimento() {
			_reverseTime = 0;
			_reverseFlag = -_reverseFlag;
		}

		double getTempoReverso() {
			return _reverseTime;
		}

		double getDirecaoMovimento() {
			return _reverseFlag;
		}

		double getRaioMovimento() {
			return _raioMovimento;
		}
		void setRaioMovimento(double r) {
			_raioMovimento = r;
		}
		void setFireRate(double fireRate) {
			_fireRate = fireRate;
		}
		double getFireRate() {
			return _fireRate;
		}
		void alinharAngulo();
		void atirar(std::list<Tiro*>& listTiros, GLdouble timeDiff);
		Ponto andar(GLdouble timeDiff);
		void updatePosition();
};

#endif
