#include "FilePath.h"

FilePath* FilePath::instance = NULL;

FilePath::FilePath(){
}

FilePath* FilePath::GetInstance(){
	if (instance == NULL)
		instance = new FilePath();
	return instance;
}


