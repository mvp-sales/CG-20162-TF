#include "Obj.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

bool Obj::loadFile(const char* path){
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< double* > temp_vertices;
	std::vector< double* > temp_uvs;
	std::vector< double* > temp_normals;

	FILE * file = fopen(path, "r");
	if( file == NULL ){
	    printf("Impossible to open the file !\n");
	    return false;
	}

	while( 1 ){

	    char lineHeader[128];
	    // read the first word of the line
	    int res = fscanf(file, "%s", lineHeader);
	    if (res == EOF)
	        break; // EOF = End Of File. Quit the loop.

	    // else : parse lineHeader
	    if ( strcmp( lineHeader, "v" ) == 0 ){
		   	double* vertex = new double[3];
		    fscanf(file, "%lf %lf %lf\n", &vertex[0], &vertex[1], &vertex[2] );
		    temp_vertices.push_back(vertex);
	    }else if ( strcmp( lineHeader, "vt" ) == 0 ){
		    double* uv = new double[2];
		    fscanf(file, "%lf %lf\n", &uv[0], &uv[1] );
		    temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
		    double* normal = new double[3];
		    fscanf(file, "%lf %lf %lf\n", &normal[0], &normal[1], &normal[2] );
		    temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
		    unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
		    int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
		    //int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );

		    if (matches != 9){
		        printf("File can't be read by our simple parser : ( Try exporting with other options\n");
		        return false;
		    }
		    vertexIndices.push_back(vertexIndex[0]);
		    vertexIndices.push_back(vertexIndex[1]);
		    vertexIndices.push_back(vertexIndex[2]);
		    uvIndices    .push_back(uvIndex[0]);
		    uvIndices    .push_back(uvIndex[1]);
		    uvIndices    .push_back(uvIndex[2]);
		    normalIndices.push_back(normalIndex[0]);
		    normalIndices.push_back(normalIndex[1]);
		    normalIndices.push_back(normalIndex[2]);
		}
	}

	for( unsigned int i=0; i<vertexIndices.size(); i++ ){
    	unsigned int vertexIndex = vertexIndices[i];
    	double* vertex = temp_vertices[ vertexIndex-1 ];
    	vertices.push_back(vertex);
    }
    for( unsigned int i=0; i<uvIndices.size(); i++ ){
    	unsigned int uvIndex = uvIndices[i];
    	double* uv = temp_uvs[ uvIndex-1 ];
    	texturas.push_back(uv);
    }
     for( unsigned int i=0; i<normalIndices.size(); i++ ){
    	unsigned int normalIndex = normalIndices[i];
    	double* normal = temp_normals[ normalIndex-1 ];
    	normais.push_back(normal);
    }

	return true;
}

void Obj::draw(){
	glBegin(GL_TRIANGLES);
		glColor3d(1.0,1.0,1.0);
		for(int i = 0; i < out_vertices.size(); i++){
			glNormal3dv(out_normals.at(i));
			glVertex3dv(out_vertices.at(i));
		}
	glEnd();
}

Obj::~Obj(){
	for(int i = 0; i < vertices.size(); i++){
		delete vertices.at(i);
	}

	for(int i = 0; i < texturas.size(); i++){
		delete texturas.at(i);
	}

	for(int i = 0; i < normais.size(); i++){
		delete normais.at(i);
	}
}