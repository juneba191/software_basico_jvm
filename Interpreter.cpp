#include "Interpreter.h"
#include "ScreenPrinter.h"
#include "ClassFile.h"
#include "Debug.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

Interpreter* Interpreter::instance = NULL;

Interpreter::Interpreter(){
	Debug("Criou Interpreter");
}

Interpreter* Interpreter::GetInstance(){
	if (instance == NULL)
		instance = new Interpreter();
	return instance;
}


void Interpreter::Run(ClassFile* mainClass){
	ClassInstance* mainInstance = new ClassInstance();
	mainInstance->classDescription = mainClass;
	
	int mainIndex = 0;

	for (mainIndex = 0; mainIndex < (int)mainClass->methods_count; mainIndex++) {
		char* mname = (char*)mainClass->constant_pool[mainClass->methods[mainIndex].name_index - 1].info.utf8_info.bytes;
		if (!strcmp(mname, "main"))
			break;
	}
	if (mainIndex == (int)mainClass->methods_count) {
		std::cout << "Main not found!!" << std::endl;
		return;
	}

	int i;

	char* className = (char*)mainClass->constant_pool[mainClass->constant_pool[mainClass->this_class - 1].info.class_info.name_index - 1].info.utf8_info.bytes;
	for (i = strlen(className) - 1; i >= 0; i--) {
		if (className[i] == '/')
			break;
	}

	std::string keystring(className);
	keystring += ".class";
		
	staticInstances[keystring] = mainInstance;
	CreateFieldVars(mainInstance);

	Frame* mainFrame = new Frame(mainClass, mainClass->constant_pool, mainInstance, mainIndex);

	executionStack.push(mainFrame);
	
	while(!executionStack.empty()){
		executionStack.top()->Execute();
	}
	
}

void Interpreter::PushFrame(Frame* frame){
	executionStack.push(frame);
}

void Interpreter::PopFrame(){
	executionStack.pop();
}

void Interpreter::AddClass(ClassFile* cFile){
	if (cFile == NULL) {
		return;
	}; 
	int i = cFile->constant_pool[cFile->this_class - 1].info.class_info.name_index;
	char* aux;
	aux = (char*)malloc(cFile->constant_pool[i - 1].info.utf8_info.length * sizeof(char) * 2); // alterei as 3 linhas abaixo
	int j;
	for (j = cFile->constant_pool[i - 1].info.utf8_info.length - 1; j >= 0; j--) {
		if (cFile->constant_pool[i - 1].info.utf8_info.bytes[j] == '/')
			break;
	}
	
	strcpy(aux, (char*)cFile->constant_pool[i - 1].info.utf8_info.bytes + j + 1);
	
	std::string key(aux);
	key += ".class";
	
	//std::cout << key << std::endl;
	loadedClasses[key] = cFile;
	ClassInstance* staticInstance;
	//std::cout << key << std::endl;
	for (i = 0; i < cFile->field_count; i++) {
		if ((cFile->fields[i].access_flags & 0x8) != 0) {
			std::cout<<"teste";

			staticInstance = (ClassInstance*)malloc(sizeof(ClassInstance));
			staticInstance->classDescription = cFile;
			CreateFieldVars(staticInstance);
			staticInstances[key] = staticInstance;
			break;
		}
	}
}

ClassFile* Interpreter::GetClass(std::string key){

	int aux;

	for (aux = key.size() - 1; aux >= 0; aux--) {
		if (key[aux] == '/')
			break;
	}

	
	std::string subkey = key.substr(aux + 1);
	subkey += ".class";

	ClassFile* aux2 = loadedClasses[subkey];
	//If not loaded load class
	if(aux == 0){
		aux2 = new ClassFile(subkey);
		AddClass(aux2);
		//loadedClasses[subkey] = aux2;
	}
	//std::cout << subkey << std::endl;
	return aux2;
}

ClassFile* Interpreter::GetClass(const char* key){
	int i;

	for (i = strlen(key) - 1; i >= 0; i--) {
		if (key[i] == '/')
			break;
	}

	std::string keystring(key + i + 1);
	keystring += ".class";

	ClassFile* aux = loadedClasses[keystring];

	if(aux == NULL){
		aux = new ClassFile(keystring);
		AddClass(aux);
		//loadedClasses[keystring] = aux;
	}

	//std::cout << keystring << std::endl;
	return aux;
}

Types* Interpreter::GetStaticField(char* className, char* fieldName) {
	int i;

	for (i = strlen(className) - 1; i >= 0; i--) {
		if (className[i] == '/')
			break;
	}

	std::string keystring(className + i + 1);
	keystring += ".class";
	
	ClassInstance* aux = staticInstances[keystring];
	std::string field(fieldName);

	return (*aux->fieldVars)[field];
}

ClassInstance* Interpreter::GetStaticInstance(const char* className) {
	int i;

	for (i = strlen(className) - 1; i >= 0; i--) {
		if (className[i] == '/')
			break;
	}

	std::string keystring(className + i + 1);
	keystring += ".class";

	return staticInstances[keystring];
}

Frame* Interpreter::GetTopFrame() {
	return executionStack.top();
}

void Interpreter::PushException(char* exception) {
	bool handled = false;
	while (!executionStack.empty() || !handled) {
		executionStack.pop();
		if (executionStack.empty()) {
			std::cout << "Exception: " << exception<<std::endl;
			break;
		}
		for (int i = 0; i < executionStack.top()->GetCodeAttribute()->exception_table_length && !handled; i++) {
			char* catchType;
			if ((executionStack.top()->GetCodeAttribute()->exception_table[i].catch_type) != 0) {
				catchType = (char*)executionStack.top()->GetClassFile()->constant_pool[executionStack.top()->GetClassFile()->constant_pool[executionStack.top()->GetCodeAttribute()->exception_table[i].catch_type - 1].info.class_info.name_index - 1].info.utf8_info.bytes;
				if (!strcmp(exception, catchType)) {
					handled = (executionStack.top()->HandleException(i));
				}
			}
			else {
				catchType = NULL;
				handled = (executionStack.top()->HandleException(i));
			}
		}
	}
}
