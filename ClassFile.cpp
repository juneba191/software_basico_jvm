#include "ClassFile.h"
#include "UtilsNumeros.h"
#include <cstring>
#include "cstdio"
#include <string>

ClassFile::ClassFile(std::string nome) {
    std::cout<<"lendo arquivo\n";
    this->nome = std::string(nome);
}

u1 readu1FromFile(u1* ptr,int count, std::ifstream &stream){
	stream.read((char*)ptr,count);
	return *ptr;
}

#define readFromFile(type, size)\
	type read##type##FromFile(type* ptr, int count, std::ifstream &stream)\
	{\
        type aux;\
        for(int i =0; i<count; i++){\
            stream.read((char*)&(aux), size);\
            aux = UtilsNumeros::swap_##type(aux);\
            *(ptr+i) = aux;\
        }\
	return aux;\
    }
readFromFile(u2, 2); /*Esses pontos e virgulas são inuteis :)*/
readFromFile(u4, 4);
readFromFile(u8, 8);

void ClassFile::leClasse() {
    this->arquivo.open(nome, std::ifstream::in | std::ifstream::binary);
    if (!this->arquivo.is_open())
    {
        
        std::cout<<" file not opened"<<std::endl;
        exit(1);
    }

    readu4FromFile(&this->magic_number, 1, arquivo);
    if (this->magic_number != 0xCAFEBABE){
        std::cout<<"Magic number not 0xCAFEBABE"<<std::endl;
        exit(1);
    }

    readu2FromFile(&this->minor_version, 1, arquivo);
    readu2FromFile(&this->major_version, 1, arquivo);
    readu2FromFile(&this->cp_size, 1, arquivo);
    readConstantPool();
    readAcessFlag();
    readThisClass();
    readSuperClass();
    readInterfaceCount();
    readInterfaces();
    readFieldsCount();
    readFields();//não testei
    readMethodCount();
    readMethodInfo();
    readAttributesCount();
    readAttributes();
    this->arquivo.close();
    return ;
}
void ClassFile::readAttributes() {
    this->attributes = (attribute_info*) malloc(sizeof(attribute_info)* this->attributes_count);
    for (u2 i = 0 ; i < this->attributes_count ; i++)
    {
        this->attributes[i] = carregarAtributos();
    }

}

void ClassFile::readAttributesCount() {
    readu2FromFile(&this->attributes_count, 1, arquivo);
}

void ClassFile::readMethodCount() {
     readu2FromFile(&this->methods_count, 1, arquivo);
}

void ClassFile::readMethodInfo() {
    this->methods = (MethodInfo*) malloc(sizeof(MethodInfo)* this->methods_count);
    for (u2 i =0; i < this->methods_count; i++)
    {
        readu2FromFile(&this->methods[i].access_flags, 1, arquivo);
        readu2FromFile(&this->methods[i].name_index, 1, arquivo);
        readu2FromFile(&this->methods[i].descriptor_index, 1, arquivo);
        readu2FromFile(&this->methods[i].attributes_count, 1, arquivo);
        
        this->methods[i].attributes = (attribute_info*)malloc(sizeof(attribute_info)*this->methods[i].attributes_count);
        for (u2 j = 0 ; j < this->methods[i].attributes_count;j++){
            this->methods[i].attributes[j] = carregarAtributos();
        }
    }
}



void ClassFile::readFields() {
    this->fields = (field_info *)malloc(sizeof(field_info) * this->field_count);

    for (u2 i = 0 ; i < this->field_count ; i++)
    {
        /*o que acontece se eu fizer readu2FromFile(&this->fields, 4, arquivo)?*/
        readu2FromFile(&this->fields[i].access_flags, 1, arquivo);
        readu2FromFile(&this->fields[i].name_index, 1, arquivo);
        readu2FromFile(&this->fields[i].descriptor_index, 1, arquivo);
        readu2FromFile(&this->fields[i].attributes_count, 1, arquivo);

        this->fields[i].attributes = (attribute_info*)malloc(sizeof(attribute_info)*this->fields[i].attributes_count);
        for (u2 j = 0 ; j < this->fields[i].attributes_count; j++)
        {
            this->fields[i].attributes[j] = carregarAtributos(); //todo aqui
        }
    }
}
// vai na tabela de constant pool que ja foi carregada, e carrega
// os dados.

CONSTANT_Utf8_info ClassFile::pegaUtf8ConstantPool(u2 index) {
    // não testado.
    cp_info teste = this->constant_pool[index-1];
    return teste.info.utf8_info;
}

attribute_info ClassFile::carregarAtributos() {

    attribute_info result;
    readu2FromFile(&result.attribute_name_index, 1, arquivo);
    readu4FromFile(&result.attribute_length, 1, arquivo);
    if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"ConstantValue")){
        result.info.constantValue_Info = loadConstantValueAttribute();
    } else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"Code")) {
        result.info.code_info = loadCodeAttribute();
    } else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"Exceptions"))
    {
        result.info.exceptions_info = loadExceptionAttribute();
    }else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"InnerClasses") )
    {
        result.info.innerClasses_attribute_info = loadInnerClassAttribute();
    }else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"Synthetic")){
        result.info.syntethic_attribute =  loadSyntethicAttribute();
    }else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"SourceFile")) {
        result.info.sourceFile_attribute = loadSourceFileAttribute();
    }else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"LineNumberTable")){
        result.info.lineNumberTable_info = loadNumberTableAttribute();
    }else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"LocalVariableTable"))
    {
        result.info.localVariableTable_info = loadLocalVariableTableInfo();
    }else if (!strcmp((char*)this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes,"Deprecated") ){
        result.info.Deprecated_attribute_info = loadDeprecatedAttributeInfo();
    }else{
        
        std::cout<<this->constant_pool[result.attribute_name_index-1].info.utf8_info.bytes<<"Unkown Attribute\n";
    }
    return result;
}

Deprecated_attribute ClassFile::loadDeprecatedAttributeInfo() {
    Deprecated_attribute info;

    return info;
}


LocalVariableTable_attribute ClassFile::loadLocalVariableTableInfo() {
    LocalVariableTable_attribute info;
    readu2FromFile(&info.local_variable_table_length, 1, arquivo);
    info.local_table = (local_variable_table*) malloc(sizeof(local_variable_table)*info.local_variable_table_length);

    for (u2 i = 0 ; i < info.local_variable_table_length ; i++)
    {
        readu2FromFile(&info.local_table[i].start_pc, 1, arquivo);
        readu2FromFile(&info.local_table[i].length, 1, arquivo);
        readu2FromFile(&info.local_table[i].name_index, 1, arquivo);
        readu2FromFile(&info.local_table[i].descriptor_index, 1, arquivo);
        readu2FromFile(&info.local_table[i].index, 1, arquivo);
    }

    return info;
}


LineNumberTable_attributes ClassFile::loadNumberTableAttribute() {
    LineNumberTable_attributes  info;
    readu2FromFile(&info.line_number_table_length, 1, arquivo);
    info.lineTable = (lineNumberTable*)malloc(sizeof(lineNumberTable)*info.line_number_table_length);

    for (u2 i = 0 ; i < info.line_number_table_length; i++)
    {
        readu2FromFile(&info.lineTable[i].start_pc, 1, arquivo);
        readu2FromFile(&info.lineTable[i].line_number, 1, arquivo);
    }
    return info;
}



SourceFile_attribute ClassFile::loadSourceFileAttribute() {
    //testar

    SourceFile_attribute info;
    readu2FromFile(&info.sourcefile_index, 1, arquivo);

    return info;
}


Syntethic_attribute ClassFile::loadSyntethicAttribute() {
    Syntethic_attribute info;
    return info;
}


InnerClasses_attribute ClassFile::loadInnerClassAttribute() {
    InnerClasses_attribute info;

    readu2FromFile(&info.number_of_classes, 1, arquivo);
    info.classes = (classes_info*) malloc(sizeof(classes_info) * info.number_of_classes);
    for (u2 i = 0 ; i < info.number_of_classes; i++)
    {
        readu2FromFile(&info.classes[i].inner_class_info_index, 1, arquivo);
        readu2FromFile(&info.classes[i].outer_class_info_index, 1, arquivo);
        readu2FromFile(&info.classes[i].inner_name_index, 1, arquivo);
        readu2FromFile(&info.classes[i].inner_class_access_flags, 1, arquivo);
    }
    return info;
}

Exceptions_attribute ClassFile::loadExceptionAttribute() {
    Exceptions_attribute info;
    readu2FromFile(&info.number_of_exceptions, 1, arquivo);
    readu2FromFile(info.exceptions_index_table, info.number_of_exceptions, arquivo);
    return info;
}


Code_attributes ClassFile::loadCodeAttribute() {
    Code_attributes result;
    /*Nao tem mais name_index nem attr length*/
    // readu2FromFile(&result.attribute_name_index, 1, arquivo);
    // readu4FromFile(&result.attribute_length, 1, arquivo);
    readu2FromFile(&result.max_stack, 1, arquivo);
    readu2FromFile(&result.max_locals, 1, arquivo);
    readu4FromFile(&result.code_length, 1, arquivo);
    result.code = (u1*) malloc(sizeof(u1)*result.code_length);
    
    readu1FromFile(result.code, result.code_length, arquivo);
    
    readu2FromFile(&result.exception_table_length, 1, arquivo);
    
    result.exception_table = (Exception_table_info*) malloc(sizeof(Exception_table_info) * result.exception_table_length);
    
    for(u2 i = 0 ; i < result.exception_table_length;i++){
        readu2FromFile(&result.exception_table[i].start_pc, 1, arquivo);
        readu2FromFile(&result.exception_table[i].end_pc, 1, arquivo);
        readu2FromFile(&result.exception_table[i].handler_pc, 1, arquivo);
        readu2FromFile(&result.exception_table[i].catch_type, 1, arquivo);
    }

    readu2FromFile(&result.attributes_count, 1, arquivo);
    result.attributes = (attribute_info*)malloc(sizeof(attribute_info) * result.attributes_count);
    for (u2 i= 0 ; i < result.attributes_count; i++)
    {
        result.attributes[i] = carregarAtributos();
    }
    return result;
}



ConstantValue_attribute ClassFile::loadConstantValueAttribute() {
    ConstantValue_attribute result;
    readu2FromFile(&result.attribute_name_index, 1, arquivo);
    readu4FromFile(&result.attribute_length, 1, arquivo);
    readu2FromFile(&result.constantvalue_index, 1, arquivo);
    return result;//faltava isso né pai.
}

void ClassFile::readFieldsCount() {

    readu2FromFile(&this->field_count, 1, arquivo);

}

void ClassFile::readInterfaces(){

    this->interface_table = (u2*)malloc(sizeof(u2)*this->interface_count);
    readu2FromFile(this->interface_table, this->interface_count, arquivo);
}


void ClassFile::readInterfaceCount() {
    readu2FromFile(&this->interface_count, 1, arquivo);
}

void ClassFile::readSuperClass(){
    readu2FromFile(&this->super_class, 1, arquivo);
}

void ClassFile::readThisClass() {
    readu2FromFile(&this->this_class, 1, arquivo);
}
void ClassFile::readAcessFlag() {
    readu2FromFile(&this->access_flags, 1, arquivo);
}

void ClassFile::readConstantPool() {

    u2 tamPool = this->cp_size - 1;
    this->constant_pool = (cp_info*) malloc(sizeof(cp_info) * tamPool);

    for(u2 i = 0 ; i < tamPool ;i++)
    {
        readu1FromFile(&this->constant_pool[i].tag, 1, arquivo);
        //
        //std::cout << tag;
        switch(constant_pool[i].tag)
        {
            case CONSTANT_Class:
                
                constant_pool[i].info.class_info = getConstantClassInfo();
                break;
            case CONSTANT_Fieldref:
                
                constant_pool[i].info.fieldref_info = getConstantFieldRefInfo();
                break;
            case CONSTANT_Methodref:
                
                constant_pool[i].info.methodref_info = getConstantMethodRefInfo();
                break;
            case CONSTANT_String:
                
                constant_pool[i].info.string_info = getConstantStringInfo();
                break;
            case CONSTANT_Utf8:
                
                constant_pool[i].info.utf8_info =  getConstantUtf8Info();
                break;
            case CONSTANT_NameAndType:
                
                constant_pool[i].info.nameAndType_info = getConstantNameAndType_info();
                break;
            case CONSTANT_InterfaceMethodref:
                
                constant_pool[i].info.interfaceMethodref_info = getConstantInterfaceMethodRefInfo();
                break;
            case CONSTANT_Integer:
                
                constant_pool[i].info.integer_info = getConstantIntegerInfo();
                break;
            case CONSTANT_Float:
                
                constant_pool[i].info.float_info = getConstantFloatInfo();
                break;
            case CONSTANT_Long:
                
                constant_pool[i].info.long_info = getConstantLongInfo();
		i++;                
		break;
            case CONSTANT_Double:
                
                constant_pool[i].info.double_info = getConstantDoubleInfo();
		i++;                
		break;
            case CONSTANT_MethodHandle:
                constant_pool[i].info.methodHandle_info = getConstantMethodHandleInfo();
                break;
            case CONSTANT_MethodType:
                constant_pool[i].info.methodType_info = getConstantMethodTypeInfo();
                break;
            case CONSTANT_InvokeDynamic:
                constant_pool[i].info.invokeDynamic_info = getConstantInvokeDynamicInfo();
                break;
            default:
                
                exit(5);
        }
    }
    return;
}


CONSTANT_Double_info ClassFile::getConstantDoubleInfo() {
    CONSTANT_Double_info result;
    readu8FromFile(&result.value, 1, arquivo);
    result.high_bytes = result.value>>32;
    result.low_bytes = result.value<<32;
    return result;
}


CONSTANT_Long_info ClassFile::getConstantLongInfo() {
    //não testado.
    CONSTANT_Long_info result;
    readu8FromFile(&result.value, 1, arquivo);
    return result;
}



CONSTANT_Float_info ClassFile::getConstantFloatInfo() {
    //não testado
    CONSTANT_Float_info result;
    readu4FromFile(&result.bytes, 1, arquivo);


    return result;
}


CONSTANT_Integer_info ClassFile::getConstantIntegerInfo() {
    //não testado.
    CONSTANT_Integer_info result;
    readu4FromFile(&result.bytes, 1, arquivo);
    return result;
}




CONSTANT_InterfaceMethodref_info ClassFile::getConstantInterfaceMethodRefInfo(){
    // nao testado.
    CONSTANT_InterfaceMethodref_info result;
    readu2FromFile(&result.class_index, 1, arquivo);
    readu2FromFile(&result.name_and_type_index, 1, arquivo);

    return result;
}



CONSTANT_NameAndType_info ClassFile::getConstantNameAndType_info() {
    CONSTANT_NameAndType_info result;
    readu2FromFile(&result.name_index, 1, arquivo);
    readu2FromFile(&result.descriptor_index, 1, arquivo);

   return result;
}

CONSTANT_Utf8_info ClassFile::getConstantUtf8Info() {

    CONSTANT_Utf8_info result;
    readu2FromFile(&result.length, 1, arquivo);
    result.bytes = (u1*) malloc(sizeof(u1) * result.length);
    readu1FromFile(result.bytes, result.length, arquivo);
    return result;
}
CONSTANT_String_info ClassFile::getConstantStringInfo() {
    CONSTANT_String_info result;
    readu2FromFile(&result.string_index, 1, arquivo);
    return result;
}

CONSTANT_Fieldref_info ClassFile::getConstantFieldRefInfo() {
    CONSTANT_Fieldref_info result;
    readu2FromFile(&result.class_index, 1, arquivo);
    readu2FromFile(&result.name_and_type_index, 1, arquivo);
    return result;
}


CONSTANT_Class_info ClassFile::getConstantClassInfo() {
    CONSTANT_Class_info result;
    readu2FromFile(&result.name_index, 1, arquivo);
    return result;
}

CONSTANT_Methodref_info ClassFile::getConstantMethodRefInfo() {
    CONSTANT_Methodref_info result;

    readu2FromFile(&result.class_index, 1, arquivo);
    readu2FromFile(&result.name_and_type_index, 1, arquivo);
    return result;

}

CONSTANT_MethodHandle_info ClassFile::getConstantMethodHandleInfo()
{
    CONSTANT_MethodHandle_info result;
    readu1FromFile(&result.reference_kind, 1, arquivo);
    readu2FromFile(&result.reference_index, 1, arquivo);
    return result;
}

CONSTANT_MethodType_info ClassFile::getConstantMethodTypeInfo()
{
    CONSTANT_MethodType_info result;

    readu2FromFile(&result.descriptor_index, 1, arquivo);
    return result;
}

CONSTANT_InvokeDynamic_info ClassFile::getConstantInvokeDynamicInfo()
{
    CONSTANT_InvokeDynamic_info result;

    readu2FromFile(&result.bootstrap_method_attr_index, 1, arquivo);
    readu2FromFile(&result.name_and_type_index, 1, arquivo);

    return result;
}
