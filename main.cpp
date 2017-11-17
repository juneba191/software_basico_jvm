
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ClassFile.h"
#include "ScreenPrinter.h"
#include <string.h>
#include "Debug.h"


using namespace std;



int main() {


    // Lê o arquivo.
    ClassFile classFile("Hello.class");
    classFile.leClasse();
    Screen_Print_Javap_Format(classFile);

    return 0;
}