
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ClassFile.h"
#include "ScreenPrinter.h"
#include <string.h>
#include "Debug.h"
#include "Frame.h"
#include "Interpreter.h"

using namespace std;

int main(int argc, char *argv[])
{

    // Lê o arquivo.
    string nome = argv[1];
    ClassFile *classFile = new ClassFile(nome);
    classFile->leClasse();
    Interpreter::GetInstance()->AddClass(classFile);
    Frame::SetUp();
    Interpreter::GetInstance()->Run(classFile);
    return 0;
}
