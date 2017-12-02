
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



int main() {


    // Lê o arquivo.
    ClassFile *classFile = new ClassFile("tFiles//TesteSwitch.class");
    classFile->leClasse();
    Interpreter::GetInstance()->AddClass(classFile);
    Frame::SetUp();
    Interpreter::GetInstance()->Run(classFile);
    return 0;
}