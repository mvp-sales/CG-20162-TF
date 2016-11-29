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
Camera* camera = new Camera(2, 60, 1, 550);
GLUquadric* o = gluNewQuadric();

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

//Controla o movimento no teclado
int keyState[256];

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

    double px = jogador->getPosicao().x;
    double py = jogador->getPosicao().y;

    double ex = px + (jogador->getCirculo().raio + 60) * cos(jogador->getAngCarro() * DEG2RAD);
    double ey = py - (jogador->getCirculo().raio + 60) * sin(jogador->getAngCarro() * DEG2RAD);
    double ez = jogador->getAltura() * 40;

    camera->lookAt(ex, ey, ez,
         px, py, 0,
         0, 0, 1);
  }
  else if (camera->getCurrentCamera() == 2) {
    //Câmera do canhão (ajustar)

    sprintf(text, "Cannon Camera");
    printText2D(0.1, 0.1, text, 0, 1, 0);

    double px = jogador->getPosicao().x;
    double py = jogador->getPosicao().y;
    double pz = 300.0;//jogador->getAltura();

    double ex = px;//px + 300 * cos(jogador->getAngCarro() * DEG2RAD);
    double ey = py;//py + 300 * sin(jogador->getAngCarro() * DEG2RAD);
    camera->lookAt(ex, ey, pz,
         px, py, 0,
         0, 1, 0);

  }
  else if (camera->getCurrentCamera() == 3) {
    //Câmera atrás do carro, seguindo sua posição
    sprintf(text, "Dynamic Camera");
    printText2D(0.1, 0.1, text, 0, 1, 0);

    double px = jogador->getPosicao().x;
    double py = jogador->getPosicao().y;
    double pz = jogador->getAltura();

    double ex = px + (jogador->getCirculo().raio + 60) * sin(jogador->getAngCarro() * DEG2RAD);
    double ey = py - (jogador->getCirculo().raio + 60) * cos(jogador->getAngCarro() * DEG2RAD);
    double ez = pz * 40;

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
	desenhaCirculoLinha(pistaExterna->raio, pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
	//desenhaCirculo(pistaExterna->raio, pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna->centro.x, pistaInterna->centro.y, 0);
	glColor3f(pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
	desenhaCirculoLinha(pistaInterna->raio, pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
	//desenhaCirculo(pistaInterna->raio, pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
	glPopMatrix();

	//Desenha a pista
	glPushMatrix();
	glTranslatef(linha->vEsqSup.x + linha->largura/2, linha->vEsqSup.y - linha->altura, 0);
	desenhaRetangulo(linha->largura, linha->altura, linha->fill.r, linha->fill.g, linha->fill.b);
	glPopMatrix();

	//Desenha o tiro
	for(list<Tiro*>::iterator it = tiros.begin(); it != tiros.end(); ++it) {
		Tiro* t = *it;
		glPushMatrix();
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
		t->desenhar();
		glPopMatrix();
	}

	//Desenha o jogador
	if (jogador != NULL) {
		glPushMatrix();
		glTranslatef(jogador->getCirculo().centro.x, jogador->getCirculo().centro.y, 0);
		jogador->desenhar2D();
		glPopMatrix();
	}

	//Desenha os círculos dos adversários
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* t = *it;
		glPushMatrix();
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
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
  gluCylinder(o, pistaExterna->raio, pistaExterna->raio, jogador->getAltura() * 40, 30, 2);
  glTranslatef(0, 0, jogador->getAltura() * 40);
  glColor3f(pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
  //gluDisk(o, 0, pistaExterna->raio, 30, 3);
  //desenhaCirculo(pistaExterna->raio, pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna->centro.x, pistaInterna->centro.y, 0);
	glRotatef(90, 1, 0, 0);
	glColor3f(pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
	DrawCylinder(pistaInterna->raio, 20);
	//DrawSphere(internal, 1);
	//desenhaCirculo(pistaInterna->raio, pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
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
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
		t->desenhar();
		glPopMatrix();
	}

	//Desenha o jogador
	if (jogador != NULL) {
		glPushMatrix();
		glTranslatef(jogador->getCirculo().centro.x, jogador->getCirculo().centro.y, 0);
		jogador->desenhar3D();
		glPopMatrix();
	}

	//Desenha os círculos dos adversários
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* t = *it;
		glPushMatrix();
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
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

	glPushMatrix();

  adjustCamera();
  char text[300];

  GLfloat light_position[] = {0, 1, 1, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glPushMatrix();
	displayGame3D();
	glPopMatrix();

	glPopMatrix();

	//Mostra o mapa na tela
	if (mapActive) {
		glPushMatrix();
		displayMap();
		glPopMatrix();
	}

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
	static int lastMouseX = 0;
	if (jogador != NULL && gameStart && !gameOver)
		jogador->virarCanhao( 2 * ((x < lastMouseX) - (lastMouseX < x)) );
	lastMouseX = x;
}

void mousePress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//cria o tiro
		if (jogador != NULL && gameStart && !gameOver)
			tiros.push_back(jogador->atirar());
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
    //glEnable( GL_TEXTURE_2D );
    glEnable(GL_LIGHTING);
    glShadeModel (GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHT0);

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
