#include "AudioSystem.h"

ISoundEngine* NCL::CSC8503::AudioSystem::engine;

NCL::CSC8503::AudioSystem::AudioSystem()
{
	engine = createIrrKlangDevice();
	engine->loadPlugins("./ikpMP3.dll");
	engine->loadPlugins("./ikpFlac.dll");
}

NCL::CSC8503::AudioSystem::~AudioSystem()
{
	engine->drop();
}

void NCL::CSC8503::AudioSystem::Update(Vector3 listener, Vector3 forward)
{
	engine->setListenerPosition(listener, forward);
}

void NCL::CSC8503::AudioSystem::Update(vec3df listener, vec3df forward)
{
	engine->setListenerPosition(listener, forward);
}

void NCL::CSC8503::AudioSystem::Update(Camera camera)
{
	engine->setListenerPosition(camera.GetPosition(), Vector3(camera.BuildViewMatrix().GetColumn(2)));
}

void NCL::CSC8503::AudioSystem::Update(GameObject obj)
{
	Transform objTrans = obj.GetTransform();
	engine->setListenerPosition(objTrans.GetPosition(), objTrans.Forward());
}

void NCL::CSC8503::AudioSystem::Play(ISoundSource* sound, bool loop)
{
	engine->play2D(sound, loop);
}

void NCL::CSC8503::AudioSystem::Play(ISoundSource* sound, Vector3 position, bool loop)
{
	engine->play3D(sound, position, loop);
}

void NCL::CSC8503::AudioSystem::PlayAudio(string filename, bool loop)
{
	engine->play2D((Assets::AUDIODIR + filename).c_str(), loop);
}

void NCL::CSC8503::AudioSystem::PlaySFX(string filename, bool loop)
{
	engine->play2D((Assets::SFXDIR + filename).c_str(), loop);
}

void NCL::CSC8503::AudioSystem::PlayAudio( string filename, Vector3 position, bool loop)
{
	ISound* s = engine->play3D((Assets::AUDIODIR + filename).c_str(), position, loop, false, true);
	s->setMinDistance(50.0f);
	s->setMaxDistance(500.0f);
}

void NCL::CSC8503::AudioSystem::PlaySFX( string filename, Vector3 position, bool loop)
{
	engine->play3D((Assets::SFXDIR + filename).c_str(), position, loop);
}
