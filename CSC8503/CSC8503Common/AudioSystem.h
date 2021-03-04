#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/Camera.h"
#include "../CSC8503Common/Transform.h"
#include "../../Common/Assets.h"

#include <irrKlang.h>
#include <string>
#include <vector>

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

			static ISound* Play(ISoundSource* sound, bool loop = false);
			static ISound* Play(ISoundSource* sound, Vector3 position, bool loop = false);
			static ISound* Play(string filename, bool loop = false);
			static ISound* Play(string filename, Vector3 position, bool loop = false);


			static ISound* PlayAudio(string filename, bool loop = false);
			static ISound* PlaySFX(string filename, bool loop = false);
			static ISound* PlayAudio(string filename, Vector3 position, bool loop = false);
			static ISound* PlaySFX(string filename, Vector3 position, bool loop = false);
			
			static ISoundSource* AddSource(string filename);
			static ISoundSource* AddSourceAudio(string filename);
			static ISoundSource* AddSourceSFX(string filename);

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
			static vector<ISoundSource*> sources;
		};
	}
}
