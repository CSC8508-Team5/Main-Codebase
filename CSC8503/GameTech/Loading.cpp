#include "Loading.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/GameTimer.h"
#include "../CSC8503Common/PositionConstraint.h"
#include <list>
#include <algorithm>
#include <math.h>
#include<thread>
Loading::Loading() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);

	player = new GameObject;
	yaw = 0.0f;
	pitch = 0.0f;

	Debug::SetRenderer(renderer);
	InitialAssets();
}

Loading::~Loading(){
	delete player;
	delete world;
	delete renderer;
	delete basicTex;
	delete basicShader;
	delete cubeMesh;
}



void Loading::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-5.0f);
	world->GetMainCamera()->SetYaw(270.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 0, 0));
}


void Loading::InitialAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);


	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");




	InitCamera();
	glDisable(GL_DEBUG_OUTPUT);


}

void NCL::CSC8503::Loading::UpdateGame(float dt)
{
	Window::GetWindow()->ShowOSPointer(false);
	Window::GetWindow()->LockMouseToWindow(true);

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);

	renderer->Render();
}