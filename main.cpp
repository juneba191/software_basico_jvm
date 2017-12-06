#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ClassFile.h"
#include "ScreenPrinter.h"
#include <string.h>
#include "Debug.h"
#include "Frame.h"
#include "Interpreter.h"
#include <windows.h>
#include "FilePath.h"
#include "SoundManager.h"
#define header() printf("########################################################################################################################\n################################################ii,,GGGGGGEEDDGGGGLL##  ################################################\n########################################;;DDDDDDDDGGGGGGLLLLLLLLGGGGDDDDDDffLL##########################################\n####################################LLffDDDDLLjjii;;::::........::,,;;ttffGGDDGGttff####################################\n##############################  DDLLDDGGLLtt..      ;;tttttttt,,  ..::..  ;;jjLLDDDDffLL################################\n############################GGLLDDGGff;;..  tt..    jjffLLDDDDii  ;;GGGGff,,  ..iiLLDDGGttLL############################\n########################tt##GGDDff;;....    DD;;      ..jjDDff::  ttDDjjtttt    ....iiLLDDGGEE  ########################\n######################GGLLDDGGii..  jjff..  DDff    ..jjDDff..    ffDDLLtt..  ,,LL;;  ..ttGGDD  ff######################\n################jjDDGGGGtt..::;;..  ;;DDff  ttDD,,..GGDDGGGGGGtt,,DDGGii,,  ,,DDLL....ffjj..::ffDDLLGG##################\n################DDGGGGii  ;;GGDDtt  ..LLDDiiGGff..  ::,,,,,,::....iijjLLGG..ffDD;;  ttDDGGLL::..jjDDLLGG################\n################GGGGii  iiLLjjGGff,,  ;;GGGGii..  ::;;iittttttttii,,::..::,,DDLL..;;DDGG;;jjGG,,..jjDDLL################\n##############GGGGii  ttDDtt,,GGDDDDtt  ..::,,ttLLGGDDDDDDDDDDDDDDDDGGfftt..ii,,,,GGDDLLffjjDDff  ..jjDDLL##############\n############LLjjtt    iiDDGGGGtt;;GGff..  ;;DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDjj,,  ;;GGii..ffDDLL;;    ..ffWWtt############\n##########jjWWDD..      ttDDGG::  ::..,,ffDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDGGjj..::    ffGG,,        ::GGDD############\n##########GGGGLL..;;,,    jjDDff..  iiGGDDDDDDDDDDDDDDDDDDGGDDDDDDDDDDDDDDDDDDDDLL,,  ;;DDff  ..iijj;;  GGLLLL##########\n########ffjjDDiiffDDii  ....jjii..jjDDDDDDDDDDDDDDDDDDDDGGiiGGDDDDDDDDDDDDDDDDDDDDGGii..tt,,..ffDDDDDDttjjDDWWii########\n########DDDDff..LLii  ,,LL::  ..jjDDDDDDDDDDDDDDDDDDGGffjj..jjffGGDDDDDDDDDDDDDDDDDDDDii    iiDDff,,;;GG::GGGGLL########\n######jjffGG,,..;;    iiDD,,  jjDDDDDDDDDDDDDDDDDDDDLL;;      ;;GGDDDDDDDDDDDDDDDDDDDDDD;;  ttGG::    tt  ttDD##;;######\n####;;DDGGff  ,,ii    iiGG..iiDDDDDDDDDDDDDDDDDDDDDDDDGG::  ::GGDDDDDDDDDDDDDDDDDDDDDDDDGG,,,,GG::  ..ff..::LLGGLL######\n####tt,,GG;;  ::DDttiiLLii,,GGDDDDDDDDDDDDDDDDDDDDDDDDLL,,tt,,LLDDDDDDDDDDDDDDDDDDDDDDDDDDLL..jjffiiffDD    ttDDEE..####\n####GGGGLL..    GGDDDDjj..ffDDDDDDDDDDDDDDDDDDDDDDDDDDLLGGDDGGLLDDDDDDDDDDDDDDDDDDDDDDDDDDDDtt..ffDDDDLL    ,,GGffff####\n##;;EEDDtt      ::ii,,  ;;DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDGG::..;;;;..      ffGGGG  ##\n##LLGGLL..            ,,DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDLL..            ,,GGtttt##\n,,DDDDjj              ttDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD;;            ..LLGGLL##\nttEEDDii              LLDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDLLDDDDDDDDDDDDjj              ffDDGG##\njj##GG;;  ....      ::GGDDDDDDDDDDDDLLLLDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDLL;;DDDDDDDDDDDDLL          ::..ttDDDDjj\nffLLGG,,iiGGLLLLjj..;;DDDDDDDDDDGGDDiiiiDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDLLjj;;..ttjjGGDDDDDDGG..,,tttt::LL,,iiDDEEjj\njjLLGG::jjDDGGDDGG::iiDDDDDDDDGGtt,,....,,ttDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDGG;;    ..ttGGDDDDDDDD::;;LLLL::LL;;;;DDWWjj\nffGGLL::ffttiiLLGG..ttDDDDDDDDDDGG;;    iiGGDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDLL..  ,,DDDDDDDDDDDD,,,,LLLL,,LLii,,GG##LL\nDDEELL::ffiiiiffLL..ttDDDDDDDDDDDDii::::ttDDDDDDDDDDDDDDDDDDDDDDDDGGDDDDDDDDDDffiiff;;LLDDDDDDDDDD;;::GGDDLLDDtt,,DD;;DD\nGGDDLL::ff;;iijjLL  ttDDDDDDDDDDGGjjGGLLjjDDDDDDDDDDDDDDDDDDDDDDGGttffGGDDDDDDGGDDDDGGGGDDDDDDDDDD;;::GGDDGGGGii,,DDEEGG\nffLLLL::::..  ..;;  ttDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDtt..::LLDDDDDDDDDDDDDDDDDDDDDDDDDD,,  ;;::::..  ;;GG##ff\njjLLGG::  ::..;;ii  iiDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDff,,iiDDDDDDDDDDDDDDDDDDDDDDDDDDGG::..jj..;;::  ;;DDKKjj\nffiiGG,,::GGiiLLGG,,,,DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDGGGGGGDDDDDDDDDDDDDDDDDDDDDDDDDDGG..iiDDLLDDLL..iiDDEEtt\nttWWGG;;;;GGiiGGjjtt..GGDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDff  ffjjLLffGG::jjDDDDtt\nttDDDDtt,,LL::GGttff  ffDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDtt..LL;;ii;;LL..ffGGGG##\n##GGDDff::LLffDDffff  iiDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD,,::DDGGffffjj::LLGGff##\n##ii  GG;;  ii;;..ttii  jjDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD;;..jj;;..,,ii::ttDDEEii##\n##iiDDDDjj      LLDDGG::::GGDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDLL..,,GGDDff    ..LLGGLL####\n####LLGGLL::::ffDDffffjj  ttDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD;;    ,,ffDD,,  ;;GG;;jj####\n####ii##DDii;;DDLLtt,,GG,,..LLDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDLLDDDDDDDDDDDDDDDDDDDDDDDDDDjj          iitt  ffGGDD;;####\n####  GGGGLL::,,;;GGLLDDii  ,,GGDDDDDDDDDDDDDDDDDDDDDDDDDDDDLLiiDDDDDDDDDDDDDDDDDDDDDDDDLL..::ff;;    ..;;,,GGLLff######\n######jj  DDii    ttGGff::    ;;GGDDDDDDDDDDDDDDDDDDDDDDLLjjii..ttjjGGDDDDDDDDDDDDDDDDLL::  iiDDDDff;;tt::jjDDEE  ######\n########GGGGLL::    ..,,ttjj;;  ;;GGDDDDDDDDDDDDDDDDDDDDLL;;    ..ttGGDDDDDDDDDDDDDDLL::      ,,LLDDDDDD;;GGffff########\n########jj##DDjj    ::LLDDGGGGjj..;;GGDDDDDDDDDDDDDDDDDDDDLL..  ,,DDDDDDDDDDDDDDDDLL::          ..;;tt,,LLDDDD##########\n##########LLLLGG    ttDDjj::::LLtt  ::ffDDDDDDDDDDDDDDDDDDjjiijj;;GGDDDDDDDDDDDDjj....ffii              DD  jj##########\n############DDDD::  jjGG::    ttGG::    iiGGDDDDDDDDDDDDDDGGDDDDGGGGDDDDDDDDLL,,    ,,DDDDii    iiii  ;;GGLL############\n############ttWWff..;;GG,,  ..LLDD::,,ii....LLDDDDDDDDDDDDDDDDDDDDDDDDDDGG;;..::iitt::;;DDDDiittGG;;,,GGDD  ############\n##############LLDDff..ttLLttLLDDff..ffDDLLii..,,ttffGGGGDDDDDDDDGGLLffii::  iiGGGGGGii  ;;DDDDGG,,::LLDDjj##############\n################LLDDjj::iiGGGGff..;;DDLLiiLL::........::,,,,,,::::..      iiDD;;..::  ..  iiLL,,::LLDD##################\n################LLffDDff....::    ffDDGGiiffttjjGGfftt;;::..::,,iittii    LLGG..    iiff..  ..,,LLDD  ff################\n##################GGGGDDLL,,    ;;DDLLiiGGDD::::;;GGDDGGGGjjGGGGffffii    ffDD;;  ::GGLL    ;;GGDD##ff##################\n######################ttWWGGDDff,,..  ..LLDD    ,,DDff    ..GGDDLLLLtt      iiLLffii::;;LLDDGGGGLL######################\n########################  GGLLDDGGjj::  iijj    iiDDtt    ..LLGG,,::,,..        ..,,ffGGDDttLL##########################\n############################jjEEGGDDGGjj,,..    iiGG;;      ffDDLLGGGG;;    ..;;ffGGDDGGGG##############################\n################################tt##GGDDGGff,,::..::        ;;ii;;,,::..::jjLLDDDDLLEE::################################\n####################################..EELLDDGGLLffjjttii;;;;;;;;iittjjffGGDDDDffLL######################################\n############################################ffGGDDDDDDGGDDDDDDGGDDDDDDDDGGLL############################################\n########################################################################################################################\n########################################################################################################################\n");
#include <thread>         // std::thread
 
using namespace std;


SoundManager sm(100);
    

void foo() 
{
	sm.ExecuteMusic(1);
}




void menu(int argc, char *argv[], thread *first){
	while(1){
	
	    string nome;
		string modo;
		
		if (argc < 3){
			std::cout << "test.class -(imprimir/interpretar): ";
			cin >> nome;
			cin >> modo;
		} else {
			
			nome = argv[1];
			modo = argv[2];
		}
	    if(modo == "-imprimir"){
	    	size_t found = nome.find_last_of("/\\");
	   		FilePath::GetInstance()->path = nome.substr(0, found+1);
	    	nome = nome.substr(found+1);
	    	ClassFile *classFile = new ClassFile(nome);
	   		Screen_Print_Javap_Format(*classFile);
		}else if(modo == "-interpretar"){
			size_t found = nome.find_last_of("/\\");
	   		FilePath::GetInstance()->path = nome.substr(0, found+1);
	    	nome = nome.substr(found+1);
	    	ClassFile *classFile = new ClassFile(nome);       
	    	Interpreter::GetInstance()->AddClass(classFile);
	    	Frame::SetUp();
	    	Interpreter::GetInstance()->Run(classFile);
		}else if(modo == "-cruzeiro"){
			sm.mute = 1;
			header();
			PlaySound(TEXT("song.wav"), NULL, SND_FILENAME | SND_ASYNC);
			
			system("COLOR F1");
		}
		argc = 0;
	}
}

int main(int argc, char *argv[])
{
	header();
		
	thread first (foo); 
	std::thread second (menu,argc,argv, &first);

	first.join(); 
	second.join();
    return 0;
}
