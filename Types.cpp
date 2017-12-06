#include "Types.h"
#include "Interpreter.h"
#include <cstring>
#include <string>
#include <iostream>

void CreateBasicType(Types* type, char descr);
void CreateFieldVars(ClassInstance* cinstance);

Types::Types(char* descr){
	char abc[150];
	strcpy(abc, descr);
	std::string description(abc);

	//create type string
	if (description == "STRING") {
		tag = STRING;
		stringRef = new std::string();
	}
	//create type return address
	else if (description == "RA") {
		tag = RETURNADDRESS;
		returnAddress = new (u4);
		returnAddress = 0;
	}
	//create a basic type
	else if(descr[0] == 'B' ||  descr[0] == 'C' || descr[0] == 'D' || descr[0] == 'F' || 
		descr[0] == 'I' || descr[0] == 'J' || descr[0] == 'S' || descr[0] == 'Z')
		CreateBasicType(this, descr[0]);
	//create a array type
	else if (descr[0] == '[') {
		tag = ARRAY;
		arrayRef = (ArrayType*)malloc(sizeof(ArrayType));
		arrayRef->array = new std::vector<Types*>();
	}
	//create a instance type
	else if (descr[0] == 'L') {
		tag = CLASSINSTANCE;
		abc[strlen(abc) - 1] = 0;
		classInstance = (ClassInstance*)malloc(sizeof(ClassInstance));
		classInstance->classDescription = Interpreter::GetInstance()->GetClass(abc + 1);
	}

}

Types::Types() {
	tag = UNDEF;
}

void CreateFieldVars(ClassInstance* cinstance) {
	cinstance->fieldVars = new std::unordered_map<std::string, Types*>();
	ClassFile* fieldToAdd = cinstance->classDescription;
	char* superName = (char*)fieldToAdd->constant_pool[fieldToAdd->constant_pool[fieldToAdd->super_class - 1].info.class_info.name_index - 1].info.utf8_info.bytes;
	
	do {
		for (int i = 0; i < fieldToAdd->field_count; i++) {
			int desc = fieldToAdd->fields[i].descriptor_index;
			int name_desc = fieldToAdd->fields[i].name_index; 
			std::string key((char*)fieldToAdd->constant_pool[name_desc - 1].info.utf8_info.bytes);
			std::cout << "\n key "<< key << std::endl;
			(*cinstance->fieldVars)[key] = new Types((char*)fieldToAdd->constant_pool[desc - 1].info.utf8_info.bytes);
		}
		if(strcmp(superName, "java/lang/Object") != 0){
			fieldToAdd = Interpreter::GetInstance()->GetClass(superName);
			 superName = (char*)fieldToAdd->constant_pool[fieldToAdd->constant_pool[fieldToAdd->super_class - 1].info.class_info.name_index - 1].info.utf8_info.bytes;
		}
			
	} while (strcmp(superName, "java/lang/Object"));
}

void CreateBasicType(Types* type, char descr){
	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
	switch (descr) {
	case 'B':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->BYTE;
		type->basicRef->T_BYTE = (u4*)malloc(sizeof(u4));
		*type->basicRef->T_BYTE = 0;
		break;
	case 'C':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->CHAR;
		type->basicRef->T_CHAR = (u4*)malloc(sizeof(u4));
		*type->basicRef->T_CHAR = 0;
		break;
	case 'D':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->DOUBLE;
		type->basicRef->T_DOUBLE = (u8*)malloc(sizeof(u8));
		*type->basicRef->T_DOUBLE = 0;
		break;
	case 'F':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->FLOAT;
		type->basicRef->T_FLOAT = (u4*)malloc(sizeof(u4));
		*type->basicRef->T_FLOAT = 0;
		break;
	case 'I':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->INT;
		type->basicRef->T_INT = (u4*)malloc(sizeof(u4));
		*type->basicRef->T_INT = 0;
		break;
	case 'J':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->LONG;
		type->basicRef->T_LONG = (u8*)malloc(sizeof(u8));
		*type->basicRef->T_LONG = 0;
		break;
	case 'S':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->SHORT;
		type->basicRef->T_SHORT = (u4*)malloc(sizeof(u4));
		*type->basicRef->T_SHORT = 0;
		break;
	case 'Z':	type->basicRef = (primitiveTypes*)malloc(sizeof(primitiveTypes));
		type->tag = type->BOOL;
		type->basicRef->T_BOOLEAN = (u4*)malloc(sizeof(u4));
		*type->basicRef->T_BOOLEAN = 0;
		break;
	}
}


Types* copyTypes(Types* type) {
	Types* tipo = NULL;

	switch (type->tag) {
	case (Types::STRING):
		tipo = new Types((char*)"STRING");
		*tipo->stringRef = *type->stringRef;
		break;
	case (Types::BOOL):
		tipo = new Types((char*) "Z");
		*tipo->basicRef->T_BOOLEAN = *type->basicRef->T_BOOLEAN;
		break;
	case (Types::BYTE):
		tipo = new Types((char*) "B");
		*tipo->basicRef->T_BYTE = *type->basicRef->T_BYTE;
		break;
	case (Types::FLOAT):
		tipo = new Types((char*) "F");

		*tipo->basicRef->T_FLOAT = *type->basicRef->T_FLOAT;
		break;
	case (Types::LONG):
		tipo = new Types((char*) "J");
		*tipo->basicRef->T_LONG = *type->basicRef->T_LONG;
		break;
	case (Types::INT):
		tipo = new Types((char*) "I");
		*tipo->basicRef->T_INT = *type->basicRef->T_INT;
		break;
	case (Types::CHAR):
		tipo = new Types((char*) "C");
		*tipo->basicRef->T_CHAR = *type->basicRef->T_CHAR;
		break;
	case (Types::SHORT):
		tipo = new Types((char*) "S");
		*tipo->basicRef->T_SHORT = *type->basicRef->T_SHORT;
		break;
	case (Types::DOUBLE):
		tipo = new Types((char*) "D");
		*tipo->basicRef->T_DOUBLE = *type->basicRef->T_DOUBLE;
		break;
	case (Types::CLASSINSTANCE):
	{
		std::string aux = "L";
		Cp_Info* cpool = type->classInstance->classDescription->constant_pool + type->classInstance->classDescription->this_class - 1;
		while (cpool->tag != 1) {
			cpool = type->classInstance->classDescription->constant_pool + cpool->info.class_info.name_index - 1;

		}
		aux += std::string((char*)cpool->info.utf8_info.bytes); 
		tipo = new Types((char*)aux.c_str());
	}
		break;
	case (Types::ARRAY):
		tipo = new Types((char*)"[");
		for (unsigned int i = 0; i < type->arrayRef->array->size(); i++) {
			Types* tipoAux = copyTypes(type->arrayRef->array->at(i));
			tipo->arrayRef->array->push_back(tipoAux);
		}
		break;
	case (Types::RETURNADDRESS):
		tipo = new Types((char*)"RA");
		*tipo->returnAddress = *type->returnAddress;
		break;
	case (Types::UNDEF):
		tipo = new Types();
		break;
	}

	return tipo;
}
