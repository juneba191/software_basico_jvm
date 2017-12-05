#include "SoundManager.h"


SoundManager::SoundManager(int bpm):BPM(bpm){
	
	notes["C"] = 261.63;
	notes["C#"] = 277.18;
	notes["D"] = 293.66;
	notes["D#"] = 311.13;
	notes["E"] = 329.63;
	notes["F"] = 349.23;
	notes["F#"] = 369.99;
	notes["G"] = 392.00;
	notes["G#"] = 415.30;
	notes["A"] = 440.00;
	notes["A#"] = 466.16;
	notes["B"] = 493.88;
}

float SoundManager::TempoUnidade(){
	return 1000*60.0/float(BPM);
}

void SoundManager::ExecuteNote(string note, float tempo, float oitava){
	
	//printf("%s%d ==> %.2f\n", note.data() , 2 + (int)oitava, tempo*TempoUnidade()/1000);
	Beep(notes[note]*oitava, tempo*TempoUnidade());
}


void SoundManager::Pause(float tempo){
	Sleep(tempo*TempoUnidade());
}

//B ==> A#
//E ==> D#
//A ==> G#
//D ==> C#


void SoundManager::ExecuteMusic(int loop){
	while(loop){
	
	ExecuteNote("F", 1.5, 1);
	ExecuteNote("G", 0.25, 1);
	ExecuteNote("G#", 0.25, 1);
	ExecuteNote("G", 1.5, 1);
	ExecuteNote("G#", 0.25, 1);
	ExecuteNote("A#", 0.25, 1);
	ExecuteNote("G#", 1.5, 1);
	ExecuteNote("G", 0.25, 1);
	ExecuteNote("F", 0.25, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("D#", 0.5, 1);
	ExecuteNote("C", 0.5, 1);
	ExecuteNote("C", 0.5, 1);
	
	//DO S
	
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	
	
	//BEQUADRO NO E
	ExecuteNote("E", 0.5, 1);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("E", 0.5, 1);
	
	ExecuteNote("G", 2, 1);
	
	ExecuteNote("F", 1, 1);
	Pause(0.5);
	ExecuteNote("F", 0.5, 1);
	
	ExecuteNote("F#", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	
	//BEQ A
	ExecuteNote("A", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	
	ExecuteNote("A#", 3, 1);
	Pause(0.5);
	ExecuteNote("C#", 0.5, 2);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	
	ExecuteNote("G#", 1, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("G", 1, 1);
	ExecuteNote("F", 1, 1);
	ExecuteNote("C", 0.5, 1);
	
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("A#", 0.5, 1);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	
	ExecuteNote("D", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("C", 0.5, 2);
	
	ExecuteNote("B", 2, 1);
	
	Pause(1);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("C#", 0.5, 1);
	
	ExecuteNote("D#", 0.666, 2);
	ExecuteNote("F", 0.666, 2);
	ExecuteNote("D#", 0.666, 2);
	
	ExecuteNote("C#", 0.666, 2);
	ExecuteNote("C", 0.666, 2);
	ExecuteNote("A#", 0.666, 1);
	
	ExecuteNote("F", 2, 1);
	
	Pause(1);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("C", 0.5, 2);
	
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	
	ExecuteNote("D#", 2.5, 1);
	Pause(0.5);
	
	ExecuteNote("F#", 0.666, 1);
	ExecuteNote("F", 0.666, 1);
	ExecuteNote("E", 0.666, 1);
	
	ExecuteNote("F", 0.666, 1);
	ExecuteNote("A", 0.666, 1);
	ExecuteNote("C", 0.666, 2);
	
	ExecuteNote("D#", 1, 1);
	ExecuteNote("C#", 1, 1);
	
	Pause(0.5);
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 1, 1);
	ExecuteNote("G#", 0.5, 1);
	
	ExecuteNote("G", 1, 1);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 1, 1);
	ExecuteNote("G#", 2, 1);
	
	ExecuteNote("C", 0.25, 2);
	ExecuteNote("C#", 0.25, 2);
	ExecuteNote("C", 0.25, 2);
	ExecuteNote("A#", 0.25, 1);
	ExecuteNote("G#", 0.25, 1);
	ExecuteNote("G", 0.25, 1);
	
	ExecuteNote("F", 1.5, 1);
	ExecuteNote("G", 0.25, 1);
	ExecuteNote("G#", 0.25, 1);
	
	ExecuteNote("G", 1.5, 1);
	ExecuteNote("G#", 0.25, 1);
	ExecuteNote("A#", 0.25, 1);
	
	ExecuteNote("G#", 1.5, 1);
	ExecuteNote("G", 0.25, 1);
	ExecuteNote("F", 0.25, 1);
	
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("D#", 0.5, 1);
	ExecuteNote("C", 0.5, 1);
	ExecuteNote("C", 0.5, 1);
	
	// S AO FINE
	
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	
	
	//BEQUADRO NO E
	ExecuteNote("E", 0.5, 1);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("E", 0.5, 1);
	
	ExecuteNote("G", 2, 1);
	
	ExecuteNote("F", 1, 1);
	Pause(0.5);
	ExecuteNote("F", 0.5, 1);
	
	ExecuteNote("F#", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	
	//BEQ A
	ExecuteNote("A", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	
	ExecuteNote("A#", 3, 1);
	Pause(0.5);
	ExecuteNote("C#", 0.5, 2);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	
	ExecuteNote("G#", 1, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("G", 1, 1);
	ExecuteNote("F", 1, 1);
	ExecuteNote("C", 0.5, 1);
	
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("A#", 0.5, 1);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	
	ExecuteNote("D", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("C", 0.5, 2);
	
	ExecuteNote("B", 2, 1);
	
	Pause(1);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("C#", 0.5, 1);
	
	ExecuteNote("D#", 0.666, 2);
	ExecuteNote("F", 0.666, 2);
	ExecuteNote("D#", 0.666, 2);
	
	ExecuteNote("C#", 0.666, 2);
	ExecuteNote("C", 0.666, 2);
	ExecuteNote("A#", 0.666, 1);
	
	ExecuteNote("F", 2, 1);
	
	Pause(1);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("C", 0.5, 2);
	
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	
	ExecuteNote("A#", 0.5, 1);
	ExecuteNote("G#", 0.5, 1);
	ExecuteNote("G", 0.5, 1);
	ExecuteNote("F", 0.5, 1);
	
	ExecuteNote("D#", 2.5, 1);
	Pause(0.5);
	
	ExecuteNote("F#", 0.666, 1);
	ExecuteNote("F", 0.666, 1);
	ExecuteNote("E", 0.666, 1);
	
	ExecuteNote("F", 0.666, 1);
	ExecuteNote("A", 0.666, 1);
	ExecuteNote("C", 0.666, 2);
	
	ExecuteNote("D#", 1, 1);
	ExecuteNote("C#", 1, 1);
	
	Pause(0.5);
	ExecuteNote("C#", 0.5, 2);
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 0.5, 1);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 1, 1);
	ExecuteNote("G#", 0.5, 1);
	
	ExecuteNote("G", 1, 1);
	
	ExecuteNote("C", 0.5, 2);
	ExecuteNote("A#", 1, 1);
	ExecuteNote("G#", 2, 1);
}
}

