#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ClassFile.h"
#include <string.h>

using namespace std;




int main() {

    // Lê o arquivo.
    ClassFile classFile("Test.class");

    classFile.leClasse();



    return 0;
}