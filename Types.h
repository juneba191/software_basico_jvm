#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <cstdint>
#include <string>
#include "ClassFile.h"
#include <unordered_map>

typedef struct primitiveTypes primitiveTypes;
typedef struct ArrayType ArrayTypes;
typedef struct ClassInstance ClassInstance;


void CreateFieldVars(ClassInstance* cinstance);

class Types{
public:
	enum TAGTYPE {UNDEF, BYTE, BOOL, CHAR, SHORT, INT, FLOAT, LONG, DOUBLE, CLASSINSTANCE, ARRAY, STRING, RETURNADDRESS};
	TAGTYPE tag;
	union{
		primitiveTypes* basicRef;
		ArrayType* arrayRef;
		ClassInstance* classInstance;
		std::string* stringRef;
		u4* returnAddress;
	};
	Types(char*);
	Types();
};

struct primitiveTypes{
	union{
		/*Return Address não ficará nos primitivetypes pq ele não corresponde a nenhum tipo da JVM (vide doc)*/
		u4 *T_BOOLEAN;
        u4 *T_BYTE;
        u4 *T_CHAR;
        u4 *T_SHORT;
        u4 *T_INT;
        u4 *T_FLOAT;
        u8 *T_LONG;
        u8 *T_DOUBLE;
	};
};

struct ArrayType{
	std::vector<Types*>* array;
};

struct ClassInstance{
	ClassFile* classDescription;
	std::unordered_map<std::string, Types*>* fieldVars;
};

Types* copyTypes(Types* type);
#endif
