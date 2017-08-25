
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ClassFile.h"
#include <string.h>
#include "Debug.h"


using namespace std;



int main() {


    // Lê o arquivo.
    ClassFile classFile("HelloWorld.class");

    classFile.leClasse();



    return 0;
}