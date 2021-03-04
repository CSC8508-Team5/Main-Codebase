#pragma once

#include "AudioSystem.h"

namespace NCL {
	namespace CSC8503 {
		class SoundSource
		{
		public:
			SoundSource(string filename, Vector3 position)
			{
				InitSound(filename);
				Update(position,Vector3::Zero());
			}
			SoundSource()
			{
				source = 0;
				sound = 0;
				fxControl = 0;
			}
			~SoundSource()
			{
				if (!sound)
					return;
				sound->drop();
				delete sound;

				if (!fxControl)
					return;
				delete fxControl;

				if (!source)
					return;
				source->drop();
				delete source;
			}

			void InitSound(string filename)
			{
				if (source)
					source->drop();
				source = AudioSystem::AddSource(filename);
			}

			ISoundEffectControl* GetSoundFxControl() { return fxControl; }

			ik_f32 GetMinDistance() { return sound->getMinDistance(); }
			ik_f32 GetMaxDistance() { return sound->getMaxDistance(); }
			ik_f32 GetVolume() { return volume; }
			bool GetIsLoop() { return isLoop; }

			void SetMinDistance(float d)
			{
				if (d != minDistance && sound)
				{
					sound->setMinDistance(d);
					minDistance = d;
				}
			}
			void SetMaxDistance(float d)
			{
				if (d != maxDistance && sound)
				{
					sound->setMaxDistance(d);
					maxDistance = d;
				}
			}
			void SetVolume(float vol)
			{
				if (vol != volume && sound)
				{
					sound->setVolume(vol);
					volume = vol;
				}
			}
			void SetIsLoop(bool loop)
			{
				if (loop != isLoop && sound)
				{
					sound->setIsLooped(loop);
					isLoop = loop;
				}
			}

			ik_u32 GetPlayPos()
			{
				if (sound)
					return sound->getPlayPosition();
			}

			void SetPlayPos(ik_u32 t)
			{
				if (sound)
					sound->setPlayPosition(t);
			}

			ik_f32 GetPlaySpeed()
			{
				if (sound)
					return sound->getPlaybackSpeed();
			}

			void SetPlaySpeed(ik_f32 s)
			{
				if (sound)
					sound->setPlaybackSpeed(s);
			}

			//Update ISound 3D position
			void Update(Vector3 pos, Vector3 speed)
			{
				if (position != pos)
					position = pos;
				if (velocity != speed)
					velocity = speed;
				if (sound)
				{
					sound->setVelocity(speed);
					sound->setPosition(vec3df(pos.x, pos.y, pos.z));
				}
			}

			//Update ISound* parameters including min/max distance, volume, isloop
			void UpdateParams()
			{
				if (source && sound)
				{
					sound->setMinDistance(minDistance);
					sound->setMaxDistance(maxDistance);
					sound->setVolume(volume);
					sound->setIsLooped(isLoop);
					sound->setPosition(position);
					sound->setVelocity(velocity);
				}
			}

			//Play sound
			void Play()
			{
				if (sound && sound->getIsPaused())
				{
					sound->setIsPaused(false);
				}
				else if (!sound || sound->isFinished())
				{
					sound = AudioSystem::Play(source, position, isLoop);
					fxControl = sound->getSoundEffectControl();
					if (!fxControl)
						std::cout << "This device or sound does not support sound effects.\n";
				}
				else
				{
					sound->setPlayPosition(0);
				}
				UpdateParams();
			}

			void Play(bool loop)
			{
				SetIsLoop(loop);
				Play();
			}

			void Pause()
			{
				if (sound)
					sound->setIsPaused(true);
			}
			void Unpause()
			{
				if (sound)
					sound->setIsPaused(false);
			}
			bool GetIsPaused()
			{
				if (sound)
					return sound->getIsPaused();
			}

			void Stop()
			{
				if (sound)
					sound->stop();
			}

			void Reset()
			{
				if (sound)
					SetPlayPos(0);
			}

			ISoundSource* source;
		private:

			ISound* sound;
			ISoundEffectControl* fxControl;
			Vector3 position;
			Vector3 velocity;
			float minDistance = 0.0f;
			float maxDistance = 10000.0f;
			float volume = 1.0f;
			bool isLoop = false;
		};
	}
}


