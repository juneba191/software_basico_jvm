#include <stack>
#include <unordered_map>
#include "Frame.h"
#include "ClassFile.h"
#include <string>

class Interpreter{
	private:
		static Interpreter* instance;
		Interpreter();
		std::stack<Frame*> executionStack;
		std::unordered_map<std::string, ClassFile*> loadedClasses;
		std::unordered_map<std::string, ClassInstance*> staticInstances;

	public:
		static Interpreter* GetInstance();
		void Run(ClassFile* mainClass);
		void PushFrame(Frame*);
		void PopFrame();
		void AddClass(ClassFile*);
		ClassFile* GetClass(std::string key);
		ClassFile* GetClass(const char* key);
		Types* GetStaticField(char* className, char* fieldName);
		Frame* GetTopFrame();
		void PushException(char* exception);
		ClassInstance* GetStaticInstance(const char* className);
};
