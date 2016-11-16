#include "leitor.h"

using namespace std;

Retangulo* linha = (Retangulo*) malloc(sizeof(Retangulo));
Circulo* pistaExterna = (Circulo*) malloc(sizeof(Circulo));
Circulo* pistaInterna = (Circulo*) malloc(sizeof(Circulo));
list<CarroInimigo*> inimigos;
CarroJogador* jogador = new CarroJogador();
list<Tiro*> tiros;

OBJ* internal;

//Dimensões da janela
double janelaLarg;
double janelaAlt;
double nearZ = 0.999;
double farZ = 100;

//Atributos dos inimigos
EnemyAttr* enemyAttributes;

//Flags de controle do jogo
bool gameStart = false;
bool gameWon = false;
bool gameOver = false;
int currentCheckpoint = 0;

//Controla o movimento no teclado
int keyState[256];

// Text variable
static char str[2000];
void * font = GLUT_BITMAP_9_BY_15;

//Câmera
int toggleCam = 0;

//Timer
double timerSeg = 0;
double timerMin = 0;
double timerHour = 0;
GLdouble previousTime = 0;
GLdouble startTime = 0;

void makeIdentityf(GLfloat m[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = i == j;
        }
    }
}

void normalize(float o[3]) {
    double norm = sqrt(o[0]*o[0]+o[1]*o[1]+o[2]*o[2]);
    o[0] /= norm;
    o[1] /= norm;
    o[2] /= norm;   
}

void crossProduct(
        float u[3], 
        float v[3], 
        float o[3])
{
    o[0] = u[1]*v[2] - u[2]*v[1];
    o[1] = u[2]*v[0] - u[0]*v[2];
    o[2] = u[0]*v[1] - u[1]*v[0];
}

void lookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx,
          GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy,
          GLdouble upz)
{
    float forward[3], side[3], up[3];
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



void printTimer(GLfloat x, GLfloat y)
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
		    sprintf(str, "Tempo: %02d:%02d:%02d", (int) timerHour, (int) timerMin, (int) timerSeg);
		    //Define the position to start printing
		    glRasterPos2f(x, y);
		    //Print  the first Char with a certain font
		    tmpStr = str;
		    //Print each of the other Char at time
		    while( *tmpStr ){
		        glutBitmapCharacter(font, *tmpStr);
		        tmpStr++;
		    }
    	glPopAttrib();
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
}

void printEnd(GLfloat x, GLfloat y)
{
    //Create a string to be printed
    char *tmpStr;
    if (gameWon)
    	sprintf(str, "Voce ganhou.");
    else
    	sprintf(str, "Voce perdeu.");
    //Define the position to start printing
    glRasterPos2f(x, y);
    //Print  the first Char with a certain font
    tmpStr = str;
    //Print each of the other Char at time
    while( *tmpStr ){
        glutBitmapCharacter(font, *tmpStr);
        tmpStr++;
    }

}

void displayGame2D() {

	//Desenha a pista externa
	glPushMatrix();
	glTranslatef(pistaExterna->centro.x, pistaExterna->centro.y, 0);
	desenhaCirculo(pistaExterna->raio, pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna->centro.x, pistaInterna->centro.y, 0);
	glColor3f(pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
	desenhaCirculo(pistaInterna->raio, pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
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
		jogador->desenhar();
		glPopMatrix();
	}

	//Desenha os círculos dos adversários
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* t = *it;
		glPushMatrix();
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
		t->desenhar();
		glPopMatrix();		
	}
}

void displayGame3D() {

	//Desenha a pista externa
	glPushMatrix();
	glTranslatef(pistaExterna->centro.x, pistaExterna->centro.y, 0);
	desenhaCirculo(pistaExterna->raio, pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna->centro.x, pistaInterna->centro.y, 0);
	glColor3f(pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
	DrawSphere(internal, 1);
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
		jogador->desenhar();
		glPopMatrix();
	}

	//Desenha os círculos dos adversários
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* t = *it;
		glPushMatrix();
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
		t->desenhar();
		glPopMatrix();		
	}
}

void displayMap() {
    glMatrixMode (GL_PROJECTION);
    //Push to recover original PROJECTION MATRIX
    glPushMatrix();
        glLoadIdentity();
        glOrtho (0, pistaExterna->raio * 2, 0, pistaExterna->raio * 2, -1, 1);
    	glPushAttrib(GL_ENABLE_BIT);
		    glDisable(GL_LIGHTING);
		    glDisable(GL_TEXTURE_2D);
		    glMatrixMode(GL_MODELVIEW);
		    glPushMatrix();
		    	glTranslatef(janelaLarg - 100, 100 , 1);
		    	glScalef(200/janelaLarg, 200/janelaAlt, 1);
		    	displayGame2D();
		    glPopMatrix();
			glMatrixMode(GL_PROJECTION);
		glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//Funcao que sera chamada toda vez que a janela for repintada.
void display() {
	static double angle = 0;
	static int sign = 1;
	angle += sign* 0.5;
	if (angle > 90) { angle = 90; sign = -1; }
	else if (angle < -90) { angle = -90; sign = 1; }
	//Limpa a tela com a cor especificada
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Ativa a visão do modelo
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Mostra o mapa na tela
	glPushMatrix();
	if (toggleCam == 0) {
		//Câmera interna ao carro
		double px = jogador->getPosicao().x;
		double py = jogador->getPosicao().y;
		double pz = 1.0;//jogador->getAltura();

		double cx = px - 0.5 * sin(jogador->getAngCarro());
		double cy = py + 0.5 * cos(jogador->getAngCarro());
		lookAt(px, py, pz,
			   cx, cy, 0,
			   0, 0, 1);

	}
	else if (toggleCam == 1) {

	}
	else if (toggleCam == 2) {

	}

	glPushMatrix();
	//glTranslatef(0,0,-1.0);
	displayGame3D();
	glPopMatrix();

	//Desenha o plano
	glPushMatrix();
	glTranslatef(0,0,-1.0);
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, -pistaExterna->raio);
	glScalef(pistaExterna->raio,pistaExterna->raio,1);
	DisplayPlane(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,0,-1.0);
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, pistaExterna->raio);
	glScalef(pistaExterna->raio,pistaExterna->raio,1);
	DisplayPlane(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,0,-1.0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, pistaExterna->raio);
	glScalef(pistaExterna->raio,pistaExterna->raio,1);
	DisplayPlane(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,0,-1.0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, -pistaExterna->raio);
	glScalef(pistaExterna->raio,pistaExterna->raio,1);
	DisplayPlane(1);
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();
	displayMap();
	glPopMatrix();

	//Desenha o tempo na tela
	glColor3f(0.0, 0.0, 0.0);
	printTimer((janelaLarg - 150.0) / janelaLarg, (janelaAlt - 15.0) / janelaAlt);

	if (gameOver)
		printEnd(janelaLarg / 2 - 45.0, janelaAlt / 2);

    //Desenha na tela
    glutSwapBuffers();
}

void keyRelease(unsigned char key, int x, int y) {
	keyState[key] = 0;
}

void keyPress(unsigned char key, int x, int y) {
	keyState[key] = 1;

	switch(key) {
		case '0':
			toggleCam = 0;
			break;
		case '1':
			toggleCam = 1;
			break;
		case '2':
			toggleCam = 2;
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
			jogador->virarRoda(1.5);
		if (keyState['D'] || keyState['d'])
			jogador->virarRoda(-1.5);
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

void changeCamera(int w, int h) {
    glMatrixMode (GL_PROJECTION);

    glLoadIdentity ();

    //gluPerspective (angle, 
    //       (GLfloat) w / (GLfloat) h, 1.0, 5.0);
    glFrustum(-w/2, w/2, -h/2, h/2, nearZ, farZ);

    glMatrixMode (GL_MODELVIEW);
}

void reshape (int w, int h) {

    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    janelaLarg = w;
    janelaAlt = h;
    changeCamera(w, h);
}

void init() {
	glClearColor(1.0, 1.0, 1.0, 0);

	//Modelo 3D
	glEnable(GL_DEPTH_TEST);
    //glEnable( GL_TEXTURE_2D );
    //glEnable(GL_LIGHTING);
    glShadeModel (GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);

    internal = CreateSphere(pistaInterna->raio, 16);

}

int main(int argc, char** argv) {
	readXML(argv[1], "config.xml", enemyAttributes, jogador, inimigos, pistaInterna, pistaExterna, linha);
	janelaLarg = pistaExterna->raio * 2;
	janelaAlt = pistaExterna->raio * 2;
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
