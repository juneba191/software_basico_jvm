#ifndef SCREENPRINTER_H_INCLUDED
#define SCREENPRINTER_H_INCLUDED
#include "ClassFile.h"


void Screen_Print_Javap_Format(ClassFile &classFile);
void Screen_Print_Access_Flag_Class(u2);
void Screen_Print_Access_Flag_Method(u2 flag);
void Screen_Print_Constant_Type(ClassFile, u2);
void Screen_Print_Utf8_Ref(ClassFile, u2);
void Screen_Initialize_Print_Functions();

#endif // SCREENPRINTER_H_INCLUDED
