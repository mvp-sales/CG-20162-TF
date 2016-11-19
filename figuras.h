#ifndef FIGURAS_H
#define FIGURAS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const double DEG2RAD = M_PI/180;

typedef struct
{
    //Vertex coordinate
    double X;
    double Y;
    double Z;
    
    //Vertex normal 
    double nX;
    double nY;
    double nZ;
    
    //Vertex texture coordinate
    double U;
    double V;
} VERTICES;

typedef struct
{
    VERTICES * vtx;
    int numVtx;
    double radius;
} OBJ;

typedef struct ponto {
	double x;
	double y;
} Ponto;

typedef struct cor {
	double r;
	double g;
	double b;
} Cor;

typedef struct circulo {
	Ponto centro;
	double raio;
	Cor fill;
} Circulo;

typedef struct grupoCirculo {
	Circulo atual;
	struct grupoCirculo* prox;
} GrupoCirculo;

struct retangulo {
	Ponto vEsqSup;
	double largura;
	double altura;
	Cor fill;
};

typedef struct retangulo Retangulo;

//Funções adicionais
double distPonto(Ponto p1, Ponto p2);
int outOfBounds(Ponto p, double bBoxLeft, double bBoxRight, double bBoxBottom, double bBoxTop);
bool dentroCirc(Circulo externo, Circulo interno);
bool colisaoCirc(Circulo c1, Circulo c2);

//Funções 2D
void desenhaRetangulo(double largura, double altura, double r, double g, double b);
void desenhaCirculo(double raio, double r, double g, double b);
void desenhaCirculoLinha(double raio, double r, double g, double b);

//Funções 3D
void DrawSphere (OBJ* objEarth, GLuint texture);
OBJ * CreateSphere (double R, double space) ;
void DisplayPlane (GLuint texture);
void DrawCylinder(double radius, double length);

#endif