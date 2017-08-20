//// Created by wagner on 15/08/17.
//

#include "ClassFile.h"
#include "UtilsNumeros.h"
#include "Debug.h"



ClassFile::ClassFile(char* nome) {
    this->arquivo.open("Test.class",std::fstream::in | std::fstream::binary);
    this->nome = std::string(nome);
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

    //le a constant pool count
    this->cp_size = readU16();
    Debug("cp size = [" <<  std::dec << this->cp_size << "]" << std::endl);

    readConstantPool();


    return ;
}

void ClassFile::readConstantPool() {

    u2 tamPool = this->cp_size - 1;
    this->constant_pool= (cp_info*) malloc(sizeof(cp_info) * tamPool);

    for(u2 i = 0 ; i < tamPool ;i++)
    {
        uint8_t tag = readU8();
        //Debug("Tag lida["<< i <<"] =" <<std::dec << tag << "\n");
        std::cout << tag;
        this->constant_pool[i].tag = tag;

        tag = this->constant_pool[i].tag;

        std::cout << tag;


        switch(tag)
        {
            case CONSTANT_Class:
                constant_pool[i].info.class_info = getConstantClassInfo();
            break;
            case CONSTANT_Fieldref:
                break;

            default:
                Debug("Foi encontrada uma tag invalida no arquivo [" << this->nome << "]\n");
                exit(5);
        }



    }
    return;
}
CONSTANT_Class_info ClassFile::getConstantClassInfo() {
    CONSTANT_Class_info result;
    result.name_index = readU16();

    return result;
}

uint8_t ClassFile::readU8() {
    uint8_t inteiro;
    if (!arquivo.is_open()) Debug("perdeu a referencia no readu8");
    arquivo.read((char*)&inteiro,1);

    return (uint8_t)inteiro;
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