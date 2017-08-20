//
// Created by wagner on 15/08/17.
//



#ifndef SOFTWAREBASICOJVM_CLASSFILE_H
#define SOFTWAREBASICOJVM_CLASSFILE_H
#define u4 uint32_t

#define u1 uint8_t

#define u2 uint16_t

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>

//info de constantes.
typedef struct cp_info Cp_Info;
typedef struct CONSTANT_Class_info CONSTANT_Class_info;
typedef struct CONSTANT_Fieldref_info CONSTANT_Fieldref_info;
typedef struct CONSTANT_Methodref_info CONSTANT_Methodref_info;
typedef struct CONSTANT_InterfaceMethodref_info CONSTANT_InterfaceMethodref_info;
typedef struct CONSTANT_String_info CONSTANT_String_info;
typedef struct CONSTANT_Integer_info CONSTANT_Integer_info;
typedef struct CONSTANT_Float_info CONSTANT_Float_info;
typedef struct CONSTANT_Long_info CONSTANT_Long_info;
typedef struct CONSTANT_Double_info CONSTANT_Double_info;
typedef struct CONSTANT_NameAndType_info CONSTANT_NameAndType_info;
typedef struct CONSTANT_Utf8_info CONSTANT_Utf8_info;

struct CONSTANT_Class_info {
    u1 tag;
    u2 name_index;
};

struct CONSTANT_Utf8_info{
    u1 tag;
    u2 length;
    u1* bytes; // talvez n seja assim
};



struct CONSTANT_Fieldref_info{
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_Methodref_info{
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_InterfaceMethodref_info{
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_String_info{
    u1 tag;
    u2 string_index;
};

struct CONSTANT_Integer_info{
    u1 tag;
    u2 bytes;
};

struct CONSTANT_Float_info{
    u1 tag;
    u4 bytes;
};

struct CONSTANT_Long_info{
    u1 tag;
    u4 high_bytes;
    u4 low_bytes;
};
struct CONSTANT_Double_info{
    u1 tag;
    u4 high_bytes;
    u4 low_bytes;
};

struct CONSTANT_NameAndType_info{
    u1 tag;
    u2 name_index;
    u2 descriptor_index;
};


struct cp_info{


    u1 tag;
    union {
        struct CONSTANT_Class_info class_info;
        struct CONSTANT_Fieldref_info fieldref_info;
        struct CONSTANT_Methodref_info methodref_info;
        struct CONSTANT_InterfaceMethodref_info interfaceMethodref_info;
        struct CONSTANT_String_info string_info;
        struct CONSTANT_Integer_info integer_info;
        struct CONSTANT_Float_info float_info;
        struct CONSTANT_Long_info long_info;
        struct CONSTANT_Double_info double_info;
        struct CONSTANT_NameAndType_info nameAndType_info;
        struct CONSTANT_Utf8_info utf8_info;
    } info;

};

typedef enum CONSTANT_Type {
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_String = 8,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_NameAndType = 12,
    CONSTANT_Utf8 = 1,
    CONSTANT_NULL = 0
} CONSTANT_Type;






class ClassFile {
private:
    std::string nome;
    std::fstream arquivo;
public:
    u4 	magic_number;
    u2 	minor_version;
    u2 	major_version;
    u2 	cp_size;
    Cp_Info* 	constant_pool;
    u2 	access_flags;
    u2	this_class;
    u2	super_class;
    u2	interface_count;
    u2*	interface_table;
    u2	field_count;
    //FieldInfo* 	fields;
    u2	methods_count;
    //MethodInfo*	methods;
    u2	attributes_count;
    //AttributeInfo* attributes;


    /*trata a classe totalmente.*/
    void leClasse();
    /*Carrega o caminho do  arquivo para a Classe simplesmente.*/
    ClassFile(char* nome);
private:
    /*leUint32*/
    u4 readU32();
    u2 readU16();
    u1 readU8();
    void readConstantPool();
    CONSTANT_Class_info getConstantClassInfo();
    CONSTANT_Methodref_info getConstantMethodRefInfo();
    CONSTANT_Fieldref_info getConstantFieldRefInfo();
};


#endif //SOFTWAREBASICOJVM_CLASSFILE_H
