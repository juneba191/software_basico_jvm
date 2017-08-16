//
// Created by wagner on 15/08/17.
//

#include "ClassFile.h"

ClassFile::ClassFile(char* nome) {
    this->arquivo.open("Test.class",std::fstream::in | std::fstream::binary);

    if (!this->arquivo.is_open())
    {
        std::cout << "ARQUIVO '" << nome << "' NÃO FOI ABERTO." << std::endl;
    }
    else{
        std::cout << "ABRIU O ARQUIVO '" << nome <<"'" << std::endl;
    }

}
void ClassFile::leClasse() {
    if (!this->arquivo.is_open())
    {
        std::cout << "PERDEU A REFERENCIA NO MÉTODO 'LÊCLASSE'" << std::endl;
        return;
    }

    std::cout << "CONTINUA COM  A REFERENCIA NO MÉTODO 'LÊCLASSE'" << std::endl;


    return;
}