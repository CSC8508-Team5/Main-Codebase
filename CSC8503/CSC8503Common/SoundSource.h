#pragma once

#include "AudioSystem.h"

namespace NCL {
	namespace CSC8503 {
		class SoundSource
		{
		public:
			SoundSource(string filename, bool loop = false, bool paused = false)
			{
				InitSound(filename, loop, paused);
			}
			SoundSource()
			{
				sound = 0;
				fxControl = 0;
			}
			~SoundSource()
			{
				if (!sound)
					return;
				sound->drop();
				delete sound;
			}

			void InitSound(string filename, bool loop, bool pausedOnInit = false)
			{
				if (sound)
				{
					sound->drop();
				}
				sound = AudioSystem::AddSound(filename, Vector3::Zero(), loop, pausedOnInit);

				this->soundFile = filename;
				this->isLoop = loop;

				UpdateParams();

				fxControl = sound->getSoundEffectControl();
				if (!fxControl)
					std::cout << "This device or sound does not support sound effects.\n";
			}

			ISoundEffectControl* GetSoundFxControl() { return fxControl; }

			float GetMinDistance() { return minDistance; }
			float GetMaxDistance() { return maxDistance; }
			float GetVolume() { return volume; }
			bool GetIsLoop() { return isLoop; }

			void SetMinDistance(float d) { if (d != minDistance)	sound->setMinDistance(d); }
			void SetMaxDistance(float d) { if (d != maxDistance)	sound->setMaxDistance(d); }
			void SetVolume(float vol) { if (vol != volume) sound->setVolume(vol); }
			void SetIsLoop(bool loop) { if (loop != isLoop) sound->setIsLooped(loop); }

			float GetPlayPos()
			{
				if (!sound)
					return;
				return sound->getPlayPosition();
			}
			void SetPlayPos(float t)
			{
				if (!sound)
					return;
				sound->setPlayPosition(t);
			}

			float GetPlaySpeed()
			{
				if (!sound)
					return;
				return sound->getPlaybackSpeed();
			}
			void SetPlaySpeed(float s)
			{
				if (!sound)
					return;
				sound->setPlaybackSpeed(1.0f);
			}

			void Update(Vector3 position)
			{
				if (!sound)
					return;
				sound->setPosition(position);
			}

			void UpdateParams()
			{
				if (!sound)
					return;
				sound->setMinDistance(minDistance);
				sound->setMaxDistance(maxDistance);
				sound->setVolume(volume);
				sound->setIsLooped(isLoop);
			}

			void Play()
			{
				if (!sound)
					return;
				sound->setPlayPosition(0);
				//if (sound->isFinished())
				{
					//InitSound(soundFile,isLoop);
					AudioSystem::Play(sound->getSoundSource(), Vector3(sound->getPosition()), isLoop);
					UpdateParams();
				}
				/*if (sound->getIsPaused())
				{
					sound->setIsPaused(false);
				}*/
			}

			void Pause()
			{
				if (!sound)
					return;
				sound->setIsPaused(true);
			}
			void Unpause()
			{
				if (!sound)
					return;
				sound->setIsPaused(false);
			}
			bool GetIsPaused()
			{
				return sound->getIsPaused();
			}

			void Reset()
			{
				if (!sound)
					return;
				InitSound(soundFile, isLoop);
			}



			void Stop()
			{
				if (!sound)
					return;
				sound->stop();
			}

		private:
			ISound* sound;
			ISoundEffectControl* fxControl;
			string soundFile;
			float minDistance = 4.0f;
			float maxDistance = 10.0f;
			float volume = 1.0f;
			bool isLoop = false;
		};
	}
}


