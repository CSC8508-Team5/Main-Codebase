#include "AudioSystem.h"

ISoundEngine* NCL::CSC8503::AudioSystem::engine;
vector<ISoundSource*> NCL::CSC8503::AudioSystem::sources;

NCL::CSC8503::AudioSystem::AudioSystem()
{
	engine = createIrrKlangDevice();
	engine->loadPlugins("./ikpMP3.dll");
	engine->loadPlugins("./ikpFlac.dll");
	engine->setSoundVolume(1.0f);
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

ISound* NCL::CSC8503::AudioSystem::Play(ISoundSource* sound, bool loop)
{
	return engine->play2D(sound, loop, false, true, true);
}

ISound* NCL::CSC8503::AudioSystem::Play(ISoundSource* sound, Vector3 position, bool loop)
{
	return engine->play3D(sound, position, loop, false, true, true);
}

ISound* NCL::CSC8503::AudioSystem::Play(string filename, bool loop)
{
	return engine->play2D((filename).c_str(), loop, false, true, ESM_AUTO_DETECT, true);
}

ISound* NCL::CSC8503::AudioSystem::Play(string filename, Vector3 position, bool loop)
{
	return engine->play3D((filename).c_str(), position, loop, false, true, ESM_AUTO_DETECT, true);
}

ISound* NCL::CSC8503::AudioSystem::PlayAudio(string filename, bool loop)
{
	return Play(GetAudioFilename(filename), loop);
}

ISound* NCL::CSC8503::AudioSystem::PlaySFX(string filename, bool loop)
{
	return Play(GetSFXFilename(filename), loop);
}

ISound* NCL::CSC8503::AudioSystem::PlayAudio(string filename, Vector3 position, bool loop)
{
	return Play(GetAudioFilename(filename), position, loop);
}
ISound* NCL::CSC8503::AudioSystem::PlaySFX(string filename, Vector3 position, bool loop)
{
	return Play(GetSFXFilename(filename), position, loop);
}

ISoundSource* NCL::CSC8503::AudioSystem::AddSource(string filename)
{

	ISoundSource* s = engine->addSoundSourceFromFile(filename.c_str());
	return s;
}

ISoundSource* NCL::CSC8503::AudioSystem::AddSourceAudio(string filename)
{
	return AddSource(GetAudioFilename(filename));
}

ISoundSource* NCL::CSC8503::AudioSystem::AddSourceSFX(string filename)
{
	return AddSource(GetSFXFilename(filename));
}

void NCL::CSC8503::AudioSystem::RemoveSource(string filename)
{
	engine->removeSoundSource(filename.c_str());
}

void NCL::CSC8503::AudioSystem::RemoveSourceAudio(string filename)
{
	RemoveSource(GetAudioFilename(filename));
}

void NCL::CSC8503::AudioSystem::RemoveSourceSFX(string filename)
{
	RemoveSource(GetSFXFilename(filename));
}
