//// Created by wagner on 15/08/17.
//

#include "ClassFile.h"
#include "UtilsNumeros.h"
#include "Debug.h"
#include "cstdio"



ClassFile::ClassFile(char* nome) {
    this->arquivo.open("HelloWorld.class",std::fstream::in | std::fstream::binary);
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

    Debug("========>FIM DA CONSTANT POOL<=========\n");

    readAcessFlag();
    Debug("Acess Flag = 0x" << std::hex << this->access_flags << std::endl);

    readThisClass();
    Debug("This Class: =" << this->this_class << std::endl);

    readSuperClass();
    Debug("Super Class: =" << this->super_class << std::endl);

    readInterfaceCount();
    Debug("Interface Count: =" << this->interface_count << std::endl);

    Debug("entrou no metodo de ler as interfaces");
    readInterfaces();
    



    /*todo terminar os possiveis tipo de constant pool*/




    return ;
}

void ClassFile::readInterfaces(){

    this->interface_table = (u2*)malloc(sizeof(u2)*this->interface_count);
    for (u2 i = 0 ; i < this->interface_count; i++)
    {
        this->interface_table[i] = readU16();
    }

}


void ClassFile::readInterfaceCount() {
    this->interface_count = readU16();
}

void ClassFile::readSuperClass(){
    this->super_class = readU16();

}

void ClassFile::readThisClass() {
    this->this_class = readU16();

}
void ClassFile::readAcessFlag() {
    this->access_flags = readU16();

}

void ClassFile::readConstantPool() {

    u2 tamPool = this->cp_size - 1;
    this->constant_pool= (cp_info*) malloc(sizeof(cp_info) * tamPool);

    printf("\n=====> INICIO READ CONSTANT POOL <<=======\n\n");
    for(u2 i = 0 ; i < tamPool ;i++)
    {
        uint8_t tag = readU8();
        //Debug("Tag lida["<< i <<"] =" <<std::dec << tag << "\n");
        //std::cout << tag;
        this->constant_pool[i].tag = tag;

        Debug(<<"valor lido na tag["<<i<<"] = "<< std::hex  << std::uppercase << static_cast<int>(tag)<< std::dec << "\n");
        switch(tag)
        {
            case CONSTANT_Class:
                Debug("entrou no case [CONSTANT CLASS]\n");
                constant_pool[i].info.class_info = getConstantClassInfo();
                break;
            case CONSTANT_Fieldref:
                Debug("entrou no case [CONSTANT_Fieldref] \n");
                constant_pool[i].info.fieldref_info = getConstantFieldRefInfo();
                break;
            case CONSTANT_Methodref:
                Debug("entrou no case [MethodRef]\n");
                constant_pool[i].info.methodref_info = getConstantMethodRefInfo();
                break;
            case CONSTANT_String:
                Debug("ENTROU NO CASE [STRING INFO]\n");
                constant_pool[i].info.string_info = getConstantStringInfo();
                break;
            case CONSTANT_Utf8:
                Debug("ENTROU NO CASE [UTF8]\n");
                constant_pool[i].info.utf8_info =  getConstantUtf8Info();
                break;
            case CONSTANT_NameAndType:
                Debug("ENTROU NO CONSTANT_NAMEANDTYPE\n");
                constant_pool[i].info.nameAndType_info = getConstantNameAndType_info();
                break;
            case CONSTANT_InterfaceMethodref:
                Debug("Entrou no constant interface methodo info\n");
                constant_pool[i].info.interfaceMethodref_info = getConstantInterfaceMethodRefInfo();
                break;
            case CONSTANT_Integer:
                Debug("Entrou no constant integer info\n");
                constant_pool[i].info.integer_info = getConstantIntegerInfo();
                break;
            case CONSTANT_Float:
                Debug("Entrou no constant float info\n");
                constant_pool[i].info.float_info = getConstantFloatInfo();
                break;
            case CONSTANT_Long:
                Debug("Entrou no caso constant long\n");
                constant_pool[i].info.long_info = getConstantLongInfo();
                break;
            case CONSTANT_Double:
                Debug("Entrou no caso constant double\n");
                constant_pool[i].info.double_info = getConstantDoubleInfo();
                break;
            default:
                Debug("Foi encontrada uma tag invalida no arquivo [" << this->nome << "]\n");
                exit(5);
        }
        printf("\n");
    }
    return;
}


CONSTANT_Double_info ClassFile::getConstantDoubleInfo() {
    CONSTANT_Double_info result;
    result.high_bytes = readU32();
    result.low_bytes = readU32();

    return result;
}


CONSTANT_Long_info ClassFile::getConstantLongInfo() {
    //não testado.
    CONSTANT_Long_info result;
    result.high_bytes = readU32();
    result.low_bytes = readU32();

    return result;
}



CONSTANT_Float_info ClassFile::getConstantFloatInfo() {
    //não testado
    CONSTANT_Float_info result;
    result.bytes = readU32();


    return result;
}


CONSTANT_Integer_info ClassFile::getConstantIntegerInfo() {
    //não testado.
    CONSTANT_Integer_info result;
    result.bytes = readU32();
    return result;
}




CONSTANT_InterfaceMethodref_info ClassFile::getConstantInterfaceMethodRefInfo(){
    // nao testado.
    CONSTANT_InterfaceMethodref_info result;
    result.class_index = readU16();
    result.name_and_type_index = readU16();

    return result;
}



CONSTANT_NameAndType_info ClassFile::getConstantNameAndType_info() {
    CONSTANT_NameAndType_info result;
    result.name_index = readU16();
    result.descriptor_index = readU16();

   return result;
}

CONSTANT_Utf8_info ClassFile::getConstantUtf8Info() {

    CONSTANT_Utf8_info result;
    u2 tam  = readU16();

    result.length = tam;
    result.bytes = (u1*) malloc(sizeof(u1) * result.length);
    for (u2 i = 0 ; i < result.length;i++)
    {
        result.bytes[i] = readU8();
    }

    return result;
}
CONSTANT_String_info ClassFile::getConstantStringInfo() {
    CONSTANT_String_info result;
    result.string_index = readU16();

    return result;
}

CONSTANT_Fieldref_info ClassFile::getConstantFieldRefInfo() {
    CONSTANT_Fieldref_info result;
    result.class_index = readU16();
    result.name_and_type_index = readU16();

    return result;
}


CONSTANT_Class_info ClassFile::getConstantClassInfo() {
    CONSTANT_Class_info result;
    result.name_index = readU16();

    return result;
}

CONSTANT_Methodref_info ClassFile::getConstantMethodRefInfo() {
    CONSTANT_Methodref_info result;

    result.class_index = readU16();
    result.name_and_type_index = readU16();

    return result;

}

uint8_t ClassFile::readU8() {
    uint8_t inteiro;
    if (!arquivo.is_open()) Debug("perdeu a referencia no readu8");
    arquivo.read((char*)&inteiro,1);
    printf("\tu8: hexadecimal lido = 0x%04x \n",(int)inteiro);
    return (uint8_t)inteiro;
}

uint32_t ClassFile::readU32 () {
    uint32_t inteiro;
    arquivo.read( (char*)&(inteiro), 4);
    inteiro = UtilsNumeros::swap_uint32(inteiro);
    printf("\tu32: hexadecimal lido = 0x%04x \n",(int)inteiro);
    return inteiro;
}
uint16_t ClassFile::readU16() {
    uint16_t inteiro;
    arquivo.read( (char*)&inteiro, 2);
    inteiro = UtilsNumeros::swap_uint16(inteiro);
    printf("\tu16: hexadecimal lido = 0x%04x \n",(int)inteiro);
    return inteiro;
}