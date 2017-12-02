#ifndef FRAME_H_
#define FRAME_H_

#include "Types.h"
#include <stack>
#include <vector>
#include "ClassFile.h"

class Frame{
private:
	u2 method_index;
	u2 code_index;
	u4 current_pc;
	ClassFile* classFile;
	ClassInstance* thisClass;
	std::stack<Types*> operandStack;
	std::vector<Types*> locals;
	Code_attributes *codeAttribute;
	Exceptions_attribute *exceptionsAttribute;
	
public:
	static void (**interpreter_operations)(u1* code, u4* pc);
	Frame(ClassFile* cFile, Cp_Info* constantPool, u2 methodIndex, ClassInstance* cInstance, std::vector<Types*>* args);
	Frame(ClassFile* cFile, Cp_Info* constantPool, ClassInstance* cInstance, u2 methodIndex);
	~Frame();
	void Execute();
	Types* GetField(char* key);
	static void SetUp();
	void PushOperandStack(Types* value);
	Code_attributes* GetCodeAttribute();
	ClassFile* GetClassFile();
	bool HandleException(int index);
};
#endif
