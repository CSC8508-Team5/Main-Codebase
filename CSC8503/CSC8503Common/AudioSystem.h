#pragma once

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/Camera.h"
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

			static void PlayAudio(string filename);
			static void PlaySFX(string filename);
			static void PlayAudio(Vector3 position, string filename);
			static void PlaySFX(Vector3 position, string filename);
		private:
			static ISoundEngine* engine;
		};
	}
}
