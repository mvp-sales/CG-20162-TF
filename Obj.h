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