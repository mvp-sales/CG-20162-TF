#include "figuras.h"

//Plano

void DisplayPlane (GLuint texture)
{
    GLfloat materialEmission[] = { 1.0, 1.0, 1.0, 1};
    GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
    GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_shininess[] = { 100.0 };
    glColor3f(0,1,1);

    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT  );//X
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );//Y

    //glBindTexture (GL_TEXTURE_2D, texture);
    double textureS = 1; // Bigger than 1, repeat
    glBegin (GL_QUADS);
        //glNormal3f(0,0,1);
        //glTexCoord2f (0, 0);
        glVertex3f (-1, -1, 0);
        //glNormal3f(0,0,1);
        //glTexCoord2f (0, textureS);
        glVertex3f (-1, 1, 0);
        //glNormal3f(0,0,1);
        //glTexCoord2f (textureS, textureS);
        glVertex3f (1, 1, 0);
        //glNormal3f(0,0,1);
        //glTexCoord2f (textureS, 0);
        glVertex3f (1, -1, 0);
    glEnd();

}

//Esfera
OBJ * CreateSphere (double R, double space) 
{
    OBJ *obj = new OBJ;
    
    obj->numVtx = (180 / space) * 
                  (2 + 360 / (2*space)) * 4;
    obj->vtx = new VERTICES[ obj->numVtx ];
    obj->radius = R;

    int n;
    double vR, lVR;
    double hR, lHR;
    double norm;
    n = 0;
    for( vR = 0; vR <= 180-space; vR+=space){
        for(hR = 0; hR <= 360+2*space; hR+=2*space)
        {
            lVR = vR;
            lHR = hR;
            obj->vtx[n].X = R * 
                    sin(lHR / 180 * M_PI) * 
                    sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R * 
                    cos(lHR / 180 * M_PI) * 
                    sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R * 
                    cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(
                    obj->vtx[n].X*obj->vtx[n].X+
                    obj->vtx[n].Y*obj->vtx[n].Y+
                    obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;

            lVR = vR + space;
            lHR = hR;
            obj->vtx[n].X = R * sin(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R * cos(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R * cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(obj->vtx[n].X*obj->vtx[n].X+obj->vtx[n].Y*obj->vtx[n].Y+obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;

            lVR = vR;
            lHR = hR + space;
            obj->vtx[n].X = R * sin(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R * cos(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R * cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(obj->vtx[n].X*obj->vtx[n].X+obj->vtx[n].Y*obj->vtx[n].Y+obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;

            lVR = vR + space;
            lHR = hR + space;
            obj->vtx[n].X = R * sin(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R * cos(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R * cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(obj->vtx[n].X*obj->vtx[n].X+obj->vtx[n].Y*obj->vtx[n].Y+obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;
        }
    }
    return obj;
}

void DrawSphere (OBJ* objEarth, GLuint texture)
{
    GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
    GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
    GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_shininess[] = { 100.0 };
    glColor3f(1,1,1);
 
    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    //glBindTexture (GL_TEXTURE_2D, texture);
    glBegin (GL_TRIANGLE_STRIP);
    for ( int i = 0; i <objEarth->numVtx; i++)
    {
        glNormal3f(objEarth->vtx[i].nX, objEarth->vtx[i].nY, objEarth->vtx[i].nZ);
        //glTexCoord2f (objEarth->vtx[i].U, objEarth->vtx[i].V);
        glVertex3f (objEarth->vtx[i].X, objEarth->vtx[i].Y, objEarth->vtx[i].Z);
    }
    glEnd();
}

//Cilindro (incluir normais)
void DrawCylinder(double radius, double length) {
    int slices = 72;
    double fraction = 360 / slices;
    for(int i=0; i<slices; i++) {
        float theta = ((float)i) * fraction * DEG2RAD;
        float nextTheta = ((float)i+1) * fraction * DEG2RAD;
        glBegin(GL_TRIANGLE_STRIP);
        /*vertex at middle of end */ glVertex3f(0.0, length, 0.0);
        /*vertices at edges of circle*/ glVertex3f(radius*cos(theta), length, radius*sin(theta));
                                                  glVertex3f (radius*cos(nextTheta), length, radius*sin(nextTheta));
        /* the same vertices at the bottom of the cylinder*/
        glVertex3f (radius*cos(nextTheta), 0.0, radius*sin(nextTheta));
        glVertex3f(radius*cos(theta), 0.0, radius*sin(theta));
        glVertex3f(0.0, 0.0, 0.0);
        glEnd();
    }
}

//Distância entre pontos
double distPonto(Ponto p1, Ponto p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

int outOfBounds(Ponto p, double bBoxLeft, double bBoxRight, double bBoxBottom, double bBoxTop) {
	return p.x > bBoxRight || p.x < bBoxLeft || p.y > bBoxTop || p.y < bBoxBottom;
}


bool dentroCirc(Circulo externo, Circulo interno) {
	return (distPonto(externo.centro, interno.centro) <= abs(externo.raio - interno.raio));
}


bool colisaoCirc(Circulo c1, Circulo c2) {
	return (distPonto(c1.centro, c2.centro) < (c1.raio + c2.raio));
}

void desenhaRetangulo(double largura, double altura, double r, double g, double b) {
	glColor3f(r,g,b);
	glBegin(GL_QUADS);
	glVertex2f(-largura/2.0, 0);
	glVertex2f(-largura/2.0, altura);
	glVertex2f(largura/2.0, altura);
	glVertex2f(largura/2.0, 0);
	glEnd();
}

void desenhaCirculo(double raio, double r, double g, double b) {
	glColor3f(r,g,b);
	glBegin(GL_POLYGON);
	int i = 0;
	for (i = 0; i <= 360; i++) {
		glVertex2f(cos(i*DEG2RAD)*raio, sin(i*DEG2RAD)*raio);
	}
	glEnd();
}

void desenhaCirculoLinha(double raio, double r, double g, double b) {
    glColor3f(r,g,b);
    glBegin(GL_LINES);
    int i = 0;
    for (i = 0; i <= 360; i++) {
        glVertex2f(cos(i*DEG2RAD)*raio, sin(i*DEG2RAD)*raio);
    }
    glEnd();
}