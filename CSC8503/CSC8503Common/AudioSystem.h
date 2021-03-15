/*
Audio System driven by irrKlang

use functions here to play sound globally, also parameters set here will affect all sound.
If you need object to play sound please check SoundSource class.

the work flow should be:
1.Initialize
	within construct function
2.Play sound
	please note if the second parameter of Play() function is an Vector3, the sound will be played in 3D, otherwise in 2D
3.Release sound
	if they are not needed any more use RemoveSource()
4.Unintialize
	within deconstruct function

Please not:
	functions with 'SFX', 'Audio'symbol will be located in SFX and Audio asset foloder.

	min and max distance are not as you imagined
	for max distance, changing this value is usually not necessary. Use setMinDistance() instead. Don't change this value if you don't know what you are doing: This value causes the sound to stop attenuating after it reaches the max distance. Most people think that this sets the volume of the sound to 0 after this distance, but this is not true. Only change the minimal distance (using for example setMinDistance()) to influence this. The maximum distance for a sound source is the distance beyond which the sound does not get any quieter. The default minimum distance is 1, the default max distance is a huge number like 1000000000.0f.
	
*/
#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/Camera.h"
#include "../CSC8503Common/Transform.h"
#include "../../Common/Assets.h"

#include <irrKlang.h>
#include <string>

#pragma comment(lib,"irrKlang.lib")

using namespace irrklang;
using namespace NCL::Maths;
using namespace std;
namespace NCL {
	namespace CSC8503 {
		class AudioSystem
		{
		public:
			AudioSystem();
			~AudioSystem();

			void Update(Vector3 listener, Vector3 forward);
			void Update(vec3df listener, vec3df forward);
			void Update(Camera camera);
			void Update(Transform trans);

			static ISound* Play(ISoundSource* sound, bool loop = false, bool trackable = true);
			static ISound* Play(ISoundSource* sound, Vector3 position, bool loop = false, bool trackable = true);

			static ISound* Play(string filename, bool loop = false, bool trackable = false);
			static ISound* Play(string filename, Vector3 position, bool loop = false, bool trackable = false);

			static ISound* PlayAudio(string filename, bool loop = false, bool trackable = false);
			static ISound* PlaySFX(string filename, bool loop = false, bool trackable = false);
			static ISound* PlayAudio(string filename, Vector3 position, bool loop = false, bool trackable = false);
			static ISound* PlaySFX(string filename, Vector3 position, bool loop = false, bool trackable = false);

			static ISoundSource* AddSource(string filename);
			static ISoundSource* AddSourceAudio(string filename);
			static ISoundSource* AddSourceSFX(string filename);

			static void RemoveSource(ISoundSource* source);
			static void RemoveSource(string filename);
			static void RemoveSourceAudio(string filename);
			static void RemoveSourceSFX(string filename);

			//global settings
			static void SetGlobalVolume(float vol) { engine->setSoundVolume(vol); }
			static float GetGlobalVolume() { return engine->getSoundVolume(); }

			static void SetDefaultMinDistance(float d) { engine->setDefault3DSoundMinDistance(d); }
			static void SetDefaultMaxDistance(float d) { engine->setDefault3DSoundMaxDistance(d); }

			static float GetDefaultMinDistance() { engine->getDefault3DSoundMinDistance(); }
			static float GetDefaultMaxDistance() { engine->getDefault3DSoundMaxDistance(); }

			static void PauseAll(bool pause = true) { engine->setAllSoundsPaused(pause); }
			static string GetDevice() { return engine->getDriverName(); }
			static int GetLoadedSourceCount() { return engine->getSoundSourceCount(); }

			static ISoundSource* GetSourceExist(string filename) { return engine->getSoundSource(filename.c_str(), false); }

		private:
			static ISoundEngine* engine;
		};
	}
}
