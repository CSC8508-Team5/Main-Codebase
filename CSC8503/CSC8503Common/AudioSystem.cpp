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

void NCL::CSC8503::AudioSystem::Update(Camera camera)
{
	engine->setListenerPosition(camera.GetPosition(), Vector3(camera.BuildViewMatrix().GetColumn(2)));
}

void NCL::CSC8503::AudioSystem::PlayAudio(string filename)
{
	engine->play2D((Assets::AUDIODIR + filename).c_str());
}

void NCL::CSC8503::AudioSystem::PlaySFX(string filename)
{
	engine->play2D((Assets::SFXDIR + filename).c_str());
}
