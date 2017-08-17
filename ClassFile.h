//
// Created by wagner on 15/08/17.
//



#ifndef SOFTWAREBASICOJVM_CLASSFILE_H
#define SOFTWAREBASICOJVM_CLASSFILE_H
#include <stdint.h>
#include <fstream>
#include <iostream>



class ClassFile {
private:
    std::fstream arquivo;
public:
    uint32_t 	magic_number;
    uint16_t 	minor_version;
    uint16_t 	major_version;
    uint16_t 	cp_size;
    //Cp_Info* 	constant_pool;
    uint16_t 	access_flags;
    uint16_t	this_class;
    uint16_t	super_class;
    uint16_t	interface_count;
    uint16_t*	interface_table;
    uint16_t	field_count;
    //FieldInfo* 	fields;
    uint16_t	methods_count;
    //MethodInfo*	methods;
    uint16_t	attributes_count;
    //AttributeInfo* attributes;
    /*trata a classe totalmente.*/
    void leClasse();
    /*Carrega o caminho do  arquivo para a Classe simplesmente.*/
    ClassFile(char* nome);
private:
    /*leUint32*/
    uint32_t readU32();
    uint16_t readU16();
    





};


#endif //SOFTWAREBASICOJVM_CLASSFILE_H
