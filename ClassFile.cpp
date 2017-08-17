//// Created by wagner on 15/08/17.
//

#include "ClassFile.h"
#include "UtilsNumeros.h"
#include "Debug.h"



ClassFile::ClassFile(char* nome) {
    this->arquivo.open("Test.class",std::fstream::in | std::fstream::binary);

    if (!this->arquivo.is_open())
    {
       Debug("Arquivo [" << nome << "] não foi aberto.\n");
    }
    else{
        Debug("Foi aberto o arquivo [" << nome <<"] no método [ClassFile]\n" );
    }

}



void ClassFile::leClasse() {
    if (!this->arquivo.is_open())
    {
        Debug("Perdeu a referência  no método leClasse.\n");
        return;
    }

    Debug("Continua com a referência no método leClasse'\n");

    // lê o número mágico.
    this->magic_number = readU32();
    Debug("Número mágico = [" << std::hex <<  this->magic_number << "]" << std::endl);

    //lê a versão menor.
    this->minor_version = readU16();
    Debug("Minor version = [" << std::hex <<  this->minor_version << "]" << std::endl);

    //lê a versão maior
    this->major_version = readU16();
    Debug("Major version = [" << std::hex << this->major_version << "]" << std::endl);





    return ;
}



uint32_t ClassFile::readU32 () {
    uint32_t inteiro;
    arquivo.read( (char*)&(inteiro), 4);
    inteiro = UtilsNumeros::swap_uint32(inteiro);
    return inteiro;
}
uint16_t ClassFile::readU16() {
    uint16_t inteiro;
    arquivo.read( (char*)&inteiro, 2);
    inteiro = UtilsNumeros::swap_uint16(inteiro);
    return inteiro;
}