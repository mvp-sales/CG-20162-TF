#include "leitor.h"
#include "camera.h"
#include "Obj.h"
#include "imageloader.h"

using namespace std;

Retangulo* linha = (Retangulo*) malloc(sizeof(Retangulo));
Circulo* pistaExterna = (Circulo*) malloc(sizeof(Circulo));
Circulo* pistaInterna = (Circulo*) malloc(sizeof(Circulo));
list<CarroInimigo*> inimigos;
CarroJogador* jogador = new CarroJogador();
list<Tiro*> tiros;
Camera* camera = new Camera(2, 45, 1, 750);
GLUquadric* o = gluNewQuadric();
GLUquadric* obj2d = gluNewQuadric();

//Dimensões da janela
double janelaLarg;
double janelaAlt;

//Atributos dos inimigos
EnemyAttr* enemyAttributes;

//Flags de controle do jogo
bool gameStart = false;
bool gameWon = false;
bool gameOver = false;
int currentCheckpoint = 0;
bool mapActive = false;
bool lightEnabled = true;
bool nightMode = false;

//Controla o movimento no teclado
int keyState[256];

//Controla o mouse
int lastMouseX = 0;
int lastMouseY = 0;
bool buttonPressed = false;

// Text variable
static char str[2000];
void * font = GLUT_BITMAP_9_BY_15;

//Timer
double timerSeg = 0;
double timerMin = 0;
double timerHour = 0;
GLdouble previousTime = 0;
GLdouble startTime = 0;

GLuint LoadTexture(const char* filename);
void printText2D(GLfloat x, GLfloat y, char* text, GLdouble r, GLdouble g, GLdouble b);

void adjustCamera() {
  char text[300];

  if (camera->getCurrentCamera() == 1) {
    //Câmera no cockpit (ajustar)
    sprintf(text, "Cockpit Camera");
    printText2D(0.1, 0.1, text, 0, 1, 0);

    double ex = jogador->getPosicao().x ;//- 1.3 * jogador->getCirculo().raio / 5;
    double ey = jogador->getPosicao().y;
    double ez = jogador->getAltura();

    double px = ex - (jogador->getCirculo().raio + 60) * sin(jogador->getAngCarro() * DEG2RAD);
    double py = ey + (jogador->getCirculo().raio + 60) * cos(jogador->getAngCarro() * DEG2RAD);
    double pz = jogador->getAltura();

    camera->lookAt(ex, ey, ez,
         px, py, pz,
         0, 0, 1);
  }
  else if (camera->getCurrentCamera() == 2) {
    //Câmera do canhão (ajustar)

    sprintf(text, "Cannon Camera");
    printText2D(0.1, 0.1, text, 0, 1, 0);
    double** cannonPoints = jogador->calcularPontosCanhao();
    double* saida = cannonPoints[0];
    double* origem = cannonPoints[1];

    camera->lookAt(origem[0], origem[1], origem[2] + 0.5 * jogador->getAltura(),
         saida[0], saida[1], saida[2],
         0, 0, 1);

  }
  else if (camera->getCurrentCamera() == 3) {
    //Câmera atrás do carro, seguindo sua posição
    sprintf(text, "Dynamic Camera");
    printText2D(0.1, 0.1, text, 0, 1, 0);

    const float distCamera = 120;
    float horDistCamera = distCamera * cos(camera->getAngleVertical() * DEG2RAD);
    float vertDistCamera = distCamera * sin (camera->getAngleVertical() * DEG2RAD);

    float theta = jogador->getAngCarro() + camera->getAngleHorizontal();

    float xOffsetCamera = horDistCamera * sin(theta * DEG2RAD);
    float yOffsetCamera = horDistCamera * cos(theta * DEG2RAD);

    double px = jogador->getPosicao().x;
    double py = jogador->getPosicao().y;
    double pz = jogador->getAltura();

    double ex = px - xOffsetCamera;
    double ey = py - yOffsetCamera;
    double ez = pz + vertDistCamera;


    camera->lookAt(ex, ey, ez,
         px, py, pz,
         0, 0, 1);
  }
}

void printText2D(GLfloat x, GLfloat y, char* text, GLdouble r, GLdouble g, GLdouble b)
{
  //Create a string to be printed
  //Draw text considering a 2D space (disable all 3d features)
  glMatrixMode (GL_PROJECTION);
  //Push to recover original PROJECTION MATRIX
  glPushMatrix();
  glLoadIdentity ();
  glOrtho (0, 1, 0, 1, -1, 1);
  glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    char *tmpStr;
    //Adjust the color
    glColor3f(r, g, b);
    //Define the position to start printing
    glRasterPos2f(x, y);
    //Print  the first Char with a certain font
    tmpStr = text;
    //Print each of the other Char at time
    while( *tmpStr ){
      glutBitmapCharacter(font, *tmpStr);
      tmpStr++;
    }
  glPopAttrib();
  glPopMatrix();
  glMatrixMode (GL_MODELVIEW);
}

void displayGame2D() {

	//Desenha a pista externa
	glPushMatrix();
	glTranslatef(pistaExterna->centro.x, pistaExterna->centro.y, 0);
  glColor3f(pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
  gluQuadricDrawStyle(obj2d, GLU_SILHOUETTE);
  gluDisk(obj2d, 0, pistaExterna->raio, 20, 1);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna->centro.x, pistaInterna->centro.y, 0);
	glColor3f(pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
  gluDisk(obj2d, 0, pistaInterna->raio, 20, 1);
	glPopMatrix();

	//Desenha a pista
	glPushMatrix();
	glTranslatef(linha->vEsqSup.x + linha->largura/2, linha->vEsqSup.y - linha->altura, 0);
	desenhaRetangulo(linha->largura, linha->altura, linha->fill.r, linha->fill.g, linha->fill.b);
	glPopMatrix();

	//Desenha o jogador
	if (jogador != NULL) {
		glPushMatrix();
		glTranslatef(jogador->getPosicao().x, jogador->getPosicao().y, 0);
		jogador->desenhar2D();
		glPopMatrix();
	}

	//Desenha os círculos dos adversários
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* t = *it;
		glPushMatrix();
		glTranslatef(t->getPosicao().x, t->getPosicao().y, 0);
		t->desenhar2D();
		glPopMatrix();
	}
}

void displayGame3D() {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);

	//Desenha a pista externa
	glPushMatrix();
	glTranslatef(pistaExterna->centro.x, pistaExterna->centro.y, 0);
  glColor3f(pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
  gluDisk(o, 0, pistaExterna->raio, 30, 3);
  glColor3f(0, 1, 1);
  gluCylinder(o, pistaExterna->raio, pistaExterna->raio, jogador->getAltura() * 4, 30, 2);
  glTranslatef(0, 0, jogador->getAltura() * 4);
  glColor3f(pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
  //gluDisk(o, 0, pistaExterna->raio, 30, 3);
  //desenhaCirculo(pistaExterna->raio, pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna->centro.x, pistaInterna->centro.y, 0);
	glColor3f(pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
  gluCylinder(o, pistaInterna->raio, pistaInterna->raio, jogador->getAltura() * 2, 30, 2);
	glPopMatrix();

	//Desenha a pista
	glPushMatrix();
	glTranslatef(linha->vEsqSup.x + linha->largura/2, linha->vEsqSup.y - linha->altura, 0);
	desenhaRetangulo(linha->largura, linha->altura, linha->fill.r, linha->fill.g, linha->fill.b);
	glPopMatrix();

  glPopAttrib();

	//Desenha o tiro
	for(list<Tiro*>::iterator it = tiros.begin(); it != tiros.end(); ++it) {
		Tiro* t = *it;
		glPushMatrix();
		glTranslatef(t->getPosicao().x, t->getPosicao().y, t->getPosicao().z);
		t->desenhar3D();
		glPopMatrix();
	}

	//Desenha o jogador
	if (jogador != NULL) {
		glPushMatrix();
		glTranslatef(jogador->getPosicao().x, jogador->getPosicao().y, 0);
		jogador->desenhar3D();
		glPopMatrix();
	}

	//Desenha os círculos dos adversários
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* t = *it;
		glPushMatrix();
		glTranslatef(t->getPosicao().x, t->getPosicao().y, 0);
		t->desenhar3D();
		glPopMatrix();
	}
}

void displayMap() {
  glMatrixMode (GL_PROJECTION);
  //Push to recover original PROJECTION MATRIX
	glPushMatrix();
    glLoadIdentity();
    glOrtho (-pistaExterna->raio, pistaExterna->raio, -pistaExterna->raio, pistaExterna->raio, -1, 1);
  	glPushAttrib(GL_ENABLE_BIT);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      	glTranslatef(0.750 * pistaExterna->raio, -0.750 * pistaExterna->raio , 1);
      	glScalef(0.25, 0.25, 1);
      	displayGame2D();
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
    glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//Funcao que sera chamada toda vez que a janela for repintada.
void display() {
	//Limpa a tela com a cor especificada
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Ativa a visão do modelo
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  //Mostra o mapa na tela
	if (mapActive) {
		glPushMatrix();
		displayMap();
		glPopMatrix();
	}

  char text[300];

	//Desenha o tempo na tela
	sprintf(text, "Tempo: %02d:%02d:%02d", (int) timerHour, (int) timerMin, (int) timerSeg);
	printText2D(0.7, 0.98, text, 1, 1, 1);

	if (gameOver) {
		if (gameWon)
		  sprintf(text, "Voce venceu");
		else
		  sprintf(text, "Voce perdeu");
		printText2D(0.44, 0.5, text, 1, 1, 1);
	}

	glPushMatrix();

  adjustCamera();


  GLfloat light_position[] = {0, 0, 80, 1};
  GLfloat light_ambient[] = {1, 1, 1, 1};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glPushMatrix();
	displayGame3D();
	glPopMatrix();

	glPopMatrix();

  //Desenha na tela
  glutSwapBuffers();
}

void keyRelease(unsigned char key, int x, int y) {
	keyState[key] = 0;
}

void keyPress(unsigned char key, int x, int y) {
	keyState[key] = 1;

	switch(key) {
		case '1':
		case '2':
		case '3':
			camera->toggleCamera((int)key - (int)'0');
			break;
		case 'm':
		case 'M':
			mapActive = !mapActive;
      break;
    case 'n':
    case 'N':
      nightMode = !nightMode;
      break;
    case 'l':
    case 'L':
      if (!lightEnabled)
        glEnable(GL_LIGHTING);
      else
        glDisable(GL_LIGHTING);
      lightEnabled = !lightEnabled;
			break;
	}

	if (!gameStart && (key == 'W' || key == 'w')) {
		gameStart = true;
		startTime = previousTime;
	}
}

void gameRun(GLdouble currentTime, GLdouble timeDifference) {
	double time = currentTime - startTime;
    timerSeg = (int) (time / 1000) % 60;
    timerMin = (int) (time / 1000) / 60;
    timerHour = (int) (time / 1000) / 3600;
    Ponto previousPos;
    if (jogador != NULL) {

		previousPos = jogador->getPosicao();
		int direction = 0;
		//Movimenta o jogador
		if (keyState['W'] || keyState['w']) {
			direction = 1;
			jogador->andar(direction, timeDifference);
		}
		if (keyState['S'] || keyState['s']) {
			direction = -1;
			jogador->andar(direction, timeDifference);
		}
		if (keyState['A'] || keyState['a'])
			jogador->virarRoda(3);
		if (keyState['D'] || keyState['d'])
			jogador->virarRoda(-3);

		//Testa se houve colisão
		if (jogador->colisaoCarro(jogador, inimigos) || colisaoCirc(*pistaInterna, jogador->getCirculo()) || !dentroCirc(*pistaExterna, jogador->getCirculo()))
			jogador->setPosicao(previousPos); //retorna à posição original
		else
			jogador->virarCarro(jogador->getTurnRate() * direction, timeDifference);
	}

	//Atualiza os tiros e confere se algum carro foi acertado
	for(list<Tiro*>::iterator it = tiros.begin(); it != tiros.end(); ++it) {
		Tiro* t = *it;
		t->updateTiro(timeDifference);
		const double raio = pistaExterna->raio;
		if (outOfBounds(t->getPosicao(), -raio, raio, -raio, raio)) {
			it = tiros.erase(it);
			delete t;
		}
		else if (jogador != NULL && t->colisaoCarro(jogador)) {
			it = tiros.erase(it);
			delete t;
			delete jogador;
			jogador = NULL;
			gameOver = true;
		}
		else {
			for(list<CarroInimigo*>::iterator its = inimigos.begin(); its != inimigos.end(); ++its) {
				CarroInimigo* ci = *its;
				if (t->colisaoCarro(ci)) {
					its = inimigos.erase(its);
					delete ci;
					it = tiros.erase(it);
					delete t;
					break;
				}
			}
		}
	}


	//Movimento e tiro automático dos inimigos
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* ci = *it;
		ci->atirar(tiros, timeDifference);
		Ponto antigo = ci->getPosicao();
		ci->andar(timeDifference);
		if (jogador != NULL && colisaoCirc(jogador->getCirculo(), ci->getCirculo())) {
				ci->reverterMovimento();
			ci->setPosicao(antigo);
		}
		else if (ci->colisaoCarro(ci, inimigos) || colisaoCirc(*pistaInterna, ci->getCirculo()) || !dentroCirc(*pistaExterna, ci->getCirculo())) {
			ci->reverterMovimento();
			ci->setPosicao(antigo);
		}
		else {
			if (ci->getDirecaoMovimento() < 0 && ci->getTempoReverso() >= 1) {
				ci->reverterMovimento();
				ci->alinharAngulo();
			}
			ci->virarCarro(ci->getDirecaoMovimento() * ci->getTurnRate(), timeDifference);
		}
	}

	if (jogador != NULL) {
		//Confere se o jogador atingiu um checkpoint
		Ponto currentPos = jogador->getPosicao();
		switch(currentCheckpoint) {
			case 0:
				//Topo da pista
				if (previousPos.x >= 0 && currentPos.x <= 0 && previousPos.y >= 0)
					currentCheckpoint++;
				break;
			case 1:
				//Lateral esquerda da pista
				if (previousPos.y >= 0 && currentPos.y <= 0 && previousPos.x <= 0)
					currentCheckpoint++;
				break;
			case 2:
				//Fundo da pista
				if (previousPos.x <= 0 && currentPos.x >= 0 && previousPos.y <= 0)
					currentCheckpoint++;
				break;
			case 3:
				//Lateral direita da pista
				if (previousPos.y <= 0 && currentPos.y >= 0 && previousPos.x >= 0) {
					gameWon = true;
					gameOver = true;
				}
				break;
		}
	}
}

void idle(void) {
    GLdouble currentTime;
    GLdouble timeDifference;

    // Elapsed time from the initiation of the game.
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeDifference = currentTime - previousTime; // Elapsed time from the previous frame.
    previousTime = currentTime; //Update previous time
	  if (gameStart && !gameOver)
		  gameRun(currentTime, timeDifference);
	  glutPostRedisplay();
}

void mouseDrag(int x, int y) {
  if (!buttonPressed) {
  	if (jogador != NULL && gameStart && !gameOver && !buttonPressed) {
  		jogador->virarCanhaoH( (x < lastMouseX) - (lastMouseX < x) );
      jogador->virarCanhaoV( 0.5 * ((lastMouseY < y) - (y < lastMouseY)) );
    }
    else return;
  }
  else if (camera->getCurrentCamera() == 3) {
    camera->giroVertical(y - lastMouseY);
    camera->giroHorizontal(x - lastMouseX);
  }
  else return;
	lastMouseX = x;
  lastMouseY = y;
}

void mousePress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//cria o tiro
		if (jogador != NULL && gameStart && !gameOver)
			tiros.push_back(jogador->atirar());
	}
  else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    //salva a posiçao atual
    lastMouseX = x;
    lastMouseY = y;
    buttonPressed = true;
  }
  else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
    buttonPressed = false;
  }
}

void reshape (int w, int h) {

    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    janelaLarg = w;
    janelaAlt = h;
    camera->updateCamera(w, h);
}


void init() {
    glClearColor(1.0, 1.0, 1.0, 0);
    //Modelo 3D
    glEnable(GL_DEPTH_TEST);
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_LIGHTING);
    glShadeModel (GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHT0);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    GLuint enTextures[5] = {
              LoadTexture("Texturas/BuggyTexturas/BuggyBodyPart.bmp"),
              LoadTexture("Texturas/BuggyTexturas/BuggyChassi.bmp"),
              LoadTexture("Texturas/BuggyTexturas/BuggyEngine.bmp"),
              LoadTexture("Texturas/BuggyTexturas/BuggyInterior.bmp"),
              LoadTexture("Texturas/BuggyTexturas/BuggyWheel.bmp")
            };
    GLuint plTextures[5] = {
      LoadTexture("Texturas/InterceptorTexturas/InterceptorBodyPart.bmp"),
      LoadTexture("Texturas/InterceptorTexturas/InterceptorChassi.bmp"),
      LoadTexture("Texturas/InterceptorTexturas/InterceptorEngine.bmp"),
      LoadTexture("Texturas/InterceptorTexturas/InterceptorInterior.bmp"),
      LoadTexture("Texturas/InterceptorTexturas/InterceptorWheel.bmp")
    };
    for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
  		CarroInimigo* t = *it;
  		t->setTexturas(enTextures);
  	}
    jogador->setTexturas(plTextures);
}

int main(int argc, char** argv) {
	readXML(argv[1], "config.xml", enemyAttributes, jogador, inimigos, pistaInterna, pistaExterna, linha);
	janelaLarg = 500;
	janelaAlt = 700;
   	glutInit(&argc, argv);
   	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   	glutInitWindowSize(janelaLarg, janelaAlt);
   	glutCreateWindow("Pista"); // Cria a janela com o título especificado
   	//glutReshapeWindow(janelaLarg, janelaAlt);   // Set the window's initial width & height
   	init();
   	glutDisplayFunc(display); // Registre a função que desenha na tela
   	glutKeyboardFunc(keyPress);
   	glutKeyboardUpFunc(keyRelease);
   	glutIdleFunc(idle);
   	glutMouseFunc(mousePress);
   	glutPassiveMotionFunc(mouseDrag);
   	glutMotionFunc(mouseDrag);
   	glutReshapeFunc(reshape);
   	glutMainLoop();
   	return 0;
}

GLuint LoadTexture(const char* filename) {
  GLuint texture;
  Image* image = loadBMP(filename);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

  delete image;
  return texture;
}
