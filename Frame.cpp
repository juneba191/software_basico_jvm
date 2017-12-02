#include "Frame.h"
#include "Interpreter.h"
#include "Types.h"
#include <iostream>
#include <string>
#include "ScreenPrinter.h"
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "ClassFile.h"
#include <cstring>
/*
u1* code is a reference to the code of a method
u4* pc is the current program counter of a frame
std::stack<Types*>* operandStack is the current frame's operandStack
std::vector<Types*>* locals a reference to the current local variables in the frame
Cp_Info* constant_pool a reference to the Frame's ClassFile's constant pool, to fetch items in it
ClassInstance* thisClass a reference to the class instance of the caller to modify it's fields, etc
*/
void (*operations[256])(u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass);

//Funcao para auxilio na criacao de arrays multidimensionais
void CreateMultiArray(ArrayType *array, std::string type, std::stack<int> counter);

void Frame::SetUp()
{

	operations[0x00] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
		// NOP
		(*pc)++;
	};
	operations[0x01] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
		// ACONST_NULL
		(*pc)++;
		operandStack->push(0);
	};

#define GENERATE_ICONST_LCONST(opcode, value, instType, instPrefix)                                                                                                \
	operations[opcode] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) { \
		(*pc)++;                                                                                                                                                   \
		Types *tipo = new Types((char *)#instPrefix);                                                                                                              \
		*tipo->basicRef->instType = value;                                                                                                                         \
		operandStack->push(tipo);                                                                                                                                  \
	};

	GENERATE_ICONST_LCONST(0x02, -1, T_INT, I);
	GENERATE_ICONST_LCONST(0x03, 0, T_INT, I);
	GENERATE_ICONST_LCONST(0x04, 1, T_INT, I);
	GENERATE_ICONST_LCONST(0x05, 2, T_INT, I);
	GENERATE_ICONST_LCONST(0x06, 3, T_INT, I);
	GENERATE_ICONST_LCONST(0x07, 4, T_INT, I);
	GENERATE_ICONST_LCONST(0x08, 5, T_INT, I);
	GENERATE_ICONST_LCONST(0x09, 0, T_INT, I);
	GENERATE_ICONST_LCONST(0x0a, 1, T_INT, J); /*TODO talvez seja do tipo long*/

#define GENERATE_FCONST_DCONST(opcode, value, instType, instPrefix, varType1, varType2)                                                                            \
	operations[opcode] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) { \
		(*pc)++;                                                                                                                                                   \
		varType1 aux1 = value;                                                                                                                                     \
		varType2 aux2;                                                                                                                                             \
		memcpy(&aux2, &aux1, sizeof(varType2));                                                                                                                    \
		Types *tipo = new Types((char *)#instPrefix);                                                                                                              \
		*tipo->basicRef->instType = value;                                                                                                                         \
		operandStack->push(tipo);                                                                                                                                  \
	};

	GENERATE_FCONST_DCONST(0X0b, 0.0, T_FLOAT, F, float, u4);
	GENERATE_FCONST_DCONST(0X0c, 1.0, T_FLOAT, F, float, u4);
	GENERATE_FCONST_DCONST(0X0d, 2.0, T_FLOAT, F, float, u4);
	GENERATE_FCONST_DCONST(0X0e, 0.0, T_DOUBLE, D, double, u8);
	GENERATE_FCONST_DCONST(0X0f, 1.0, T_DOUBLE, D, double, u8);

	operations[0x10] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
		// BI_PUSH
		(*pc)++;
		u1 byte = *(code + (*pc));
		(*pc)++;
		u4 aux = ((byte & 0x80) == 0) ? byte : 0x00 | byte;
		Types *tipo = new Types((char *)"I");
		*tipo->basicRef->T_INT = aux;
		operandStack->push(tipo);
	};
	operations[0x11] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
		// SI_PUSH
		(*pc)++;
		u1 byte1 = *(code + (*pc));
		(*pc)++;
		u1 byte2 = *(code + (*pc));
		(*pc)++;
		u2 aux = 0;
		aux += byte1;
		aux = (aux << 8) + byte2;
		u4 aux1 = ((aux & 0x8000) == 0) ? aux : 0xFFFF0000 + aux;
		Types *tipo = new Types((char *)"I");
		*tipo->basicRef->T_INT = aux1;
		operandStack->push(tipo);
	};

	operations[0x12] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
		// LDC
		(*pc)++;
		u1 index = *(code + (*pc));
		(*pc)++;

		Cp_Info *cpool = (constant_pool + index - 1);
		Types *tipo = NULL;

		switch (cpool->tag)
		{
		case 3: //int
			tipo = new Types((char *)"I");
			*tipo->basicRef->T_INT = cpool->info.integer_info.bytes;
			break;
		case 4: //float
			tipo = new Types((char *)"F");
			*tipo->basicRef->T_FLOAT = cpool->info.float_info.bytes;
			break;
		case 7: //classRef
		{
			std::string aux = "L";
			cpool = constant_pool + cpool->info.class_info.name_index - 1;
			char *aux2 = (char *)cpool->info.utf8_info.bytes;
			aux += std::string(aux2);
			tipo = new Types((char *)aux.c_str());
		}
		break;
		case 8: //stringRef
			tipo = new Types((char *)"STRING");
			cpool = constant_pool + cpool->info.string_info.string_index - 1;
			*tipo->stringRef = std::string((char *)cpool->info.utf8_info.bytes);
			break;
		default:
			break; //methodRef e methodHandle n�o implementados
		}

		operandStack->push(tipo);
	};
	operations[0x13] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
		// LDC_W
		(*pc)++;
		u1 index1 = *(code + (*pc));
		(*pc)++;
		u1 index2 = *(code + (*pc));
		(*pc)++;
		u2 index = index1;
		index = (index1 << 8) + index2;
		Cp_Info *cpool = (constant_pool + index - 1);
		Types *tipo;
		switch (cpool->tag)
		{
			case 3: //int
				tipo = new Types((char *)"I");
				*tipo->basicRef->T_INT = cpool->info.integer_info.bytes;
				break;
			case 4: //float
				tipo = new Types((char *)"F");
				*tipo->basicRef->T_FLOAT = cpool->info.float_info.bytes;
				break;
			case 7: //classRef
			{
				std::string aux = "L";
				cpool = constant_pool + cpool->info.class_info.name_index - 1;
				char *aux2 = (char *)cpool->info.utf8_info.bytes;
				aux += std::string(aux2);
				tipo = new Types((char *)aux.c_str());
			}
			break;
			case 8: //stringRef
				tipo = new Types((char *)"STRING");
				cpool = constant_pool + cpool->info.string_info.string_index - 1;
				*tipo->stringRef = std::string((char *)cpool->info.utf8_info.bytes);
				break;
			default:
				break; //methodRef e methodHandle n�o implementados
		}
		operandStack->push(tipo);
	};
	operations[0x14] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
		// LDC2_W
		(*pc)++;
		u1 index1 = *(code + (*pc));
		(*pc)++;
		u1 index2 = *(code + (*pc));
		(*pc)++;
		u2 index = index1;
		index = (index << 8) + index2;
		Cp_Info *cpool = (constant_pool + index - 1);
		Types *tipo;

		if (cpool->tag == 6)
		{ // double
			tipo = new Types((char *)"D");
			*tipo->basicRef->T_DOUBLE = cpool->info.double_info.value;
		}
		else
		{
			tipo = new Types((char *)"J");
			*tipo->basicRef->T_LONG = cpool->info.long_info.value;
		}
		operandStack->push(tipo);
	};

#define GENERATE_LOADS(opcode, instType, instSufix, value)                                                                                                          \
	operations[opcode] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {  \
		(*pc)++;                                                                                                                                                    \
		if(value >= 0 and #instType == "I"){																														\
			Types *toAdd = copyTypes(locals->at(value));																											\
			operandStack->push(toAdd);																																\
		}																																							\
		else if(value >= 0){																																		\
			operandStack->push(locals->at(value));																													\
		}																																							\
		else{																																						\
			u1 index = *(code + (*pc));																																\
			(*pc)++;																																				\
			operandStack->push(locals->at(index));																													\
		};\
	};\

/* gera todos os loads o value de -1 define que é pra adicionar o operando direto na pilha de locais*/
GENERATE_LOADS(0x15, I, LOAD, -1) // ILOAD
GENERATE_LOADS(0x16, L, LOAD, -1) // LLOAD
GENERATE_LOADS(0x17, F, LOAD, -1) // FLOAD
GENERATE_LOADS(0x18, D, LOAD, -1) // DLOAD
GENERATE_LOADS(0x19, A, LOAD, -1) // ALOAD

GENERATE_LOADS(0x1a, I, LOAD, 0) // ILOAD_0
GENERATE_LOADS(0x1b, I, LOAD, 1) // ILOAD_1
GENERATE_LOADS(0x1c, I, LOAD, 2) // ILOAD_2
GENERATE_LOADS(0x1d, I, LOAD, 3) // ILOAD_3

GENERATE_LOADS(0x1e, L, LOAD, 0) // LLOAD_0
GENERATE_LOADS(0x1f, L, LOAD, 1) // LLOAD_1
GENERATE_LOADS(0x20, L, LOAD, 2) // LLOAD_2
GENERATE_LOADS(0x21, L, LOAD, 3) // LLOAD_3

GENERATE_LOADS(0x22, F, LOAD, 0) // FLOAD_0
GENERATE_LOADS(0x23, F, LOAD, 1) // FLOAD_1
GENERATE_LOADS(0x24, F, LOAD, 2) // FLOAD_2
GENERATE_LOADS(0x25, F, LOAD, 3) // FLOAD_3

GENERATE_LOADS(0x26, D, LOAD, 0) // DLOAD_0
GENERATE_LOADS(0x27, D, LOAD, 1) // DLOAD_1
GENERATE_LOADS(0x28, D, LOAD, 2) // DLOAD_2
GENERATE_LOADS(0x29, D, LOAD, 3) // DLOAD_3

GENERATE_LOADS(0x2a, A, LOAD, 0) // ALOAD_0
GENERATE_LOADS(0x2b, A, LOAD, 1) // ALOAD_1
GENERATE_LOADS(0x2c, A, LOAD, 2) // ALOAD_2
GENERATE_LOADS(0x2d, A, LOAD, 3) // ALOAD_3

#define GENERATE_ALOADS(opcode, instType, instSufix, value)                                                                                                        \
	operations[opcode] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) { \
		(*pc)++;                                                                                                                                                   \
		Types *index = operandStack->top();                                                                                                                        \
		operandStack->pop();                                                                                                                                       \
		Types *arrayR = operandStack->top();                                                                                                                       \
		operandStack->pop();                                                                                                                                       \
		operandStack->push(arrayR->arrayRef->array->at(*index->basicRef->T_INT));                                                                                  \
	};

GENERATE_ALOADS(0x2e, A, ALOAD, 0) // IALOAD
GENERATE_ALOADS(0x2f, L, ALOAD, 0) // LALOAD
GENERATE_ALOADS(0x30, F, ALOAD, 0) // FALOAD
GENERATE_ALOADS(0x31, D, ALOAD, 0) // DALOAD
GENERATE_ALOADS(0x32, A, ALOAD, 0) // AALOAD
GENERATE_ALOADS(0x33, B, ALOAD, 0) // BALOAD
GENERATE_ALOADS(0x34, C, ALOAD, 0) // CALOAD
GENERATE_ALOADS(0x35, S, ALOAD, 0) // SALOAD

#define GENERATE_STORES(opcode, instType, instSufix, newVal)                                                                                                       \
	operations[opcode] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) { \
		(*pc)++;                                                                                                                                                   \
		Types *value = operandStack->top();                                                                                                                        \
		operandStack->pop();                                                                                                                                       \
		if (newVal >= 0)                                                                                                                                           \
		{                                                                                                                                                          \
			locals->at(newVal) = value;                                                                                                                            \
		}                                                                                                                                                          \
		else if (newVal == -2)                                                                                                                            		   \
		{                                                                                                                                                          \
			Types *index = operandStack->top();                                                                                                                    \
			operandStack->pop();                                                                                                                                   \
			Types *arrayR = operandStack->top();                                                                                                                   \
			operandStack->pop();                                                                                                                                   \
			if (newVal==-3)                                                                                                                               \
			{                                                                                                                                                      \
				arrayR->arrayRef->array->at(*index->basicRef->T_INT)->arrayRef->array = value->arrayRef->array;                                                    \
			}                                                                                                                                                      \
			else                                                                                                                                                   \
			{                                                                                                                                                      \
				*arrayR->arrayRef->array->at(*index->basicRef->T_INT)->basicRef->instType = *value->basicRef->instType;                                            \
			}                                                                                                                                                      \
		}                                                                                                                                                          \
		else                                                                                                                                                       \
		{                                                                                                                                                          \
			u1 index = *(code + (*pc));                                                                                                                            \
			(*pc)++;                                                                                                                                               \
			locals->at(index) = value;                                                                                                                             \
		}                                                                                                                                                          \
	};

GENERATE_STORES(0x36, T_INT, STORE, -1) // ISTORE
GENERATE_STORES(0x37, T_LONG, STORE, -1) // LSTORE
GENERATE_STORES(0x38, T_FLOAT, STORE, -1) // FSTORE
GENERATE_STORES(0x39, T_DOUBLE, STORE, -1) // DSTORE
GENERATE_STORES(0x3a, T_INT, STORE, -1)	// ASTORE
GENERATE_STORES(0x3b, T_INT, STORE, 0)  // ISTORE_0
GENERATE_STORES(0x3c, T_INT, STORE, 1)  // ISTORE_1
GENERATE_STORES(0x3d, T_INT, STORE, 2)  // ISTORE_2
GENERATE_STORES(0x3e, T_INT, STORE, 3)  // ISTORE_3
GENERATE_STORES(0x3f, T_LONG, STORE, 0)  // LSTORE_0
GENERATE_STORES(0x40, T_LONG, STORE, 1)  // LSTORE_1
GENERATE_STORES(0x41, T_LONG, STORE, 2)  // LSTORE_2
GENERATE_STORES(0x42, T_LONG, STORE, 3)  // LSTORE_3
GENERATE_STORES(0x43, T_FLOAT, STORE, 0)  // FSTORE_0
GENERATE_STORES(0x44, T_FLOAT, STORE, 1)  // FSTORE_1
GENERATE_STORES(0x45, T_FLOAT, STORE, 2)  // FSTORE_2
GENERATE_STORES(0x46, T_FLOAT, STORE, 3)  // FSTORE_3
GENERATE_STORES(0x47, T_DOUBLE, STORE, 0)  // DSTORE_0
GENERATE_STORES(0x48, T_DOUBLE, STORE, 1)  // DSTORE_1
GENERATE_STORES(0x49, T_DOUBLE, STORE, 2)  // DSTORE_2
GENERATE_STORES(0x4a, T_DOUBLE, STORE, 3)  // DSTORE_3
GENERATE_STORES(0x4b, T_INT, STORE, 0)	 // ASTORE_0
GENERATE_STORES(0x4c, T_INT, STORE, 1)   // ASTORE_1
GENERATE_STORES(0x4d, T_INT, STORE, 2)   // ASTORE_2
GENERATE_STORES(0x4e, T_INT, STORE, 3)   // ASTORE_3

GENERATE_STORES(0x4f, T_INT,     ASTORE, -2) // IASTORE
GENERATE_STORES(0x50, T_LONG,    ASTORE, -2) // LASTORE
GENERATE_STORES(0x51, T_FLOAT,   ASTORE, -2) // FASTORE
GENERATE_STORES(0x52, T_DOUBLE,  ASTORE, -2) // DASTORE
GENERATE_STORES(0x53, T_INT, ASTORE, -3)	 // AASTORE  /*Definido -3 como controle*/
GENERATE_STORES(0x54, T_BOOLEAN, ASTORE, -2) // BASTORE
GENERATE_STORES(0x55, T_CHAR,    ASTORE, -2) // CASTORE
GENERATE_STORES(0x56, T_SHORT,   ASTORE, -2) // SASTORE


operations[0x57] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// POP
	(*pc)++;
	operandStack->pop();
};
operations[0x58] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// POP2
	(*pc)++;
	Types *tipo = operandStack->top();
	operandStack->pop();
	if (tipo->tag != Types::LONG && tipo->tag != Types::DOUBLE)
	{
		operandStack->pop();
	}
};
operations[0x59] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// DUP
	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	operandStack->push(value);
	operandStack->push(value);
};
operations[0x5A] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// DUP_X1
	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	Types *value2 = operandStack->top();
	operandStack->pop();
	if (value->tag == Types::DOUBLE || value->tag == Types::LONG || value2->tag == Types::DOUBLE || value2->tag == Types::LONG)
	{
		operandStack->push(value2);
		operandStack->push(value);
	}
	else
	{
		Types *tipo = copyTypes(value);
		operandStack->push(tipo);
		operandStack->push(value2);
		operandStack->push(value);
	}
};
operations[0x5B] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// DUP_X2
	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	Types *value2 = operandStack->top();
	operandStack->pop();

	if (value->tag != Types::DOUBLE && value->tag != Types::LONG)
	{
		if (value2->tag == Types::DOUBLE || value2->tag == Types::LONG)
		{
			Types *tipo = copyTypes(value);
			operandStack->push(tipo);
			operandStack->push(value2);
			operandStack->push(value);
		}
		else
		{
			Types *value3 = operandStack->top();
			operandStack->pop();
			if (value3->tag != Types::DOUBLE && value3->tag != Types::LONG)
			{
				Types *tipo = copyTypes(value);
				operandStack->push(tipo);
				operandStack->push(value3);
				operandStack->push(value2);
				operandStack->push(value);
			}
		}
	}
};
operations[0x5C] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// DUP2
	(*pc)++;
	Types *value1 = operandStack->top();
	operandStack->pop();
	if (value1->tag != Types::DOUBLE && value1->tag != Types::LONG)
	{
		Types *value2 = operandStack->top();
		operandStack->pop();

		Types *tipo1 = copyTypes(value1);
		Types *tipo2 = copyTypes(value2);

		operandStack->push(tipo2);
		operandStack->push(tipo1);
		operandStack->push(value2);
		operandStack->push(value1);
	}
	else
	{
		Types *tipo = copyTypes(value1);
		operandStack->push(tipo);
		operandStack->push(value1);
	}
};
operations[0x5D] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// DUP2_X1
	(*pc)++;
	Types *value1 = operandStack->top();
	operandStack->pop();
	Types *value2 = operandStack->top();
	operandStack->pop();

	if (value1->tag != Types::DOUBLE && value1->tag != Types::LONG)
	{
		Types *value3 = operandStack->top();
		operandStack->pop();

		Types *tipo1 = copyTypes(value1);
		Types *tipo2 = copyTypes(value2);

		operandStack->push(tipo2);
		operandStack->push(tipo1);

		operandStack->push(value3);

		operandStack->push(value2);
		operandStack->push(value1);
	}
	else
	{
		Types *tipo1 = copyTypes(value1);

		operandStack->push(tipo1);
		operandStack->push(value2);
		operandStack->push(value1);
	}

};
operations[0x5E] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// DUP2_X2
	(*pc)++;
	Types *value1 = operandStack->top();
	operandStack->pop();
	Types *value2 = operandStack->top();
	operandStack->pop();

	Types *tipo1 = copyTypes(value1);
	if (value1->tag != Types::DOUBLE && value1->tag != Types::LONG && value2->tag != Types::DOUBLE && value2->tag != Types::LONG)
	{
		Types *value3 = operandStack->top();
		operandStack->pop();

		Types *tipo2 = copyTypes(value2);

		if (value3->tag != Types::DOUBLE && value3->tag != Types::LONG)
		{
			Types *value4 = operandStack->top();
			operandStack->pop();

			operandStack->push(tipo2);
			operandStack->push(tipo1);

			operandStack->push(value4);
			operandStack->push(value3);

			operandStack->push(value2);
			operandStack->push(value1);
		}
		else
		{

			operandStack->push(tipo2);
			operandStack->push(tipo1);

			operandStack->push(value3);
			operandStack->push(value2);
			operandStack->push(value1);
		}
	}
	else
	{

		if (value2->tag == Types::DOUBLE || value2->tag == Types::LONG)
		{
			operandStack->push(tipo1);
			operandStack->push(value2);
			operandStack->push(value1);
		}
		else
		{
			Types *value3 = operandStack->top();
			operandStack->pop();

			operandStack->push(tipo1);
			operandStack->push(value3);
			operandStack->push(value2);
			operandStack->push(value1);
		}
	}

};

operations[0x5F] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	// SWAP
	(*pc)++;
	Types *value1 = operandStack->top();
	operandStack->pop();
	Types *value2 = operandStack->top();
	operandStack->pop();
	operandStack->push(value1);
	operandStack->push(value2);
};

#define GENERATE_ARITHMETIC(opcode, instType, castType, instPrefix, instSufix, op)                                                                                 \
	operations[opcode] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) { \
		(*pc)++;                                                                                                                                                   \
		Types *value2 = operandStack->top();                                                                                                                       \
		operandStack->pop();                                                                                                                                       \
		Types *value = new Types((char *)#instPrefix);                                                                                                             \
		if (#op == "+" or #op == "-" or #op == "*" or #op == "/" or (#op == "%" and (#instSufix == "I" or #instSufix == "J")))                                     \
		{                                                                                                                                                          \
			Types *value1 = operandStack->top();                                                                                                                   \
			operandStack->pop();                                                                                                                                   \
			*value->basicRef->instType = reinterpret_cast<castType &>(*value1->basicRef->instType) op reinterpret_cast<castType &>(*value2->basicRef->instType);   \
		}                                                                                                                                                          \
		else if (#op == "-1*")                                                                                                                                     \
		{                                                                                                                                                          \
			*value->basicRef->instType = -1 * reinterpret_cast<castType &>(*value2->basicRef->instType);                                                           \
		}                                                                                                                                                          \
		else if (#op == ">>" or #op == "<<")                                                                                                                       \
		{                                                                                                                                                          \
			Types *value1 = operandStack->top();                                                                                                                   \
			operandStack->pop();                                                                                                                                   \
			*value->basicRef->instType = reinterpret_cast<castType &>(*value1->basicRef->instType) op reinterpret_cast<int &>(*value2->basicRef->T_INT);           \
		}                                                                                                                                                          \
		operandStack->push(value);                                                                                                                                 \
	};
GENERATE_ARITHMETIC(0x60, T_INT,    int,    I, ADD, +)
GENERATE_ARITHMETIC(0x61, T_LONG,   long,   J, ADD, +)
GENERATE_ARITHMETIC(0x62, T_FLOAT,  float,  F, ADD, +)
GENERATE_ARITHMETIC(0x63, T_DOUBLE, double, D, ADD, +)

GENERATE_ARITHMETIC(0x64, T_INT,    int,    I, SUB, -)
GENERATE_ARITHMETIC(0x65, T_LONG,   long,   J, SUB, -)
GENERATE_ARITHMETIC(0x66, T_FLOAT,  float,  F, SUB, -)
GENERATE_ARITHMETIC(0x67, T_DOUBLE, double, D, SUB, -)

GENERATE_ARITHMETIC(0x68, T_INT,    int,    I, MUL, *)
GENERATE_ARITHMETIC(0x69, T_LONG,   long,   J, MUL, *)
GENERATE_ARITHMETIC(0x6a, T_FLOAT,  float,  F, MUL, *)
GENERATE_ARITHMETIC(0x6b, T_DOUBLE, double, D, MUL, *)


GENERATE_ARITHMETIC(0x6c, T_INT,    int,    I, DIV, /)
GENERATE_ARITHMETIC(0x6d, T_LONG,   long,   J, DIV, /)
GENERATE_ARITHMETIC(0x6e, T_FLOAT,  float,  F, DIV, /)
GENERATE_ARITHMETIC(0x6f, T_DOUBLE, double, D, DIV, /)

GENERATE_ARITHMETIC(0x70, T_INT,    int,    I, REM, %)
GENERATE_ARITHMETIC(0x71, T_LONG,   long,   J, REM, %)

GENERATE_ARITHMETIC(0x74, T_INT,    int,    I, NEG, -1*)
GENERATE_ARITHMETIC(0x75, T_LONG,   long,   J, NEG, -1*)
GENERATE_ARITHMETIC(0x76, T_FLOAT,  float,  F, NEG, -1*)
GENERATE_ARITHMETIC(0x77, T_DOUBLE, double, D, NEG, -1*)

//frem
operations[114] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	Types *value = new Types((char *)"F");
	*value->basicRef->T_FLOAT = fmod(reinterpret_cast<float &>(*value1->basicRef->T_FLOAT), reinterpret_cast<float &>(*value2->basicRef->T_FLOAT));
	operandStack->push(value);
};
//drem
operations[115] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	double dvalue1, dvalue2, dresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	Types *value = new Types((char *)"D");
	*value->basicRef->T_DOUBLE = fmod(reinterpret_cast<double &>(*value1->basicRef->T_DOUBLE), reinterpret_cast<double &>(*value2->basicRef->T_DOUBLE));
	operandStack->push(value);

};
//ishl
operations[120] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	u4 ivalue1, ivalue2, iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	ivalue2 &= 0x0000001f;

	iresult = ivalue1 << ivalue2;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;

	operandStack->push(result);
};
//lshl
operations[121] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue1, lresult;
	u8 lvalue2;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	lvalue2 = *value2->basicRef->T_INT;
	lvalue1 = *value1->basicRef->T_LONG;

	lvalue2 &= 0x0000003f;
	lresult = lvalue1 << lvalue2;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//ishr
operations[122] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	u4 ivalue1, ivalue2, iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	iresult = (int32_t)(ivalue1 / pow(2, ivalue2));

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;

	operandStack->push(result);
};
//lshr
operations[123] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue1, lresult;
	u8 lvalue2;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	lvalue2 = *value2->basicRef->T_INT;
	lvalue1 = *value1->basicRef->T_LONG;

	lresult = (int64_t)(lvalue1 / pow(2, lvalue2 & 0x3f));

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//iushr
operations[124] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	u4 ivalue1, ivalue2, iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	ivalue2 &= 0x0000001f;

	iresult = ivalue1 >> ivalue2;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;

	operandStack->push(result);
};
//lushr
operations[125] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue1, lresult;
	u8 lvalue2;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	lvalue2 = *value2->basicRef->T_INT;
	lvalue1 = *value1->basicRef->T_LONG;

	lvalue2 &= 0x0000003f;
	lresult = lvalue1 >> lvalue2;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//iand
operations[126] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2, iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	iresult = ivalue1 & ivalue2;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;

	operandStack->push(result);
};
//land
operations[127] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue1, lvalue2, lresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	lvalue2 = *value2->basicRef->T_LONG;
	lvalue1 = *value1->basicRef->T_LONG;

	lresult = lvalue1 & lvalue2;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//ior
operations[128] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {

	u4 ivalue1, ivalue2, iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	iresult = ivalue1 | ivalue2;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;

	operandStack->push(result);
};
//lor
operations[129] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue1, lvalue2, lresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	lvalue2 = *value2->basicRef->T_LONG;
	lvalue1 = *value1->basicRef->T_LONG;

	lresult = lvalue1 | lvalue2;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//ixor
operations[130] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2, iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	iresult = ivalue1 ^ ivalue2;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;

	operandStack->push(result);
};
//lxor
operations[131] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue1, lvalue2, lresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	lvalue2 = *value2->basicRef->T_LONG;
	lvalue1 = *value1->basicRef->T_LONG;

	lresult = lvalue1 ^ lvalue2;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//iinc
operations[132] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	(*pc)++;
	u1 index = *(code + (*pc));
	(*pc)++;
	int8_t consti = *(code + (*pc));
	(*pc)++;
	int32_t inte = ((consti & 0x80) == 0) ? consti : 0x00 | consti;
	//if(consti)
	int base = (int)*(locals->at(index)->basicRef->T_INT);

	base += inte;
	*locals->at(index)->basicRef->T_INT = base;

};
//i2l
operations[133] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int64_t lresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	lresult = (long int)ivalue;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//i2f
operations[134] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	float fresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	fresult = (float)ivalue;

	Types *result = new Types((char *)"F");
	memcpy(result->basicRef->T_FLOAT, &fresult, sizeof(u4));
	operandStack->push(result);

};
//i2d
operations[135] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	double dresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;
	dresult = (double)ivalue;

	Types *result = new Types((char *)"D");

	memcpy(result->basicRef->T_DOUBLE, &dresult, sizeof(u8));
	operandStack->push(result);

};
//l2i
operations[136] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue;

	int32_t iresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	lvalue = *value->basicRef->T_LONG;

	iresult = (int32_t)lvalue;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);
};

//l2f
operations[137] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue;
	float fresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	lvalue = *value->basicRef->T_LONG;

	fresult = (float)lvalue;

	Types *result = new Types((char *)"F");
	memcpy(result->basicRef->T_FLOAT, &fresult, sizeof(u4));
	operandStack->push(result);

};

//l2d
operations[138] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue;

	double dresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	lvalue = *value->basicRef->T_LONG;

	dresult = (double)lvalue;

	Types *result = new Types((char *)"D");
	memcpy(result->basicRef->T_DOUBLE, &dresult, sizeof(u8));
	operandStack->push(result);

};
//f2i
operations[139] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	float fvalue;
	u4 iresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	memcpy(&fvalue, value->basicRef->T_FLOAT, sizeof(float));

	iresult = (int32_t)fvalue;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);
};
//f2l
operations[140] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	float fvalue;
	u8 lresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	memcpy(&fvalue, value->basicRef->T_FLOAT, sizeof(float));

	lresult = (int64_t)fvalue;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);

};
//f2d
operations[141] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	float fvalue;
	double dresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	memcpy(&fvalue, value->basicRef->T_FLOAT, sizeof(float));

	dresult = (double)fvalue;

	Types *result = new Types((char *)"D");
	memcpy(result->basicRef->T_DOUBLE, &dresult, sizeof(u8));
	operandStack->push(result);
};
//d2i
operations[142] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	double dvalue;
	u4 iresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	memcpy(&dvalue, value->basicRef->T_DOUBLE, sizeof(double));

	iresult = (int32_t)dvalue;

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);

};
//d2l
operations[143] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	double dvalue;
	u8 lresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	memcpy(&dvalue, value->basicRef->T_DOUBLE, sizeof(double));

	lresult = (int64_t)dvalue;

	Types *result = new Types((char *)"J");
	*result->basicRef->T_LONG = lresult;

	operandStack->push(result);
};
//d2f
operations[144] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	double dvalue;
	float fresult;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	memcpy(&dvalue, value->basicRef->T_DOUBLE, sizeof(double));

	fresult = (float)dvalue;

	Types *result = new Types((char *)"F");
	memcpy(result->basicRef->T_FLOAT, &fresult, sizeof(u4));
	operandStack->push(result);

};
//i2b
operations[145] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int8_t byte;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	byte = (int8_t)ivalue;

	Types *result = new Types((char *)"B");
	*result->basicRef->T_BYTE = (int32_t)byte;
	operandStack->push(result);
};
//i2c
operations[146] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	char chaar;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	chaar = (char)ivalue;
	Types *result = new Types((char *)"C");
	*result->basicRef->T_BYTE = (u4)chaar;
	operandStack->push(result);
};
//i2s
operations[147] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int16_t shorti;

	(*pc)++;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	shorti = (int16_t)ivalue;
	Types *result = new Types((char *)"S");
	*result->basicRef->T_SHORT = (int32_t)shorti;
	operandStack->push(result);
};
//lcmp
operations[148] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int64_t lvalue1, lvalue2;
	int32_t iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();
	lvalue2 = *value2->basicRef->T_LONG;
	lvalue1 = *value1->basicRef->T_LONG;

	if (lvalue1 > lvalue2)
	{
		iresult = 1;
	}
	if (lvalue1 < lvalue2)
	{
		iresult = -1;
	}
	if (lvalue1 == lvalue2)
	{
		iresult = 0;
	}

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);
};
//fcmpl
operations[149] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	float fvalue1, fvalue2;
	u4 iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	memcpy(&fvalue2, value2->basicRef->T_FLOAT, sizeof(float));
	memcpy(&fvalue1, value1->basicRef->T_FLOAT, sizeof(float));

	if (fvalue1 > fvalue2)
	{
		iresult = 1;
	}
	else if (fvalue1 == fvalue2)
	{
		iresult = 0;
	}
	else if (fvalue1 < fvalue2)
	{
		iresult = -1;
	}
	else
	{
		iresult = -1;
	}

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);
};
//fcmpg
operations[150] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	float fvalue1, fvalue2;
	u4 iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	memcpy(&fvalue2, value2->basicRef->T_FLOAT, sizeof(float));
	memcpy(&fvalue1, value1->basicRef->T_FLOAT, sizeof(float));

	if (fvalue1 > fvalue2)
	{
		iresult = 1;
	}
	else if (fvalue1 == fvalue2)
	{
		iresult = 0;
	}
	else if (fvalue1 < fvalue2)
	{
		iresult = -1;
	}
	else
	{
		iresult = 1;
	}

	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);
};
//dcmpl
operations[151] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	double dvalue1, dvalue2;
	u4 iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	memcpy(&dvalue2, value2->basicRef->T_DOUBLE, sizeof(double));
	memcpy(&dvalue1, value1->basicRef->T_DOUBLE, sizeof(double));

	if (dvalue1 > dvalue2)
	{
		iresult = 1;
	}
	else if (dvalue1 == dvalue2)
	{
		iresult = 0;
	}
	else if (dvalue1 < dvalue2)
	{
		iresult = -1;
	}
	else
	{
		iresult = -1;
	}
	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);
};
//dcmpg
operations[152] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	double dvalue1, dvalue2;
	u4 iresult;

	(*pc)++;
	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	memcpy(&dvalue2, value2->basicRef->T_DOUBLE, sizeof(double));
	memcpy(&dvalue1, value1->basicRef->T_DOUBLE, sizeof(double));

	if (dvalue1 > dvalue2)
	{
		iresult = 1;
	}
	else if (dvalue1 == dvalue2)
	{
		iresult = 0;
	}
	else if (dvalue1 < dvalue2)
	{
		iresult = -1;
	}
	else
	{
		iresult = 1;
	}
	Types *result = new Types((char *)"I");
	*result->basicRef->T_INT = iresult;
	operandStack->push(result);
};
//ifeq
operations[153] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int16_t offset = 0;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	if (ivalue == 0)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//ifne
operations[154] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int16_t offset = 0;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	if (ivalue != 0)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//iflt
operations[155] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int16_t offset = 0;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	if (ivalue < 0)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//ifle
operations[158] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int16_t offset = 0;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	if (ivalue <= 0)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//ifgt
operations[157] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int16_t offset = 0;
	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	if (ivalue > 0)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//ifge
operations[156] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue;
	int16_t offset = 0;

	Types *value = operandStack->top();
	operandStack->pop();
	ivalue = *value->basicRef->T_INT;

	if (ivalue >= 0)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//if_icmpeq
operations[159] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2;
	int16_t offset = 0;

	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	if (ivalue1 == ivalue2)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//if_icmpne
operations[160] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2;
	int16_t offset = 0;

	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	if (ivalue1 != ivalue2)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//if_icmplt
operations[161] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2;
	int16_t offset = 0;

	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	if (ivalue1 < ivalue2)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//if_icmple
operations[164] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2;
	int16_t offset = 0;

	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	if (ivalue1 <= ivalue2)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//if_icmpgt
operations[163] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2;
	int16_t offset = 0;

	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	if (ivalue1 > ivalue2)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//if_icmpge
operations[162] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int32_t ivalue1, ivalue2;
	int16_t offset = 0;

	Types *value2 = operandStack->top();
	operandStack->pop();
	Types *value1 = operandStack->top();
	operandStack->pop();

	ivalue2 = *value2->basicRef->T_INT;
	ivalue1 = *value1->basicRef->T_INT;

	if (ivalue1 >= ivalue2)
	{

		offset = *(code + (*pc + 1));
		offset <<= 8;
		offset |= *(code + (*pc + 2));

		(*pc) += offset;
	}
	else
	{
		(*pc) += 3;
	}
};
//if_acmpeq
operations[165] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	Types *value2, *value1;
	int16_t branch = (int16_t)(((*(pc + 1)) << 8) + (*(pc + 2)));

	value2 = operandStack->top();
	operandStack->pop();
	value1 = operandStack->top();
	operandStack->pop();

	if (value2->classInstance == value1->classInstance)
	{
		*pc += (int32_t)branch;
	}
};
//if_acmpne
operations[166] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	Types *value2, *value1;
	int16_t branch = (int16_t)(((*(pc + 1)) << 8) + (*(pc + 2)));

	value2 = operandStack->top();
	operandStack->pop();
	value1 = operandStack->top();
	operandStack->pop();

	if (value2->classInstance != value1->classInstance)
	{
		*pc += (int32_t)branch;
	}
	else
	{
		*pc += 3;
	}
};
//goto
operations[167] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int16_t offset = 0;

	offset = *(code + (*pc + 1));
	offset <<= 8;
	offset |= *(code + (*pc + 2));

	(*pc) += offset;
};
//jsr
operations[168] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	int16_t offset = 0;
	Types *address = new Types((char *)"RA");
	*address->returnAddress = (*pc) + 3;
	operandStack->push(address);

	offset = *(code + (*pc + 1));
	offset <<= 8;
	offset |= *(code + (*pc + 2));

	(*pc) += offset;
};
//ret
operations[169] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	u2 index = (u1) * (code + (*pc + 1));

	(*pc) = *(locals->at(index)->returnAddress); // type int usado como type  returnAddress
};
//tableswitch
operations[170] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	u4 defaultbyte = 0;
	u4 lowbyte = 0;
	u4 highbyte = 0;
	u4 *jump_offsets;
	u4 index;
	Types *value1 = operandStack->top();
	operandStack->pop();
	index = *value1->basicRef->T_INT;
	u4 start = *(pc);
	(*pc)++;

	while ((*pc) % 4 != 0)
	{
		(*pc)++;
	}

	defaultbyte = code[(*pc)++];
	defaultbyte = (defaultbyte << 8) + code[(*pc)++];
	defaultbyte = (defaultbyte << 8) + code[(*pc)++];
	defaultbyte = (defaultbyte << 8) + code[(*pc)++];

	lowbyte = code[(*pc)++];
	lowbyte = (lowbyte << 8) + code[(*pc)++];
	lowbyte = (lowbyte << 8) + code[(*pc)++];
	lowbyte = (lowbyte << 8) + code[(*pc)++];

	highbyte = code[(*pc)++];
	highbyte = (highbyte << 8) + code[(*pc)++];
	highbyte = (highbyte << 8) + code[(*pc)++];
	highbyte = (highbyte << 8) + code[(*pc)++];

	jump_offsets = (u4 *)malloc((highbyte - lowbyte + 1) * sizeof(u4));

	for (int i = 0; i < (int)(highbyte - lowbyte + 1); i++)
	{
		jump_offsets[i] = code[(*pc)++];
		jump_offsets[i] = (jump_offsets[i] << 8) + code[(*pc)++];
		jump_offsets[i] = (jump_offsets[i] << 8) + code[(*pc)++];
		jump_offsets[i] = (jump_offsets[i] << 8) + code[(*pc)++];
	}

	if ((int32_t)index < (int32_t)lowbyte || (int32_t)index > (int32_t)highbyte)
	{
		(*pc) = start + (int32_t)defaultbyte;
	}
	else
	{
		(*pc) = start + (int32_t)jump_offsets[index - lowbyte];
	}
};

operations[171] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//LOOKUPSWITCH ACESS JUMP TABLE BY KEY AND JUMP
	u4 defaultbyte = 0;
	u4 npairs = 0;
	u4 *jump_keys;
	u4 *jump_offsets;
	u4 key;
	u4 start = *pc;

	Types *value1 = operandStack->top();
	operandStack->pop();
	key = *value1->basicRef->T_INT;
	(*pc)++;

	while ((*pc) % 4 != 0)
	{
		(*pc)++;
	}

	defaultbyte = code[(*pc)++];
	defaultbyte = (defaultbyte << 8) + code[(*pc)++];
	defaultbyte = (defaultbyte << 8) + code[(*pc)++];
	defaultbyte = (defaultbyte << 8) + code[(*pc)++];

	npairs = code[(*pc)++];
	npairs = (npairs << 8) + code[(*pc)++];
	npairs = (npairs << 8) + code[(*pc)++];
	npairs = (npairs << 8) + code[(*pc)++];

	jump_keys = (u4 *)malloc(npairs * sizeof(u4));
	jump_offsets = (u4 *)malloc(npairs * sizeof(u4));
	for (int i = 0; i < (int)npairs; i++)
	{
		jump_keys[i] = code[(*pc)++];
		jump_keys[i] = (jump_keys[i] << 8) + code[(*pc)++];
		jump_keys[i] = (jump_keys[i] << 8) + code[(*pc)++];
		jump_keys[i] = (jump_keys[i] << 8) + code[(*pc)++];

		jump_offsets[i] = code[(*pc)++];
		jump_offsets[i] = (jump_offsets[i] << 8) + code[(*pc)++];
		jump_offsets[i] = (jump_offsets[i] << 8) + code[(*pc)++];
		jump_offsets[i] = (jump_offsets[i] << 8) + code[(*pc)++];
	}

	int i;
	for (i = 0; i < (int)npairs; ++i)
	{
		if (jump_keys[i] == key)
		{
			*(pc) = start + jump_offsets[i];
			break;
		}
	}
	if (i == npairs)
		*(pc) = start + defaultbyte;
};

operations[172] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//IRETURN
	(*pc)++;
	Types *returnValue;

	returnValue = operandStack->top();
	Interpreter::GetInstance()->PopFrame();
	Interpreter::GetInstance()->GetTopFrame()->PushOperandStack(returnValue);
};

operations[173] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//LRETURN
	(*pc)++;
	Types *returnValue;

	returnValue = operandStack->top();
	operandStack->pop();
	Interpreter::GetInstance()->PopFrame();
	Interpreter::GetInstance()->GetTopFrame()->PushOperandStack(returnValue);
};

operations[174] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//FRETURN
	(*pc)++;
	Types *returnValue;

	returnValue = operandStack->top();
	Interpreter::GetInstance()->PopFrame();
	Interpreter::GetInstance()->GetTopFrame()->PushOperandStack(returnValue);
};

operations[175] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//DRETURN
	(*pc)++;
	Types *returnValue;

	returnValue = operandStack->top();
	operandStack->pop();

	Interpreter::GetInstance()->PopFrame();

	Interpreter::GetInstance()->GetTopFrame()->PushOperandStack(returnValue);
};

operations[176] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//ARETURN
	(*pc)++;
	Types *returnValue;

	returnValue = operandStack->top();

	Interpreter::GetInstance()->PopFrame();
	Interpreter::GetInstance()->GetTopFrame()->PushOperandStack(returnValue);
};

operations[177] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//RET
	(*pc)++;
	Interpreter::GetInstance()->PopFrame();
};

operations[178] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//GETSTATIC
	(*pc)++;
	u2 index = code[(*pc)++];
	index = (index << 8) + code[(*pc)++];

	u2 className = constant_pool[index - 1].info.fieldref_info.class_index; //name and type ou class?
	className = constant_pool[className - 1].info.class_info.name_index;

	u2 fieldName = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldName = constant_pool[fieldName - 1].info.nameAndType_info.name_index;

	u2 fieldType = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldType = constant_pool[fieldType - 1].info.nameAndType_info.descriptor_index;

	Types *fieldVar;

	if (!strcmp((char *)constant_pool[className - 1].info.utf8_info.bytes, "java/lang/System"))
		return;

	ClassFile *classFile = Interpreter::GetInstance()->GetClass((char *)constant_pool[className - 1].info.utf8_info.bytes);
	fieldVar = Interpreter::GetInstance()->GetStaticField((char *)constant_pool[className - 1].info.utf8_info.bytes,
														  (char *)constant_pool[fieldName - 1].info.utf8_info.bytes);

	operandStack->push(fieldVar);
};

operations[179] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//PUTSTATIC
	(*pc)++;
	Types *var;
	u2 index = code[(*pc)++];
	index = (index << 8) + code[(*pc)++];

	u2 className = constant_pool[index - 1].info.fieldref_info.class_index;
	className = constant_pool[className - 1].info.class_info.name_index;

	u2 fieldName = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldName = constant_pool[fieldName - 1].info.nameAndType_info.name_index;

	u2 fieldType = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldType = constant_pool[fieldType - 1].info.nameAndType_info.descriptor_index;

	Types *fieldVar;

	ClassFile *classFile = Interpreter::GetInstance()->GetClass((char *)constant_pool[className - 1].info.utf8_info.bytes);
	fieldVar = Interpreter::GetInstance()->GetStaticField((char *)constant_pool[className - 1].info.utf8_info.bytes,
														  (char *)constant_pool[fieldName - 1].info.utf8_info.bytes);

	switch (fieldVar->tag)
	{
	case Types::INT:
		var = operandStack->top();
		operandStack->pop();
		*fieldVar->basicRef->T_INT = *var->basicRef->T_INT;
		break;
	case Types::FLOAT:
		var = operandStack->top();
		operandStack->pop();
		*fieldVar->basicRef->T_FLOAT = *var->basicRef->T_FLOAT;
		break;
	case Types::SHORT:
		var = operandStack->top();
		operandStack->pop();
		(*fieldVar->basicRef->T_SHORT) = *var->basicRef->T_SHORT;
		break;
	case Types::BOOL:
		var = operandStack->top();
		operandStack->pop();
		(*fieldVar->basicRef->T_BOOLEAN) = *var->basicRef->T_BOOLEAN;
		break;
	case Types::CHAR:
		var = operandStack->top();
		(*fieldVar->basicRef->T_CHAR) = *var->basicRef->T_CHAR;
		operandStack->pop();
		break;
	case Types::CLASSINSTANCE:
		var = operandStack->top();
		operandStack->pop();
		(fieldVar->classInstance) = var->classInstance;
		break;
	case Types::ARRAY:
		var = operandStack->top();
		operandStack->pop();
		(fieldVar->arrayRef) = var->arrayRef;
		break;
	case Types::DOUBLE:
		var = operandStack->top();
		*fieldVar->basicRef->T_DOUBLE = *var->basicRef->T_DOUBLE;
		break;
	case Types::LONG:
	{
		var = operandStack->top();
		operandStack->pop();
		*fieldVar->basicRef->T_LONG = *var->basicRef->T_LONG;
	}
	break;
	}
};

operations[180] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//GETFIELD
	(*pc)++;
	u2 index = code[(*pc)++];
	index = (index << 8) + code[(*pc)++];

	u2 className = constant_pool[index - 1].info.fieldref_info.class_index;
	className = constant_pool[className - 1].info.class_info.name_index;

	u2 fieldName = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldName = constant_pool[fieldName - 1].info.nameAndType_info.name_index;

	u2 fieldType = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldType = constant_pool[fieldType - 1].info.nameAndType_info.descriptor_index;

	Types *fieldVar;
	Types *var = (Types *)operandStack->top();

	//Interpreter::GetInstance()->GetClass((char*)constant_pool[className - 1].info.utf8_info.bytes);
	std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);

	fieldVar = thisClass->fieldVars->at(key);

	operandStack->push(fieldVar);
};

operations[181] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//PUTFIELD
	(*pc)++;
	Types *var;
	u2 index = code[(*pc)++];
	index = (index << 8) + code[(*pc)++];

	u2 className = constant_pool[index - 1].info.fieldref_info.class_index;
	className = constant_pool[className - 1].info.class_info.name_index;

	u2 fieldName = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldName = constant_pool[fieldName - 1].info.nameAndType_info.name_index;

	u2 fieldType = constant_pool[index - 1].info.fieldref_info.name_and_type_index;
	fieldType = constant_pool[fieldType - 1].info.nameAndType_info.descriptor_index;

	Types *fieldVar = new Types((char *)constant_pool[fieldType - 1].info.utf8_info.bytes);

	Types::TAGTYPE type = fieldVar->tag;

	//ClassFile *ClassFile = Interpreter::GetInstance()->GetClass((char*)constant_pool[className - 1].info.utf8_info.bytes);

	switch (type)
	{
	case Types::INT:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		//fieldVar = /*(*objectref->classInstance->fieldVars)*/[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		*fieldVar->basicRef->T_INT = *var->basicRef->T_INT;
	}
	break;
	case Types::FLOAT:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		*fieldVar->basicRef->T_FLOAT = *var->basicRef->T_FLOAT;
	}
	break;
	case Types::SHORT:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		(*fieldVar->basicRef->T_SHORT) = *var->basicRef->T_SHORT;
	}
	break;
	case Types::BOOL:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		(*fieldVar->basicRef->T_BOOLEAN) = *var->basicRef->T_BOOLEAN;
	}
	break;
	case Types::CHAR:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		(*fieldVar->basicRef->T_CHAR) = *var->basicRef->T_CHAR;
	}
	break;
	case Types::CLASSINSTANCE:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		(fieldVar->classInstance) = (ClassInstance *)var->classInstance;
	}
	break;
	case Types::ARRAY:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		(fieldVar->arrayRef) = (ArrayType *)var->arrayRef;
	}
	break;
	case Types::DOUBLE:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		*fieldVar->basicRef->T_DOUBLE = *var->basicRef->T_DOUBLE;
	}
	break;
	case Types::LONG:
	{
		var = operandStack->top();
		operandStack->pop();
		Types *objectref = (Types *)operandStack->top();
		operandStack->pop();
		std::string key((char *)constant_pool[fieldName - 1].info.utf8_info.bytes);
		fieldVar = thisClass->fieldVars->at(key);
		//fieldVar = (*objectref->classInstance->fieldVars)[(char*)constant_pool[className - 1].info.utf8_info.bytes];
		*fieldVar->basicRef->T_LONG = *var->basicRef->T_LONG;
	}
	break;
	}
};

operations[182] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//INVOKEVIRTUAL BOYS

	u2 methodIndex = code[++(*pc)];
	methodIndex = (methodIndex << 8) | code[++(*pc)];
	(*pc)++;

	Cp_Info *methodRef = &constant_pool[methodIndex - 1];

	u2 classNameIndex = constant_pool[(u2)((methodRef->info.methodref_info.class_index)) - 1].info.class_info.name_index;
	std::string className((char *)constant_pool[classNameIndex - 1].info.utf8_info.bytes);

	u2 nameAndTypeIndex = (u2)methodRef->info.methodref_info.name_and_type_index;
	Cp_Info *nameAndType = &constant_pool[nameAndTypeIndex - 1];

	std::string methodName = (char *)constant_pool[(u2)((nameAndType->info.nameAndType_info.name_index)) - 1].info.utf8_info.bytes;
	std::string methodDescriptor = (char *)constant_pool[(u2)(nameAndType->info.nameAndType_info.descriptor_index) - 1].info.utf8_info.bytes;

	if (strstr(className.c_str(), "java/"))
	{
		// Call of print or println
		if (className == "java/io/PrintStream" && (methodName == "print" || methodName == "println"))
		{
			if (methodDescriptor != "()V")
			{
				Types *streamRef = operandStack->top();
				operandStack->pop();
				float a = 2;
				switch (streamRef->tag)
				{
				case (Types::STRING):
					std::cout << *streamRef->stringRef;
					break;
				case (Types::ARRAY):
					std::cout << streamRef->arrayRef;
					break;
				case (Types::BOOL):
					std::cout << (bool)*streamRef->basicRef->T_BOOLEAN;
					break;
				case (Types::BYTE):
					std::cout << *streamRef->basicRef->T_BYTE;
					break;
				case (Types::CHAR):
					std::cout << (char)*streamRef->basicRef->T_CHAR;
					break;
				case (Types::CLASSINSTANCE):
					std::cout << streamRef->classInstance;
					break;
				case (Types::DOUBLE):
				{
					double aux;
					memcpy(&aux, streamRef->basicRef->T_DOUBLE, sizeof(double));
					printf("%.15lf", (double)aux);
				}
				break;
				case (Types::FLOAT):
				{
					float aux;
					memcpy(&aux, streamRef->basicRef->T_FLOAT, sizeof(float));
					printf("%f", (float)aux);
				}
				break;
				case (Types::INT):
					std::cout << (int32_t)*streamRef->basicRef->T_INT;
					break;
				case (Types::LONG):
					std::cout << (int64_t)*streamRef->basicRef->T_LONG;
					break;
				case (Types::SHORT):
					std::cout << *streamRef->basicRef->T_SHORT;
					break;
				}
			}
			if (methodName == "println")
			{
				std::cout << std::endl;
			}
		}
		else if (className == "java/lang/String" && methodName == "equals")
		{
			Types *ref1, *ref2;
			ref2 = operandStack->top();
			operandStack->pop();
			ref1 = operandStack->top();
			operandStack->pop();

			if (!strcmp(ref1->stringRef->c_str(), ref2->stringRef->c_str()))
			{
				Types *toReturn = new Types((char *)"Z");
				*toReturn->basicRef->T_BOOLEAN = 1;
				operandStack->push(toReturn);
			}
			else
			{
				Types *toReturn = new Types((char *)"Z");
				*toReturn->basicRef->T_BOOLEAN = 0;
				operandStack->push(toReturn);
			}
		}
		else if (className == "java/lang/String" && methodName == "length")
		{
			Types *ref1 = operandStack->top();

			Types *toAdd = new Types((char *)"I");
			*toAdd->basicRef->T_INT = ref1->stringRef->size();
			operandStack->pop();
			operandStack->push(toAdd);
		}
		else if (className == "java/lang/StringBuilder" && methodName == "append")
		{
			Types *reftoAppend = operandStack->top();
			operandStack->pop();
			Types *refToString = operandStack->top();
			operandStack->pop();
			std::stringstream toAppend;
			switch (reftoAppend->tag)
			{
			case (Types::STRING):
				toAppend << *reftoAppend->stringRef;
				break;
			case (Types::ARRAY):
				toAppend << reftoAppend->arrayRef;
				break;
			case (Types::BOOL):
				toAppend << *reftoAppend->basicRef->T_BOOLEAN;
				break;
			case (Types::BYTE):
				toAppend << *reftoAppend->basicRef->T_BYTE;
				break;
			case (Types::CHAR):
				toAppend << *reftoAppend->basicRef->T_CHAR;
				break;
			case (Types::CLASSINSTANCE):
				toAppend << reftoAppend->classInstance;
				break;
			case (Types::DOUBLE):
			{
				double aux;
				memcpy(&aux, reftoAppend->basicRef->T_DOUBLE, sizeof(double));
				toAppend << aux;
			}
			break;
			case (Types::FLOAT):
			{
				float aux;
				memcpy(&aux, reftoAppend->basicRef->T_FLOAT, sizeof(float));
				toAppend << aux;
			}
			break;
			case (Types::INT):
				toAppend << (int32_t)*reftoAppend->basicRef->T_INT;
				break;
			case (Types::LONG):
				toAppend << (int64_t)*reftoAppend->basicRef->T_LONG;
				break;
			case (Types::SHORT):
				toAppend << *reftoAppend->basicRef->T_SHORT;
				break;
			}

			refToString->stringRef->append(toAppend.str());
			operandStack->push(refToString);
		}
		else if (className == "java/lang/StringBuilder")
		{
			return;
		}
		else
		{
			std::cout << methodName << "Nao implementado nesta JVM" << std::endl;
			exit(1);
		}
	}
	else
	{
		u2 nargs = 0;
		u2 i = 1;
		while (methodDescriptor[i] != ')')
		{
			char baseType = methodDescriptor[i];
			if (baseType == 'L')
			{
				nargs++;
				while (methodDescriptor[++i] != ';')
					;
			}
			else if (baseType == '[')
			{
				nargs++;
				while (methodDescriptor[++i] == '[')
					;
				if (methodDescriptor[i] == 'L')
					while (methodDescriptor[++i] != ';')
						;
			}
			else
			{
				nargs++;
			}
			i++;
		}

		std::vector<Types *> arguments;
		for (int i = 0; i < nargs; i++)
		{
			Types *value = operandStack->top();
			operandStack->pop();
			arguments.insert(arguments.begin(), value);
			if (value->tag == Types::DOUBLE || value->tag == Types::LONG)
				arguments.insert(arguments.begin() + 1, new Types());
		}

		Types *objectValue = operandStack->top();
		arguments.insert(arguments.begin(), objectValue);

		operandStack->pop();

		//Receives cFile from Classes' map, constantPool from the current class, method index from code attribute, cInstance as current class instance and a vector of method's arguments
		Frame *nextFrame = new Frame(objectValue->classInstance->classDescription, constant_pool, nameAndTypeIndex, objectValue->classInstance, &arguments);

		// se a stack frame mudou, � porque teve <clinit> adicionado, ent�o terminar a execu��o da instru��o para eles serem executados.
		Interpreter::GetInstance()->PushFrame(nextFrame);
	}
};

operations[183] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//InvokeSpecial

	u2 methodIndex = code[++(*pc)];
	methodIndex = (methodIndex << 8) | code[++(*pc)];
	(*pc)++;

	Cp_Info *methodRef = &constant_pool[methodIndex - 1];

	u2 classNameIndex = constant_pool[(u2)(methodRef->info.methodref_info.class_index) - 1].info.class_info.name_index;
	std::string className((char *)constant_pool[classNameIndex - 1].info.utf8_info.bytes);

	u2 nameAndTypeIndex = (u2)methodRef->info.methodref_info.name_and_type_index;
	Cp_Info *nameAndType = &constant_pool[nameAndTypeIndex - 1];

	std::string methodName = (char *)constant_pool[(u2)(nameAndType->info.nameAndType_info.name_index) - 1].info.utf8_info.bytes;
	std::string methodDescriptor = (char *)constant_pool[(u2)(nameAndType->info.nameAndType_info.descriptor_index) - 1].info.utf8_info.bytes;

	// Caso de Object<init>, String<init>, StringBuilder<init>
	if ((className == "java/lang/Object" || className == "java/lang/String") && methodName == "<init>" ||
		(className == "java/lang/StringBuilder" && methodName == "<init>"))
	{
		if (className == "java/lang/String" || className == "java/lang/StringBuilder")
		{
			operandStack->pop();
		}
		else if (methodName == "<init>")
		{
			CreateFieldVars(locals->at(0)->classInstance);
		}
		return;
	}
	//Caso forem outras classes do java nao implementadas aqui
	if (strstr(className.c_str(), (char *)"java/") != NULL)
	{
		std::cout << className << " Nao implementada!\n";
		exit(1);
	}
	else
	{
		u2 nargs = 0; // numero de argumentos contidos na pilha de operandos
		u2 index = 1;
		while (methodDescriptor[index] != ')')
		{
			char baseType = methodDescriptor[index];
			//Argumento do tipo objeto
			if (baseType == 'L')
			{
				nargs++;
				while (methodDescriptor[++index] != ';')
					;
			}
			//Argumento do tipo array
			else if (baseType == '[')
			{
				nargs++;
				while (methodDescriptor[++index] == '[')
					;
				if (methodDescriptor[index] == 'L')
					while (methodDescriptor[++index] != ';')
						;
			}
			else
			{
				nargs++;
			}
			index++;
		}

		std::vector<Types *> args;
		for (int i = 0; i < nargs; i++)
		{
			Types *value = operandStack->top();
			operandStack->pop();
			args.insert(args.begin(), value);
			if (value->tag == Types::DOUBLE || value->tag == Types::LONG)
				args.insert(args.begin() + 1, new Types());
		}

		//Adicionar objectref pro inicio dos argumentos
		Types *objectValue = operandStack->top();
		operandStack->pop();
		args.insert(args.begin(), objectValue);

		ClassInstance *object = objectValue->classInstance;
		Frame *nextFrame = new Frame(objectValue->classInstance->classDescription, constant_pool, nameAndTypeIndex, object, &args);
		Interpreter::GetInstance()->PushFrame(nextFrame);
	}
};
operations[184] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	//INVOKE STATIC
	u2 methodIndex = code[++(*pc)];
	methodIndex = (methodIndex << 8) | code[++(*pc)];
	(*pc)++;

	Cp_Info *methodRef = &constant_pool[methodIndex - 1];

	u2 classNameIndex = constant_pool[(u2)((methodRef->info.methodref_info.class_index)) - 1].info.class_info.name_index;
	std::string className((char *)constant_pool[classNameIndex - 1].info.utf8_info.bytes);

	u2 nameAndTypeIndex = (u2)methodRef->info.methodref_info.name_and_type_index;
	Cp_Info *nameAndType = &constant_pool[nameAndTypeIndex - 1];

	std::string methodName = (char *)constant_pool[(u2)(nameAndType->info.nameAndType_info.name_index) - 1].info.utf8_info.bytes;
	std::string methodDescriptor = (char *)constant_pool[(u2)(nameAndType->info.nameAndType_info.descriptor_index) - 1].info.utf8_info.bytes;

	if (className == "java/lang/Object" && methodName == "registerNatives")
	{
		//nao trabalhar com codigo nativo
		return;
	}

	if (strstr(className.c_str(), (char *)"java/") != NULL)
	{
		//se for alguma classe do java nao implementada aqui
		exit(1);
	}
	else
	{
		u2 nargs = 0;
		u2 i = 1;
		while (methodDescriptor[i] != ')')
		{
			char baseType = methodDescriptor[i];
			if (baseType == 'L')
			{
				nargs++;
				while (methodDescriptor[++i] != ';')
					;
			}
			else if (baseType == '[')
			{
				nargs++;
				while (methodDescriptor[++i] == '[')
					;
				if (methodDescriptor[i] == 'L')
					while (methodDescriptor[++i] != ';')
						;
			}
			else
			{
				nargs++;
			}
			i++;
		}

		std::vector<Types *> args;
		for (int i = 0; i < nargs; i++)
		{
			Types *value = operandStack->top();
			operandStack->pop();
			args.insert(args.begin(), value);
			if (value->tag == Types::DOUBLE || value->tag == Types::LONG)
				args.insert(args.begin() + 1, new Types());
		}

		ClassInstance *staticInstance = Interpreter::GetInstance()->GetStaticInstance(className.c_str());

		Frame *nextFrame = new Frame(staticInstance->classDescription, constant_pool, nameAndTypeIndex, staticInstance, &args);
		Interpreter::GetInstance()->PushFrame(nextFrame);
	}
};

//new
operations[187] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {

	u2 indexByte1 = code[(*pc) + 1];
	u2 indexByte2 = code[(*pc) + 2];
	u2 index = (indexByte1 << 8) | indexByte2;

	u2 classRef = constant_pool[index - 1].info.class_info.name_index;
	char *className = (char *)constant_pool[classRef - 1].info.utf8_info.bytes;
	std::string loko("L");
	loko += className;

	if (!strcmp(className, "java/lang/StringBuilder"))
	{
		Types *toAdd = new Types((char *)"STRING");
		operandStack->push(toAdd);
		(*pc) += 3;
		return;
	}

	loko += ";";

	Types *tipo = new Types((char *)loko.c_str());
	if (tipo->classInstance->classDescription == NULL)
		Interpreter::GetInstance()->PushException((char *)"Link Error!");

	operandStack->push(tipo);
	(*pc) += 3;
	return;
};
//newarray
operations[188] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	Types *value = operandStack->top();
	u4 counter = *value->basicRef->T_INT;
	operandStack->pop();
	(*pc)++;
	std::string loko = "[";
	Types *tipo = new Types((char *)loko.c_str());
	Types *toAdd;
	u1 atype = code[(*pc)++];
	switch (atype)
	{
	case 4: //T_BOOLEAN
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)"B"));
		}
		break;

	case 5: //T_CHAR
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)"C"));
		}
		break;

	case 6: //T_FLOAT
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)"F"));
		}
		break;

	case 7: //T_DOUBLE
		for (int i = 0; i < (int)counter; i++)
		{
			toAdd = new Types((char *)"D");
			tipo->arrayRef->array->emplace_back(toAdd);
		}
		break;

	case 8: //T_BYTE
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)"Z"));
		}
		break;

	case 9: // T_SHORT
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)"S"));
		}
		break;

	case 10: // T_INT
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)"I"));
		}
		break;

	case 11: // T_LONG
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)"J"));
		}
		break;
	}

	operandStack->push(tipo);

};
//anewarray
operations[189] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	Types *value = operandStack->top();
	u4 counter = *value->basicRef->T_INT;
	operandStack->pop();
	(*pc)++;

	Types *tipo = 0;

	u2 indexByte1 = code[(*pc)++];
	u2 indexByte2 = code[(*pc)++];
	u2 index = (indexByte1 << 8) | indexByte2;

	u1 tag = constant_pool[index - 1].tag;

	u2 classRef;
	char *className;
	std::string loko;

	classRef = constant_pool[index - 1].info.class_info.name_index;
	className = (char *)constant_pool[classRef - 1].info.utf8_info.bytes;
	loko = "L";
	loko += className;
	loko += ";";
	tipo = new Types((char *)loko.c_str());

	switch (tag)
	{
	case 7: //class
			//nao e tratado string diferentemente pois nao existe tipo basico.
		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)loko.c_str()));
		}

		break;
	case 11: //intarface

		for (int i = 0; i < (int)counter; i++)
		{
			tipo->arrayRef->array->emplace_back(new Types((char *)loko.c_str()));
		}
		break;
	}
	operandStack->push(tipo);
};

//multianewarray
operations[197] = [](u1 *code, u4 *pc, std::stack<Types *> *operandStack, std::vector<Types *> *locals, Cp_Info *constant_pool, ClassInstance *thisClass) {
	(*pc)++;
	Types *tipo = 0;

	u2 indexByte1 = code[(*pc)++];
	u2 indexByte2 = code[(*pc)++];
	u2 index = (indexByte1 << 8) | indexByte2;

	u1 tag = constant_pool[index - 1].tag;
	u1 dimension = code[(*pc)++];
	u2 classRef;

	std::stack<int> counter;

	for (u1 i = 0; i < dimension; i++)
	{
		Types *value = operandStack->top();
		counter.push(*value->basicRef->T_INT);
		operandStack->pop();
	}

	classRef = constant_pool[index - 1].info.class_info.name_index;
	std::string loko((char *)constant_pool[classRef - 1].info.utf8_info.bytes + dimension);

	Types *aux = new Types((char *)"[");

	CreateMultiArray(aux->arrayRef, loko, counter);

	operandStack->push(aux);
};
}

void CreateMultiArray(ArrayType *array, std::string type, std::stack<int> counter)
{
	int c = counter.top();
	counter.pop();

	if (counter.size() == 0)
	{
		for (int i = 0; i < c; i++)
		{
			Types *toAdd = new Types((char *)type.c_str());
			array->array->emplace_back(toAdd);
		}
	}
	else
	{
		for (int i = 0; i < c; i++)
		{
			Types *toAdd = new Types((char *)"[");
			CreateMultiArray(toAdd->arrayRef, type, counter);
			array->array->emplace_back(toAdd);
		}
	}
}

void Frame::Execute()
{
	//std::cout<<current_pc<<" "<<(int)codeAttribute->code[current_pc]<<std::endl;
	operations[*(codeAttribute->code + current_pc)](codeAttribute->code, &current_pc, &operandStack, &locals, classFile->constant_pool, thisClass);
}

//Receives cFile from Classes' map, constantPool from the current class, method index from code attribute, cInstance as current class instance and a vector of method's arguments
Frame::Frame(ClassFile *cFile, Cp_Info *constantPool, u2 methodIndex, ClassInstance *cInstance, std::vector<Types *> *args)
{
	//methodNaT is a reference to the method's name and type
	Cp_Info *methodNaT = (constantPool + methodIndex - 1);

	exceptionsAttribute = NULL;
	current_pc = 0;

	//Get method's name and return type char* ref1
	char *methodName = (char *)(constantPool[(methodNaT->info.nameAndType_info.name_index) - 1].info.utf8_info.bytes);
	char *methodType = (char *)(constantPool[(methodNaT->info.nameAndType_info.descriptor_index) - 1].info.utf8_info.bytes); //TODO n sei o que acontece aqui

	//Set up classes vars
	classFile = cFile;

	/* TESTE CIRACAO DE FIELDS
	FIELDS SAO CRIADOS NO OBJECT<INIT>
	this->thisClass = (ClassInstance*)malloc(sizeof(ClassInstance));
	this->thisClass->classDescription = cFile;
	CreateFieldVars(thisClass);*/

	//Find method in class
	for (method_index = 0; method_index < cFile->methods_count; ++method_index)
	{
		auto aux = cFile->constant_pool[cFile->methods[method_index].name_index - 1].info.utf8_info.bytes;
		auto aux2 = cFile->constant_pool[cFile->methods[method_index].descriptor_index - 1].info.utf8_info.bytes;

		if (!strcmp((char *)aux, (char *)methodName) && !strcmp((char *)aux2, methodType))
			break;
	}
	//if method not found
	if (method_index == cFile->methods_count)
		return;

	//Find and assign code attribute
	for (int i = 0; i < cFile->methods[method_index].attributes_count; i++)
	{
		int strref = (cFile->methods[method_index].attributes[i].attribute_name_index) - 1;
		if (!strcmp((char *)cFile->constant_pool[strref].info.utf8_info.bytes, "Code"))
		{
			codeAttribute = &(cFile->methods[method_index].attributes + i)->info.code_info; //n sei se ta certo com endereço
		}
		if (!strcmp((char *)cFile->constant_pool[strref].info.utf8_info.bytes, "Exceptions"))
		{
			exceptionsAttribute = &(cFile->methods[method_index].attributes + i)->info.exceptions_info;
		}
	}

	this->locals.resize(codeAttribute->max_locals);

	thisClass = cInstance;

	for (int i = 0; i < args->size(); i++)
	{
		this->locals[i] = args->at(i);
	}

	//load local variables
	/*for(int i = 0; i < codeAttribute->attributes_count; ++i){
		int strref = codeAttribute->attributes[i].attribute_name_index;
		if (!strcmp((char*)cFile->constant_pool[strref - 1].info.utf8_info.bytes, "LocalVariableTable")) {
			for (int j = 0; j < codeAttribute->attributes[i].localVariableAttribute->local_variable_table_length; j++) {
				int typeRefIndex = codeAttribute->attributes[i].localVariableAttribute->local_variable_table[j].descriptor_index;
				locals.emplace_back(new Types((char*)(cFile->constant_pool[typeRefIndex - 1].info.utf8_info.bytes)));
			}
		}
	}*/
}

Frame::Frame(ClassFile *cFile, Cp_Info *constantPool, ClassInstance *cInstance, u2 method_Index)
{
	//methodNaT is a reference to the method's name and type
	exceptionsAttribute = NULL;

	//Set up classes vars
	classFile = cFile;
	method_index = method_Index;
	//Find and assign code attribute
	for (int i = 0; i < cFile->methods[method_index].attributes_count; i++)
	{
		int strref = (cFile->methods[method_index].attributes[i].attribute_name_index) - 1;
		if (!strcmp((char *)cFile->constant_pool[strref].info.utf8_info.bytes, "Code"))
		{
			codeAttribute = &(cFile->methods[method_index].attributes + i)->info.code_info;
		}
		if (!strcmp((char *)cFile->constant_pool[strref].info.utf8_info.bytes, "Exceptions"))
		{
			exceptionsAttribute = &(cFile->methods[method_index].attributes + i)->info.exceptions_info;
		}
	}

	locals.resize(codeAttribute->max_locals);
	for (int i = 0; i < cFile->field_count; i++)
	{
		locals[i] = new Types((char *)cFile->constant_pool[cFile->fields[i].descriptor_index - 1].info.utf8_info.bytes);
	}

	this->thisClass = cInstance;
	current_pc = 0;
}

Types *Frame::GetField(char *key)
{
	std::string strkey(key);
	return (*thisClass->fieldVars)[strkey];
}

void Frame::PushOperandStack(Types *value)
{
	operandStack.push(value);
}

Code_attributes *Frame::GetCodeAttribute()
{
	return codeAttribute;
}

ClassFile *Frame::GetClassFile()
{
	return classFile;
}

bool Frame::HandleException(int index)
{
	if (current_pc >= (codeAttribute->exception_table + index)->start_pc &&
		current_pc < (codeAttribute->exception_table + index)->end_pc)
	{
		current_pc = (codeAttribute->exception_table + index)->handler_pc;
		return true;
	}
	return false;
}