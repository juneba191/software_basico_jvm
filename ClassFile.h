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
typedef struct field_info FieldInfo;
typedef struct  attribute_info attribute_info;

//attributes
typedef struct ConstantValue_attribute ConstantValue_attribute;
typedef struct Exception_table_info Exception_table_info;
typedef struct Code_attributes Code_attributes;
typedef struct Exceptions_attribute Exceptions_attribute;
typedef struct classes_info classes_info;
typedef struct Syntethic_attribute Syntethic_attribute;
typedef struct SourceFile_attribute SourceFile_attribute;
typedef struct LineNumberTable_attributes LineNumberTable_attributes;
typedef struct LocalVariableTable_attribute LocalVariableTable_attribute;
typedef struct local_variable_table local_variable_table;
typedef struct Deprecated_attribute Deprecated_attribute;




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

struct SourceFile_attribute{
    u2 sourcefile_index;
};

struct lineNumberTable{
    u2 start_pc;
    u2 line_number;
};
struct LineNumberTable_attributes{
    u2 line_number_table_length;
    lineNumberTable* lineTable;
};
struct local_variable_table{
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 descriptor_index;
    u2 index;
};


struct LocalVariableTable_attribute{
    u2 local_variable_table_length;
    local_variable_table* local_table;
};

struct Deprecated_attribute{
    //vazio
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

typedef enum CONSTANT_ACCESS_FLAGS{
    ACC_PUBLIC = 0x0001,
    ACC_PRIVATE = 0x0002,
    ACC_PROTECTED = 0x0004,
    ACC_STATIC = 0x0008,
    ACC_FINAL = 0x0010,
    ACC_VOLATILE = 0x0040,
    ACC_TRANSIENT = 0x0080,
    ACC_SYNTHETIC = 0x1000,
    ACC_ENUM = 0x4000
}CONSTANT_ACCESS_FLAGS;

typedef enum CONSTANT_ACCESS_FLAGS_INNERFLAGS{
    INNER_ACC_PUBLIC = 0x0001,
    INNER_ACC_PRIVATE = 0x0002,
    INNER_ACC_PROTECTED = 0x0004,
    INNER_ACC_STATIC = 0x0008,
    INNER_ACC_FINAL = 0x0010,
    INNER_ACC_INTERFACE = 0x0200,
    INNER_ACC_ABSTRACT = 0x0400,
    INNER_ACC_SYNTHETIC = 0x1000,
    INNER_ACC_ANNOTATION = 0x2000,
    INNER_ACC_ENUM = 0x4000
}CONSTANT_ACCESS_FLAGS_INNERFLAGS;

struct ConstantValue_attribute{
    u2 attribute_name_index;
    u4 attribute_length;
    u2 constanvalue_index;

};
struct Exception_table_info{
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;

};


struct Code_attributes {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1* code;
    u2 exception_table_length;
    Exception_table_info* exception_table;
    u2 attributes_count;
    attribute_info* attributes;
};



struct attribute_info{
    u2 attribute_name_index;
    u4 attribute_length;
    u1* info;
    //todo implementar os tipos de atributo voltar aqui.

};

struct Exceptions_attribute{
    u2 number_of_exceptions;
    u2* exceptions_index_table;
};

struct classes_info{
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
};

struct InnerClasses_attribute{
    u2 number_of_classes;
    classes_info* classes;
};

struct Syntethic_attribute{
    // ela nao possui nenhum atributo principal.
};


struct field_info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info* attributes;
};


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
    FieldInfo* 	fields;
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
    void readAcessFlag();
    void readThisClass();
    void readSuperClass();
    void readInterfaceCount();
    void readInterfaces();
    void readFieldsCount();
    void readFields();
    void carregarAtributos();

    //le os tipos de constant pool
    CONSTANT_Class_info getConstantClassInfo();
    CONSTANT_Methodref_info getConstantMethodRefInfo();
    CONSTANT_Fieldref_info getConstantFieldRefInfo();
    CONSTANT_String_info getConstantStringInfo();
    CONSTANT_Utf8_info getConstantUtf8Info();
    CONSTANT_NameAndType_info getConstantNameAndType_info();
    CONSTANT_InterfaceMethodref_info getConstantInterfaceMethodRefInfo();
    CONSTANT_Integer_info getConstantIntegerInfo();
    CONSTANT_Float_info getConstantFloatInfo();
    CONSTANT_Long_info getConstantLongInfo();
    CONSTANT_Double_info getConstantDoubleInfo();

};


#endif //SOFTWAREBASICOJVM_CLASSFILE_H
