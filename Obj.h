#ifndef OBJ_H
#define OBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using std::vector;

class Obj {
	private:
		vector<double*> vertices;
		vector<double*> texturas;
		vector<double*> normais;

	public:
		bool loadFile(const char* path);
		void draw();
		~Obj();
};

#endif
