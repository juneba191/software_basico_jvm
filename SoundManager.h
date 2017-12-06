/*
 * Sound.cpp
 *
 *  Created on: 23/05/2015
 *      Author: Isaac
 */

#ifndef SOUND_H_
#define SOUND_H_

#include <unordered_map>
#include <windows.h>

using namespace std;

class SoundManager {
	private:
		unordered_map<string, float> notes;
		int BPM;
		float TempoUnidade();
		void ExecuteNote(string note, float tempo, float oitava);
		void Pause(float tempo);
	public:
		int mute;
		SoundManager(int BPM);
		void ExecuteMusic(int loop);
};

#endif /* SOUND_H_ */
