#include "AudioSystem.h"

ISoundEngine* NCL::CSC8503::AudioSystem::engine = nullptr;

NCL::CSC8503::AudioSystem::AudioSystem()
{
	engine = createIrrKlangDevice();
	engine->loadPlugins("./ikpMP3.dll");
	engine->loadPlugins("./ikpFlac.dll");
	engine->setSoundVolume(volumeVar);
	std::cout << "irrKlang audio system initialized\n";
}

NCL::CSC8503::AudioSystem::~AudioSystem()
{
	engine->drop();
	engine = 0;
}

void NCL::CSC8503::AudioSystem::Update(Vector3 listener, Vector3 forward)
{
	engine->setListenerPosition(listener, forward);
}

void NCL::CSC8503::AudioSystem::Update(vec3df listener, vec3df forward)
{
	Update(Vector3(listener), Vector3(forward));
}

void NCL::CSC8503::AudioSystem::Update(Camera camera)
{
	engine->setListenerPosition(camera.GetPosition(), Vector3(camera.BuildViewMatrix().GetRow(2)).Normalised());
}

void NCL::CSC8503::AudioSystem::Update(Transform trans)
{
	engine->setListenerPosition(trans.GetPosition(), trans.Forward());
}

ISound* NCL::CSC8503::AudioSystem::Play(ISoundSource* sound, bool loop, bool trackable)
{
	return engine->play2D(sound, loop, false, trackable, true);
}

ISound* NCL::CSC8503::AudioSystem::Play(ISoundSource* sound, Vector3 position, bool loop, bool trackable)
{
	return engine->play3D(sound, position, loop, false, trackable, true);
}

ISound* NCL::CSC8503::AudioSystem::Play(string filename, bool loop, bool trackable)
{
	return engine->play2D((filename).c_str(), loop, false, trackable);
}

ISound* NCL::CSC8503::AudioSystem::Play(string filename, Vector3 position, bool loop, bool trackable)
{
	return engine->play3D((filename).c_str(), position, loop, false, trackable);
}

ISound* NCL::CSC8503::AudioSystem::PlayAudio(string filename, bool loop, bool trackable)
{
	return Play(Assets::AUDIODIR + filename, loop, trackable);
}

ISound* NCL::CSC8503::AudioSystem::PlaySFX(string filename, bool loop, bool trackable)
{
	return Play(Assets::SFXDIR + filename, loop, trackable);
}

ISound* NCL::CSC8503::AudioSystem::PlayAudio(string filename, Vector3 position, bool loop, bool trackable)
{
	return Play(Assets::AUDIODIR + filename, position, loop, trackable);
}
ISound* NCL::CSC8503::AudioSystem::PlaySFX(string filename, Vector3 position, bool loop, bool trackable)
{
	return Play(Assets::SFXDIR + filename, position, loop, trackable);
}

ISoundSource* NCL::CSC8503::AudioSystem::AddSource(string filename)
{
	//return engine->addSoundSourceFromFile(filename.c_str());
	return engine->getSoundSource(filename.c_str());
}

ISoundSource* NCL::CSC8503::AudioSystem::AddSourceAudio(string filename)
{
	return AddSource(Assets::AUDIODIR + filename);
}

ISoundSource* NCL::CSC8503::AudioSystem::AddSourceSFX(string filename)
{
	return AddSource(Assets::SFXDIR + filename);
}

void NCL::CSC8503::AudioSystem::StopAll()
{
	if (engine) engine->stopAllSounds();
}

void NCL::CSC8503::AudioSystem::RemoveSource(ISoundSource* source)
{
	engine->removeSoundSource(source);
}

void NCL::CSC8503::AudioSystem::RemoveSource(string filename)
{
	engine->removeSoundSource(filename.c_str());
}

void NCL::CSC8503::AudioSystem::RemoveSourceAudio(string filename)
{
	RemoveSource(Assets::AUDIODIR + filename);
}

void NCL::CSC8503::AudioSystem::RemoveSourceSFX(string filename)
{
	RemoveSource(Assets::SFXDIR + filename);
}
