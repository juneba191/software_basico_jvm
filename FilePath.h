
#include<string>

using namespace std;

class FilePath {
	private:
		static FilePath* instance;
		FilePath();
	public:
   		string path;
		static FilePath* GetInstance();
};
