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

struct ponto {
	double x;
	double y;
};

typedef struct ponto Ponto;

struct cor {
	double r;
	double g;
	double b;
};

typedef struct cor Cor;

struct circulo {
	Ponto centro;
	double raio;
	Cor fill;
};

typedef struct circulo Circulo;

struct grupoCirculo {
	Circulo atual;
	struct grupoCirculo* prox;
};

typedef struct grupoCirculo GrupoCirculo;

struct retangulo {
	Ponto vEsqSup;
	double largura;
	double altura;
	Cor fill;
};

typedef struct retangulo Retangulo;

double distPonto(Ponto p1, Ponto p2);
bool dentroCirc(Circulo externo, Circulo interno);
bool colisaoCirc(Circulo c1, Circulo c2);
void desenhaRetangulo(double largura, double altura, double r, double g, double b);
void desenhaCirculo(double raio, double r, double g, double b);
int outOfBounds(Ponto p, double bBoxLeft, double bBoxRight, double bBoxBottom, double bBoxTop);
void DrawSphere (OBJ* objEarth, GLuint texture);
OBJ * CreateSphere (double R, double space) ;
void DisplayPlane (GLuint texture);


#endif