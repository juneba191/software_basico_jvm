#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ClassFile.h"
#include "ScreenPrinter.h"
#include <string.h>
#include "Debug.h"
#include "Frame.h"
#include "Interpreter.h"
#include "FilePath.h"

using namespace std;

int main(int argc, char *argv[])
{

    // Lê o arquivo.
    string nome = argv[1];
    
    size_t found = nome.find_last_of("/\\");
    FilePath::GetInstance()->path = nome.substr(0, found+1);
    
    nome = nome.substr(found+1);
    
    ClassFile *classFile = new ClassFile(nome);    
    
    
    
    
    
    Interpreter::GetInstance()->AddClass(classFile);
    Debug("dd");
    Frame::SetUp();
    Interpreter::GetInstance()->Run(classFile);
    return 0;
}
