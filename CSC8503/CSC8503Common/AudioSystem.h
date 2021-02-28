#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/Camera.h"
#include "GameObject.h"
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
			void Update(GameObject obj);

			static void Play(ISoundSource* sound, bool loop = false);
			static void Play(ISoundSource* sound, Vector3 position, bool loop = false);

			static void PlayAudio(string filename, bool loop = false);
			static void PlaySFX(string filename, bool loop = false);
			static void PlayAudio(string filename, Vector3 position, bool loop = false);
			static void PlaySFX(string filename, Vector3 position, bool loop = false);
		private:
			static ISoundEngine* engine;

			struct SoundSource
			{

			};
		};
	}
}
