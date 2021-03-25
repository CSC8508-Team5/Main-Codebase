#include "TutorialGame.h"
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
#include "windows.h"
#include "psapi.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame(SettingsManager* s) {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	renderer->SetIsRenderFlame(false);
	physics = new PhysicsSystem(*world, s->GetUseBulletEngine());
	//irrklang audio system
	audio = new AudioSystem(s);

	inDebugMode = false;
	forceMagnitude = 10.0f;
	useGravity = true;
	inSelectionMode = false;
	//current level
	currentLevel = 1;

	//adding for level design
	platformtimer = 0.0f;
	platforms = new GameObject * [17];
	numcoins = 25; // Upper limit of coins
	coins = new GameObject * [numcoins]; //No more than 25 coins
	cannonBullet = new GameObject * [10]; //No more than 10 cannon
	spinplat = new GameObject;
	player = new GameObject;
	yaw = 0.0f;
	pitch = 0.0f;
	isjump = false;
	pausetime = 0;
	score = 0;
	//gamestate
	ispause = false;
	//gamestate
	numstairs = 14;
	coincollected = 0;
	//end 

	settings = s;
	//selectionObject = player;

	if (settings)
		langContent = new LanguageManager();
	else
		langContent = new LanguageManager("en");

	Debug::SetRenderer(renderer);

	InitialiseAssets();

	//--------------------------------------------------In Game UI------------------------------------------//
	InGameUI = new DW_UIPanel("InGameUI");

	Score_text = new DW_UIText("Scoretext", langContent->GetText("score") + std::to_string((int)(coincollected)), 0.7f, NCL::Maths::Vector3{ 1000.0f,650.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f });
	Timer_text = new DW_UIText("Timertext", langContent->GetText("time"), 0.7f, NCL::Maths::Vector3{ 30.0f,650.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f });
	InGameUI->AddComponent(Score_text);
	InGameUI->AddComponent(Timer_text);

	InGameUI1 = new DW_UIPanel("InGameUI");
	Debug_text1 = new DW_UIText("Scoretext", "FPS: ", 0.5f, NCL::Maths::Vector3{ 900.0f,300.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f });
	Debug_text2 = new DW_UIText("Scoretext", "DEBUG_TEXT_TIMING_COSTS", 0.5f, NCL::Maths::Vector3{ 900.0f,250.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f });
	Debug_text3 = new DW_UIText("Scoretext", "DEBUG_TEXT_MEMORY_FOOTPRINT", 0.5f, NCL::Maths::Vector3{ 900.0f,200.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f });
	Debug_text4 = new DW_UIText("Scoretext", "Click object for info", 0.5f, NCL::Maths::Vector3{ 900.0f,150.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f });
	InGameUI1->AddComponent(Debug_text1);
	InGameUI1->AddComponent(Debug_text2);
	InGameUI1->AddComponent(Debug_text3);
	InGameUI1->AddComponent(Debug_text4);

	DW_UIRenderer::get_instance().AddPanel(InGameUI1);
	InGameUI1->SetPanelIsEnable(false);
	DW_UIRenderer::get_instance().AddPanel(InGameUI);
	InGameUI->SetPanelIsEnable(true);
	//-----------------------------------------------------Ui-----------------------------------------------------------------------//
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("Cylinder.msh", &cylinderMesh);
	loadFunc("Male1.msh", &charMeshA);
	loadFunc("courier.msh", &charMeshB);
	loadFunc("security.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);

	loadFunc("Cylinder.msh", &spinplatMesh);

	renderer->SetSphereMesh(sphereMesh);
	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");


	whiteTex = (OGLTexture*)TextureLoader::LoadAPITexture("white.png");
	greyTex = (OGLTexture*)TextureLoader::LoadAPITexture("grey.png");
	greenTex = (OGLTexture*)TextureLoader::LoadAPITexture("green.png");
	redTex = (OGLTexture*)TextureLoader::LoadAPITexture("red.png");
	yellowTex = (OGLTexture*)TextureLoader::LoadAPITexture("yellow.png");
	finishTex = (OGLTexture*)TextureLoader::LoadAPITexture("finish.png");

	InitWorld();
	InitCamera();
	glDisable(GL_DEBUG_OUTPUT);
	scoreAdded = false;
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete cylinderMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;
	delete spinplatMesh;

	delete basicTex;
	delete basicShader;

	delete[] platforms; // Gameobject** is an array -> this may not be correct but needs cleaning up -Conor
	delete[] coins;
	delete[] cannonBullet;
	delete spinplat;
	delete player;
	delete InGameUI;
	delete Score_text;

	delete physics;
	delete renderer;
	delete world;
	delete audio;

	delete langContent;
}

void TutorialGame::Reload() {
	timer = 120;

	if (currentLevel == 3) {

		player->GetTransform().SetPosition(Vector3(-150, 10, 0));
		Quaternion orientation = Quaternion(0, -1, 0, 1);
		orientation.Normalise();
		player->GetTransform().SetOrientation(orientation);
		SphereVolume* volume = new SphereVolume(1.5f);
		for (int i = 0; i < numcoins; ++i) {
			if (coins[i] != nullptr) {
				coins[i]->SetBoundingVolume((CollisionVolume*)volume);
				coins[i]->GetTransform().SetScale(Vector3(0.25, 0.25, 0.25));
			}
		}
		coincollected = 0;

	}
	else {
		world->ClearAndErase();
		physics->Clear();
		InitWorld();
	}


	startTime = ::GetTickCount64();

}



string enum_to_string(VolumeType type) {
	switch (type) {
	case VolumeType::AABB:
		return "AABB";
	case VolumeType::OBB:
		return "OBB";
	case VolumeType::Sphere:
		return "Sphere";
	case VolumeType::Mesh:
		return "Mesh";
	case VolumeType::Capsule:
		return "Capsule";
	case VolumeType::Compound:
		return "Compound";
	case VolumeType::Invalid:
		return "Invalid";
	}
	return "Empty";
}


/* All update functions */
void TutorialGame::UpdateGame(float dt) {
	fps = 1.0f / dt;
	Debug_text1->SetText("FPS: " + std::to_string((int)fps));	//updateFPS


	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

	Debug_text2->SetText("Virtual Memory used: " + std::to_string(virtualMemUsedByMe / 100000) + " MBs");	//vram
	Debug_text3->SetText("RAM used: " + std::to_string(physMemUsedByMe / 100000) + " MBs");	//ram


	if (GameStateManager::GetGameState() == GameStateManager::State::Playing)
	{

		if (timer <= 0) {

			GameStateManager::SetGameState(GameStateManager::State::LoseTimeout);
		}
		if ((int(::GetTickCount64() - startTime) >= 1000) && (pausetime == 0)) {
			timer -= 1;
			startTime = ::GetTickCount64();
			pausetime = 0;
		}
		Timer_text->SetText(langContent->GetText("time") + std::to_string(timer) + " s");


		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::Y))
			GameStateManager::SetGameState(GameStateManager::State::Win);

		UpdateKeys();
		if (inDebugMode) {

			InGameUI1->SetPanelIsEnable(true);
			SelectObject();
		}
		else {
			InGameUI1->SetPanelIsEnable(false);

		}
		if (selectionObject) {
			string objectsStr;
			if (selectionObject->GetCollisionObjectCount() > 0)
			{
				vector<GameObject*>::iterator begin, end;
				selectionObject->GetCollisionObjectIterators(begin, end);
				for (vector<GameObject*>::iterator i = begin; i != end; i++)
				{
					objectsStr += (*i)->GetWorldID() + ":" + (*i)->GetName() + ", ";
				}
			}

			Debug_text4->SetText(enum_to_string(selectionObject->GetBoundingVolume()->type) + " Colliding with " + objectsStr);	//non functional atm
		}



		//AI

		/*if (testEnemy) {
			if (checknum == 1) {
				checkAitime = gametime.GetTotalTimeSeconds();
				checknum = 0;
			}
			else {
				checkAitime1 = gametime.GetTotalTimeSeconds();
				if (checkAitime1 - checkAitime >= 5) {
					world->RemoveGameObject(testEnemy);
					Vector3 enemyp = player->GetTransform().GetPosition() + Vector3(0, 0, -5);
					testEnemy = AddAiEnemyToWorld(enemyp);
					checknum = 1;
				}
				else {
					checknum = 0;
				}
			}


		}*/


		UpdatePlayer(dt);
		world->UpdateWorld(dt);
		physics->Update(dt);

		if (lockedObject != nullptr) {
			Vector3 objPos = lockedObject->GetTransform().GetPosition();
			Vector3 camPos = objPos + lockedOffset;

			Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

			Matrix4 modelMat = temp.Inverse();

			Quaternion q(modelMat);
			Vector3 angles = q.ToEuler(); //nearly there now!

			world->GetMainCamera()->SetPosition(camPos);
			world->GetMainCamera()->SetPitch(angles.x);
			world->GetMainCamera()->SetYaw(angles.y);

		}
		if (testStateObject) {
			testStateObject->Update(dt);

		}



		CollisionDetection::CollisionInfo info;
		if (currentLevel == 1) {//Update level 1
			UpdateLevelOne();
		}
		else if (currentLevel == 2) {
			UpdateLevelTwo();
		}
		else if (currentLevel == 3) { //Update level 3
			UpdateLevelThree(dt);

		}


		world->GetMainCamera()->UpdateThirdPersonCamera(player->GetTransform(), Vector3::Up(), dt);
		if (testEnemy) {
			world->GetSecondCamera()->UpdateThirdPersonCamera(testEnemy->GetTransform(), Vector3::Up(), dt);
		}
	}


	renderer->Update(dt);
	Debug::FlushRenderables(dt);
	DW_UIRenderer::get_instance().Update(dt);
	audio->Update(*world->GetMainCamera());
	if(renderer->GetSplitscreen())
		renderer->RenderSplitscreen(2);
	else
		renderer->Render();
}

void TutorialGame::UpdateLevelOne() {
	float speed = 30.0f;
	for (int i = 1; i < numstairs - 1; ++i) {
		Vector3 position = platforms[i]->GetTransform().GetPosition();
		if (i % 3 == 1) {
			if (position.z <= -60) {
				if (physics->isUseBulletPhysics())
				{
					platforms[i]->GetBulletBody()->setActivationState(true);
					platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, speed));
				}
				else
					platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, speed));
			}
			else if (position.z >= 10) {
				if (physics->isUseBulletPhysics())
				{
					platforms[i]->GetBulletBody()->setActivationState(true);
					platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, -speed));
				}
				else
					platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, -speed));
			}
		}
		else if (i % 3 == 0) {
			if (position.z <= -10) {
				if (physics->isUseBulletPhysics())
				{
					platforms[i]->GetBulletBody()->setActivationState(true);
					platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, speed));
				}
				else
					platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, speed));
			}
			else if (position.z >= 60) {
				if (physics->isUseBulletPhysics())
				{
					platforms[i]->GetBulletBody()->setActivationState(true);
					platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, -speed));
				}
				else
					platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, -speed));
			}
		}
		UpdateCoins();
		UpdateCannonBullet(cannonBullet[0], Vector3(-100, 5, -80) + Vector3(6, 7, 6), "left");
		UpdateCannonBullet(cannonBullet[1], Vector3(-100, 25, 80) + Vector3(6, 7, -6), "right");
		UpdateCannonBullet(cannonBullet[2], Vector3(50, 45, -80) + Vector3(6, 7, 6), "left");
		UpdateCannonBullet(cannonBullet[3], Vector3(50, 55, 80) + Vector3(6, 7, -6), "right");
	}
};

void TutorialGame::UpdateLevelTwo() {
	Score_text->SetText(langContent->GetText("score") + std::to_string((int)(score + timer * 10 + coincollected * 50)));

}

void TutorialGame::UpdateLevelThree(float dt) {
	UpdateCoins();

}


//todo update coins behaviour
void TutorialGame::UpdateCoins() {
	SphereVolume* volume = new SphereVolume(0.0f);
	Score_text->SetText(langContent->GetText("score") + std::to_string((int)(score + timer * 10 + coincollected * 50)));
	for (int i = 0; i < numcoins; ++i) {
		if (coins[i]) {
			if (physics->isUseBulletPhysics())
			{
				coins[i]->GetBulletBody()->activate(true);
				coins[i]->GetBulletBody()->setAngularVelocity(Vector3(0, 2, 0));
			}
			else
				coins[i]->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 2, 0));

		}
	}
}

void TutorialGame::UpdateCannonBullet(GameObject* bullet, const Vector3& startPosition, string direction) {
	SphereVolume* volume = new SphereVolume(0.0f);
	Vector3 relativePos = player->GetTransform().GetPosition() - bullet->GetTransform().GetPosition();
	float currentDistance = relativePos.Length();
	float offset = currentDistance;
	float startDistance = (bullet->GetTransform().GetPosition() - startPosition).Length();
	if (startDistance >= 150) {
		bullet->GetTransform().SetPosition(startPosition);
	}
	else	if (abs(offset) > 100.0f) {
		if (direction == "left") {
			if (physics->isUseBulletPhysics())
				bullet->GetBulletBody()->setLinearVelocity(Vector3(0, 0, 10));
			else
				bullet->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 10));


		}
		else if (direction == "right") {
			if (physics->isUseBulletPhysics())
				bullet->GetBulletBody()->setLinearVelocity(Vector3(0, 0, -10));
			else
				bullet->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, -10));

		}
	}
	else if (abs(offset) > 0.0f) {
		if (physics->isUseBulletPhysics())
			bullet->GetBulletBody()->setLinearVelocity(relativePos);
		else
		{
			PhysicsObject* bulletPhys = bullet->GetPhysicsObject();
			bulletPhys->SetLinearVelocity(relativePos);
		}
	}
	CollisionDetection::CollisionInfo info;
	if (CollisionDetection::ObjectIntersection(player, bullet, info)) {
		timer = timer - 5;
		bullet->GetTransform().SetPosition(startPosition);
	}
	for (int i = 0; i < numstairs; ++i) {
		if (CollisionDetection::ObjectIntersection(bullet, platforms[i], info)) {
			bullet->GetTransform().SetPosition(startPosition);
		}
	}
}

void TutorialGame::UpdatePlayer(float dt) {
	if (player->GetTransform().GetPosition().y < -5) {
		ResetCharacters();
		timer = timer - 5;
	}

}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1

	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F4)) {
		AudioSystem::PlaySFX("bell.wav");
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F5)) {
		audioAgent->GetSoundSource()->Play();
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F6)) {
		audioAgent->GetSoundSource()->SetMinDistance(5.0f);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K)) {
		inDebugMode = !inDebugMode;
		if (inDebugMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}





}

/* Scoreboard functionality */
void TutorialGame::AddBoardScore(int score) {

	std::fstream file;				//file stuff
	file.open("HighScore.txt");

	std::string s;
	std::vector<int> scores;

	getline(file, s); // skips first line

	while ((getline(file, s))) {

		scores.push_back(std::stoi(s));
	}
	//closes default file settings
	file.close();

	//check if any scores were beat
	for (int i = 0; i < scores.size(); i++) {
		if (score > scores.at(i)) {
			scores.insert(scores.begin() + i, score);//inserts before current position
			scores.pop_back(); //removes the lowest score
			break;
		}
	}

	file.open("HighScore.txt", std::fstream::out | std::fstream::trunc); // clears file and re-writes to it
	file << "IF YOU'RE READING THIS AND CHANGING THE SCORE YOU'RE A DIRTY CHEATER >:(\n";
	for (int i = 0; i < scores.size(); i++) {
		file << scores.at(i) << "\n";
	}
	file.close();

}

std::string NCL::CSC8503::TutorialGame::GetScoreBoard() {

	std::ifstream file;
	file.open("HighScore.txt");

	if (file.is_open()) {
		std::string s;
		std::string sFinal = "";
		int counter = 1;
		(getline(file, s)); //skips first line 
		while ((getline(file, s))) {
			sFinal.append(std::to_string(counter) + " - " + s + '\n');
			counter++;
		}
		file.close();

		return sFinal;
	}
}

/* Camera, World and Player initialisation */
void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-5.0f);
	world->GetMainCamera()->SetYaw(270.0f);
	world->GetMainCamera()->SetPosition(player->GetTransform().GetPosition() + Vector3(-15, 5, 0));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	physics->UseGravity(useGravity);
	world->ClearAndErase();
	physics->Clear();
	renderer->GetDeferredRenderingHelper()->ClearLights();
	if (currentLevel == 1)
		InitLevel1();       // start lv1 
	else if (currentLevel == 2)
		InitLevel2();  // start lv2 
	else if (currentLevel == 3)
		InitLevel3(); // Start Level 3
	else //Just make sure nobody write number more than 3 for now
		currentLevel = 1;
	startTime = ::GetTickCount64();
	coincollected = 0;
	timer = 120;
}

/* Level Preparations */
void TutorialGame::InitLevel1() {

	InitCharaters(Vector3(-150, 10, 0));
	InstantiateCharaters();
	//-------------LV1 -------------------------------------

	platforms = LevelOne();
	std::vector<Vector3> poses;
	for (int i = 0; i < numstairs; i++)
	{

		poses.push_back(platforms[i]->GetTransform().GetPosition());
	}
	AddFinishAttributesToObject(platforms[numstairs - 1]);

	renderer->GetDeferredRenderingHelper()->SetPointLights(poses);
	renderer->GetDeferredRenderingHelper()->SetDirectionalLight(NCL::Maths::Vector3(-180.0f, 100.0f, 70.0f));
	//-------------LV1 -------------------------------------


	coincollected = 0;
}

void TutorialGame::InitLevel2() {

	InitCharaters(Vector3(0, 0, -320));
	InstantiateCharaters();
	InitLevel2design();
}

void TutorialGame::InitLevel3() {
	//InitCharaters(Vector3(-80, 5, 0));
	InitCharaters(Vector3(-150, 5, 0));
	InstantiateCharaters();
	LevelThree();
}

/* Game Levels */
GameObject** TutorialGame::LevelOne() {
	Vector3 PlatformSize = Vector3(10, 4, 50);
	Vector3 cubeSize = Vector3(10, 4, 10);
	Vector3 middlecubeSize = Vector3(10, 4, 20);

	float invCubeMass = 0.1f; // how heavy the middle pieces are
	float cubeDistance = 20; // distance between links

	Vector3 startPos = Vector3(-150, 5, 0);

	platforms[0] = AddCubeToWorld(startPos + Vector3(0, 0, 0), PlatformSize, 0);
	platforms[numstairs - 1] = AddCubeToWorld(startPos + Vector3((numstairs - 1) * cubeDistance, (numstairs - 1) * 5.0f, 0), PlatformSize, 0);
	platforms[0]->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
	platforms[numstairs - 1]->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	//initial coins
	for (int i = 0; i < numcoins; ++i) {
		coins[i] = nullptr;
	}

	for (int i = 1; i < numstairs - 1; ++i) {
		if (i % 3 == 1) {
			platforms[i] = AddCubeToWorld(startPos + Vector3(i * cubeDistance, i * 5.0f, -40), cubeSize, invCubeMass);
			if (physics->isUseBulletPhysics())
			{

				platforms[i]->GetBulletBody()->setActivationState(true);
				platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, 30));
				platforms[i]->GetBulletBody()->setLinearFactor(Vector3::Zero());
				platforms[i]->GetBulletBody()->setAngularFactor(Vector3::Zero());
			}
			else
			{
				platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 30));
				platforms[i]->GetPhysicsObject()->SetInverseMass(0);
			}
			platforms[i]->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
			coins[i] = AddCoins(startPos + Vector3(i * cubeDistance, (i + 1) * 5.0f + 3, -20));
		}
		else if (i % 3 == 2) {
			platforms[i] = AddCubeToWorld(startPos + Vector3(i * cubeDistance, i * 5.0f, 0), middlecubeSize, 0);
			platforms[i]->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
		}
		else if (i % 3 == 0) {
			platforms[i] = AddCubeToWorld(startPos + Vector3(i * cubeDistance, i * 5.0f, 40), cubeSize, invCubeMass);
			if (physics->isUseBulletPhysics())
			{
				platforms[i]->GetBulletBody()->setActivationState(true);
				platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, -30));
				platforms[i]->GetBulletBody()->setLinearFactor(Vector3::Zero());
				platforms[i]->GetBulletBody()->setAngularFactor(Vector3::Zero());
			}
			else
			{
				platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, -30));
				platforms[i]->GetPhysicsObject()->SetInverseMass(0);
			}
			platforms[i]->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
			coins[i] = AddCoins(startPos + Vector3(i * cubeDistance, (i + 1) * 5.0f + 3, 20));
		}
	}
	cannonBullet[0] = AddCannonToWorld(Vector3(-100, 5, -80), "left");
	cannonBullet[1] = AddCannonToWorld(Vector3(-100, 25, 80), "right");
	cannonBullet[2] = AddCannonToWorld(Vector3(50, 45, -80), "left");
	cannonBullet[3] = AddCannonToWorld(Vector3(50, 55, 80), "right");
	return platforms;
}

void TutorialGame::InitLevel2design() {
	level2Floor = AddWallToWorld(Vector3(0, 0, 0), 56, 1, 350, yellowTex, "floor");  //floor	
	AddWallToWorld(Vector3(-56.5, 10, 0), 1, 11, 350, whiteTex, "sidewall");  //sidewall
	AddWallToWorld(Vector3(56.5, 10, 0), 1, 11, 350, whiteTex, "sidewall");  //sidewall
	AddWallToWorld(Vector3(0, 10, 349), 56, 11, 1, whiteTex, "frontwall");  //frontwall
	AddWallToWorld(Vector3(0, 10, -349), 56, 11, 1, whiteTex, "backwall");  //backwall	
	level2finishLine = AddWallToWorld(Vector3(0, 0.1, 320), 56, 1, 7, finishTex, "finishline");  //finish
	AddFinishAttributesToObject(level2finishLine);

	AddWallToWorld(Vector3(-55, 9, -220), 1, 8, 1, greenTex, "pillar");  //pillar	
	AddDoorToWorld(Vector3(-49, 10, -220), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddDoorToWorld(Vector3(-39, 10, -220), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddWallToWorld(Vector3(-33, 9, -220), 1, 8, 1, greenTex, "pillar");  //pillar	

	AddWallToWorld(Vector3(-22, 9, -220), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(-11, 9, -220), 1, 8, 1, greenTex, "pillar");  //pillar	

	AddWallToWorld(Vector3(0, 9, -220), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(11, 9, -220), 1, 8, 1, greenTex, "pillar");  //pillar		

	AddWallToWorld(Vector3(22, 9, -220), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(33, 9, -220), 1, 8, 1, greenTex, "pillar");  //pillar
	AddDoorToWorld(Vector3(39, 10, -220), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddDoorToWorld(Vector3(49, 10, -220), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddWallToWorld(Vector3(55, 9, -220), 1, 8, 1, greenTex, "pillar");  //pillar

	AddWallToWorld(Vector3(-55, 9, -160), 1, 8, 1, greenTex, "pillar");  //pillar	

	AddWallToWorld(Vector3(-44, 9, -160), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(-33, 9, -160), 1, 8, 1, greenTex, "pillar");  //pillar	

	AddWallToWorld(Vector3(-22, 9, -160), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(-11, 9, -160), 1, 8, 1, greenTex, "pillar");  //pillar	
	AddDoorToWorld(Vector3(-5, 10, -160), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddDoorToWorld(Vector3(5, 10, -160), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddWallToWorld(Vector3(11, 9, -160), 1, 8, 1, greenTex, "pillar");  //pillar		

	AddWallToWorld(Vector3(22, 9, -160), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(33, 9, -160), 1, 8, 1, greenTex, "pillar");  //pillar

	AddWallToWorld(Vector3(44, 9, -160), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(55, 9, -160), 1, 8, 1, greenTex, "pillar");  //pillar

	AddWallToWorld(Vector3(-55, 9, -100), 1, 8, 1, greenTex, "pillar");  //pillar	

	AddWallToWorld(Vector3(-33, 9, -100), 1, 8, 1, greenTex, "pillar");  //pillar	

	AddWallToWorld(Vector3(-11, 9, -100), 1, 8, 1, greenTex, "pillar");  //pillar	
	AddDoorToWorld(Vector3(-5, 10, -100), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddDoorToWorld(Vector3(5, 10, -100), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddWallToWorld(Vector3(11, 9, -100), 1, 8, 1, greenTex, "pillar");  //pillar		

	AddWallToWorld(Vector3(33, 9, -100), 1, 8, 1, greenTex, "pillar");  //pillar
	AddDoorToWorld(Vector3(39, 10, -100), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddDoorToWorld(Vector3(49, 10, -100), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddWallToWorld(Vector3(55, 9, -100), 1, 8, 1, greenTex, "pillar");  //pillar	
	AddWallToWorld(Vector3(-44, 9, -100), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(-22, 9, -100), 10, 8, 1, redTex, "block"); //block	
	AddWallToWorld(Vector3(22, 9, -100), 10, 8, 1, redTex, "block"); //block

	//---------------------------------5-------------------------------

	AddWallToWorld(Vector3(0, 9, -40), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(22, 9, -40), 10, 8, 1, redTex, "block"); //block	
	AddWallToWorld(Vector3(-44, 9, -40), 12, 8, 1, greenTex, "pillar");  //pillar	
	AddDoorToWorld(Vector3(-27, 10, -40), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddDoorToWorld(Vector3(-17, 10, -40), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddWallToWorld(Vector3(-11, 9, -40), 1, 8, 1, greenTex, "pillar");  //pillar	
	AddWallToWorld(Vector3(11, 9, -40), 1, 8, 1, greenTex, "pillar");  //pillar		
	AddWallToWorld(Vector3(44, 9, -40), 12, 8, 1, greenTex, "pillar");  //pillar


	AddWallToWorld(Vector3(-22, 9, 20), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(0, 9, 20), 10, 8, 1, redTex, "block"); //block	
	AddWallToWorld(Vector3(-44, 9, 20), 12, 8, 1, greenTex, "pillar");  //pillar	
	AddWallToWorld(Vector3(-11, 9, 20), 1, 8, 1, greenTex, "pillar");  //pillar	
	AddWallToWorld(Vector3(11, 9, 20), 1, 8, 1, greenTex, "pillar");  //pillar		
	AddDoorToWorld(Vector3(17, 10, 20), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddDoorToWorld(Vector3(27, 10, 20), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddWallToWorld(Vector3(44, 9, 20), 12, 8, 1, greenTex, "pillar");  //pillar

	AddWallToWorld(Vector3(-22, 9, 80), 10, 8, 1, redTex, "block"); //block	
	AddWallToWorld(Vector3(22, 9, 80), 10, 8, 1, redTex, "block"); //block
	AddWallToWorld(Vector3(-44, 9, 80), 12, 8, 1, greenTex, "pillar");  //pillar	
	AddWallToWorld(Vector3(-11, 9, 80), 1, 8, 1, greenTex, "pillar");  //pillar	
	AddDoorToWorld(Vector3(-5, 10, 80), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddDoorToWorld(Vector3(5, 10, 80), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddWallToWorld(Vector3(11, 9, 80), 1, 8, 1, greenTex, "pillar");  //pillar		
	AddWallToWorld(Vector3(44, 9, 80), 12, 8, 1, greenTex, "pillar");  //pillar


	AddWallToWorld(Vector3(-11, 9, 140), 10, 8, 1, redTex, "block"); //block	
	AddWallToWorld(Vector3(-38.5, 9, 140), 18, 8, 1, greenTex, "pillar");  //pillar	
	AddWallToWorld(Vector3(0, 9, 140), 1, 8, 1, greenTex, "pillar");  //pillar		
	AddDoorToWorld(Vector3(6, 10, 140), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddDoorToWorld(Vector3(16, 10, 140), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddWallToWorld(Vector3(38.5, 9, 140), 18, 8, 1, greenTex, "pillar");  //pillar

	AddWallToWorld(Vector3(11, 9, 200), 10, 8, 1, redTex, "block"); //block	
	AddWallToWorld(Vector3(-38.5, 9, 200), 18, 8, 1, greenTex, "pillar");  //pillar	
	AddDoorToWorld(Vector3(-16, 10, 200), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddDoorToWorld(Vector3(-6, 10, 200), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddWallToWorld(Vector3(0, 9, 200), 1, 8, 1, greenTex, "pillar");  //pillar		
	AddWallToWorld(Vector3(38.5, 9, 200), 18, 8, 1, greenTex, "pillar");  //pillar

	AddWallToWorld(Vector3(11, 9, 260), 10, 8, 1, redTex, "block"); //block	
	AddWallToWorld(Vector3(-38.5, 9, 260), 18, 8, 1, greenTex, "pillar");  //pillar	
	AddDoorToWorld(Vector3(-16, 10, 260), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door 
	AddDoorToWorld(Vector3(-6, 10, 260), Vector3(5, 8, 1), redTex, "DestructibleDoor"); //door
	AddWallToWorld(Vector3(0, 9, 260), 1, 8, 1, greenTex, "pillar");  //pillar		
	AddWallToWorld(Vector3(38.5, 9, 260), 18, 8, 1, greenTex, "pillar");  //pillar


	std::vector<Vector3> poses;
	for (int i = 0; i < 12; i++)
	{
		poses.push_back(Vector3{ -25.0f,0.0f,i * 50.0f - 300.0f });
		poses.push_back(Vector3{ 25.0f,0.0f,i * 50.0f - 300.0f });
	}
	renderer->GetDeferredRenderingHelper()->SetPointLights(poses);
	renderer->GetDeferredRenderingHelper()->SetDirectionalLight(NCL::Maths::Vector3(-120.0f, 100.0f, -20.0f));

}

void TutorialGame::LevelThree() {

	// Platforms 

	//To make player able to jump
	level3Floor = AddCubeToWorld(Vector3(40, 0, 0), Vector3(200, 2, 50), 0);
	level3Floor->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));

	level3finishLine = AddCubeToWorld(Vector3(260, 0, 0), Vector3(20, 2, 50), 0);
	level3finishLine->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	level3finishLine->SetName("Finish");

	AddFinishAttributesToObject(level3finishLine);

	// State Objects ("sliders")
	//todo sliders uncessary as we will call gameobjct->update of every object in world->update()
	sliderVector.emplace_back(AddStateObjectToWorld(Vector3(-60, 6, 0), Vector3(20, 4, 1), false, true));
	sliderVector.emplace_back(AddStateObjectToWorld(Vector3(0, 6, 0), Vector3(20, 4, 1), true, true));
	sliderVector.emplace_back(AddStateObjectToWorld(Vector3(60, 6, 0), Vector3(20, 4, 1), false, true));

	sliderVector.emplace_back(AddStateObjectToWorld(Vector3(200, 6, -30), Vector3(1, 4, 15), false));
	sliderVector.emplace_back(AddStateObjectToWorld(Vector3(200, 6, 0), Vector3(1, 4, 15), true));
	sliderVector.emplace_back(AddStateObjectToWorld(Vector3(200, 6, 30), Vector3(1, 4, 15), false));

	// Cylinder Obstacles
	// First Row
	AddCylinderToWorld(Vector3(100, 3, 0), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(100, 3, -20), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(100, 3, -40), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(100, 3, 20), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(100, 3, 40), 2, 3, 0.0f);

	// Second Row
	AddCylinderToWorld(Vector3(120, 3, 10), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(120, 3, 30), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(120, 3, -10), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(120, 3, -30), 2, 3, 0.0f);

	// Third Row
	AddCylinderToWorld(Vector3(140, 3, 0), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(140, 3, -20), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(140, 3, -40), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(140, 3, 20), 2, 3, 0.0f);
	AddCylinderToWorld(Vector3(140, 3, 40), 2, 3, 0.0f);

	// Coins 
	// Initialise coins
	for (int i = 0; i < numcoins; ++i) {
		coins[i] = nullptr;
	}

	coins[0] = AddCoins(Vector3(-30, 4, 20));
	coins[1] = AddCoins(Vector3(-30, 4, -20));
	coins[2] = AddCoins(Vector3(30, 4, -20));
	coins[3] = AddCoins(Vector3(30, 4, 20));
	coins[4] = AddCoins(Vector3(90, 4, -20));
	coins[5] = AddCoins(Vector3(90, 4, 20));
	coins[6] = AddCoins(Vector3(110, 4, 10));
	coins[7] = AddCoins(Vector3(110, 4, 30));
	coins[8] = AddCoins(Vector3(110, 4, -10));
	coins[9] = AddCoins(Vector3(110, 4, -30));
	coins[10] = AddCoins(Vector3(150, 4, 10));
	coins[11] = AddCoins(Vector3(150, 4, 30));
	coins[12] = AddCoins(Vector3(150, 4, -10));
	coins[13] = AddCoins(Vector3(150, 4, -30));

	std::vector<Vector3> poses;
	for (int i = 0; i < 10; i++)
	{
		poses.push_back(Vector3{ i * 30.0f - 50.0f,0.0f,-15.0f });
		poses.push_back(Vector3{ i * 30.0f - 50.0f,0.0f,15.0f });
	}
	renderer->GetDeferredRenderingHelper()->SetPointLights(poses);
	renderer->GetDeferredRenderingHelper()->SetDirectionalLight(NCL::Maths::Vector3(-180.0f, 100.0f, 70.0f));

}

/* Level elements and Obstacles*/
GameObject* TutorialGame::AddCoins(const Vector3& position) {//No more than 25 coins
	GameObject* coin = AddBonusToWorld(position);
	AddCoinAttributesToObject(coin);
	//AddFinishAttributesToObject(coin);
	return coin;
}

GameObject* TutorialGame::AddCannonToWorld(const Vector3& position, string orientation) {
	GameObject* bullet = new GameObject();
	GameObject* cylinder = new GameObject();
	GameObject* sphere1 = new GameObject();
	GameObject* sphere2 = new GameObject();
	GameObject* platform = new GameObject();
	platform = AddCubeToWorld(position, Vector3(8, 1, 8), 0);
	platform->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

	cylinder = AddCylinderToWorld(position + Vector3(0, 3.5, 0), 2.0f, 5.0f, 0);
	Quaternion cylinderturn;
	if (orientation == "left") {
		cylinderturn = Quaternion(0.5, 0.5, 0, 0.5);
		sphere1 = AddSphereToWorld(position + Vector3(-2.5, 2.5, 1), 1.5f, 0);
		sphere2 = AddSphereToWorld(position + Vector3(1, 2.5, -2.5), 1.5f, 0);
		bullet = AddSphereToWorld(position + Vector3(6, 7, 6), 1.5f, 0);
		bullet->GetPhysicsObject()->SetTrigger(true);
		bullet->SetLayer(GameObject::Layer::Projectile);
	}
	else if (orientation == "right") {
		cylinderturn = Quaternion(0.5, 0, 0.5, 0.5);
		sphere1 = AddSphereToWorld(position + Vector3(-1, 2.5, -1.5), 1.5f, 0);
		sphere2 = AddSphereToWorld(position + Vector3(1, 2.5, 1.5), 1.5f, 0);
		bullet = AddSphereToWorld(position + Vector3(6, 7, -6), 1.5f, 0);
		bullet->GetPhysicsObject()->SetTrigger(true);
		bullet->SetLayer(GameObject::Layer::Projectile);
	}
	else {
		cylinderturn = Quaternion(0.5, 0.5, 0, 0.5);
		sphere1 = AddSphereToWorld(position + Vector3(-1.5, 2.5, -1), 1.5f, 0);
		sphere2 = AddSphereToWorld(position + Vector3(1.5, 2.5, 1), 1.5f, 0);
	}
	cylinderturn.Normalise();
	cylinder->GetTransform().SetOrientation(cylinderturn);
	cylinder->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
	sphere1->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
	sphere2->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
	bullet->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	return bullet;
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);
	float invCubeMass = 5; // how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 25; // constraint distance
	float cubeDistance = 20; // distance between links

	Vector3 startPos = Vector3(5, 100, 5);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0)
		, cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 1)
		* cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) *
			cubeDistance, 0, 0), cubeSize, invCubeMass);

		PositionConstraint* constraint = new PositionConstraint(previous,
			block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;

	}
	PositionConstraint* constraint = new PositionConstraint(previous,
		end, maxDistance);
	world->AddConstraint(constraint);

}

void TutorialGame::BridgeBulletConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);
	float invCubeMass = 0.01f; // how heavy the middle pieces are
	int numLinks = 20;
	float maxDistance = 22; // constraint distance
	float cubeDistance = 20; // distance between links

	Vector3 startPos = Vector3(5, 100, 5);

	GameObject* start = CreateBulletCube(startPos + Vector3(0, 0, 0)
		, cubeSize, 0);
	GameObject* end = CreateBulletCube(startPos + Vector3((numLinks + 1)
		* cubeDistance + 8, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	//relative transform of objects
	btTransform frameInA;
	btTransform frameInB;

	frameInA.setIdentity();
	frameInA.setOrigin(btVector3(cubeDistance / 2, 0, 0));
	frameInB.setIdentity();
	frameInB.setOrigin(btVector3(-cubeDistance / 2, 0, 0));

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = CreateBulletCube(startPos + Vector3((i + 1) *
			cubeDistance, 0, 0), cubeSize, invCubeMass);


		btGeneric6DofSpringConstraint* constraint = new btGeneric6DofSpringConstraint(*previous->GetBulletBody(), *block->GetBulletBody(), frameInA, frameInB, true);
		constraint->setLinearUpperLimit(btVector3(0, 0, 0));
		constraint->setLinearLowerLimit(btVector3(0, 0, 0));
		constraint->setAngularLowerLimit(btVector3(0, -1, -1));
		constraint->setAngularUpperLimit(btVector3(0, 1, 1));
		constraint->enableSpring(3, true);
		constraint->setStiffness(3, 0.01f);


		physics->AddConstraint(constraint);
		previous = block;

	}

	btGeneric6DofSpringConstraint* constraint = new btGeneric6DofSpringConstraint(*previous->GetBulletBody(), *end->GetBulletBody(), frameInA, frameInB, true);

	physics->AddConstraint(constraint);

}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	if (physics->isUseBulletPhysics())
		return CreateBulletFloor(position);
	else
		return CreateFloor(position);
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	if (physics->isUseBulletPhysics())
		return CreateBulletCube(position, dimensions, inverseMass);
	else
		return CreateCube(position, dimensions, inverseMass);
}

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	if (physics->isUseBulletPhysics())
		return CreateBulletSphere(position, radius, inverseMass);
	else
		return CreateSphere(position, radius, inverseMass);
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	if (physics->isUseBulletPhysics())
		return CreateBulletCapsule(position, halfHeight, radius, inverseMass);
	else
		return CreateCapsule(position, halfHeight, radius, inverseMass);
}

GameObject* NCL::CSC8503::TutorialGame::AddCylinderToWorld(const Vector3& position, float radius, float height, float inverseMass)
{
	if (physics->isUseBulletPhysics())
		return CreateBulletCylinder(position, height, radius, inverseMass);
	else
		return CreateCylinder(position, radius, height, inverseMass);
}

GameObject* NCL::CSC8503::TutorialGame::CreateFloor(const Vector3& position)
{
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}


/*
The cylinder now using the CapsuleVolume, might need to change
*/
GameObject* TutorialGame::CreateCylinder(const Vector3& position, float radius, float hight, float inverseMass) {
	GameObject* cylinder = new GameObject();

	Vector3 cylinderSize = Vector3(radius * 2, hight, radius * 2);
	AABBVolume* volume = new AABBVolume(Vector3(radius, hight, radius));
	cylinder->SetBoundingVolume((CollisionVolume*)volume);

	cylinder->GetTransform()
		.SetScale(cylinderSize)
		.SetPosition(position);

	cylinder->SetRenderObject(new RenderObject(&cylinder->GetTransform(), spinplatMesh, basicTex, basicShader));
	cylinder->SetPhysicsObject(new PhysicsObject(&cylinder->GetTransform(), cylinder->GetBoundingVolume()));

	cylinder->GetPhysicsObject()->SetInverseMass(inverseMass);
	cylinder->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(cylinder);

	return cylinder;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* NCL::CSC8503::TutorialGame::CreateSphere(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* NCL::CSC8503::TutorialGame::CreateCube(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, whiteTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* NCL::CSC8503::TutorialGame::CreateCapsule(const Vector3& position, float halfHeight, float radius, float inverseMass)
{
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;
}

GameObject* NCL::CSC8503::TutorialGame::CreateBulletFloor(const Vector3& position)
{
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);

	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	btCollisionShape* shape = new btBoxShape(floorSize);
	btTransform bulletTransform = floor->GetTransform();


	btScalar mass = 0.0f;

	btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape);


	floor->SetBulletPhysicsObject(new btRigidBody(rbInfo));

	world->AddGameObject(floor);

	return floor;
}

GameObject* NCL::CSC8503::TutorialGame::CreateBulletCube(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	//Initialize Gameobject*
	GameObject* cube = new GameObject();

	//Set Gameobject world Transform
	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	//Set Rendering agent
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));

	//Set Object to Kinematic if you want it not affect by Gravity or Force

	//Initialize shape of the collision object
	btCollisionShape* shape = new btBoxShape(dimensions);

	//Initialize bullet inner transfrom(have no scale), you can set it from gameworld transform directly or create new bulletTransfrom
	btTransform bulletTransform = cube->GetTransform();


	//caculate the mass of object, bullet object use mass instead of inverse mass for initialing
	btScalar mass = 0.0f;
	if (inverseMass != 0.0f)
		mass = (1 / inverseMass);

	//if mass != zero, we shall caculate the local inertia by calling bullet api
	bool isDynamic = (mass != 0.0f);
	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//init motionstate from bullet transform
	btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

	//init bullet rigid body
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	//give gameobject the new rigidbody, it will automaticlly add it to bullet world for simulation later
	cube->SetBulletPhysicsObject(new btRigidBody(rbInfo));


	world->AddGameObject(cube);


	return cube;
}

GameObject* NCL::CSC8503::TutorialGame::CreateBulletSphere(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));


	btCollisionShape* shape = new btSphereShape(btScalar(radius));

	btTransform bulletTransform = sphere->GetTransform();


	btScalar mass = 0.0f;
	if (inverseMass != 0.0f)
		mass = (1 / inverseMass);

	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	sphere->SetBulletPhysicsObject(new btRigidBody(rbInfo));


	world->AddGameObject(sphere);

	return sphere;
}

GameObject* NCL::CSC8503::TutorialGame::CreateBulletCapsule(const Vector3& position, float halfHeight, float radius, float inverseMass)
{
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));

	btCollisionShape* shape = new btCapsuleShape(btScalar(radius), btScalar(halfHeight));

	btTransform bulletTransform;

	bulletTransform.setIdentity();
	bulletTransform.setOrigin(position);

	btScalar mass = 0.0f;
	if (inverseMass != 0.0f)
		mass = (1 / inverseMass);

	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	capsule->SetBulletPhysicsObject(new btRigidBody(rbInfo));

	world->AddGameObject(capsule);

	return capsule;
}

GameObject* NCL::CSC8503::TutorialGame::CreateBulletCylinder(const Vector3& position, float halfHeight, float radius, float inverseMass)
{
	GameObject* cylinder = new GameObject();

	cylinder->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	cylinder->SetRenderObject(new RenderObject(&cylinder->GetTransform(), cylinderMesh, basicTex, basicShader));

	btCollisionShape* shape = new btCylinderShape(btVector3(btScalar(radius), btScalar(halfHeight), btScalar(radius)));

	btTransform bulletTransform;

	bulletTransform.setIdentity();
	bulletTransform.setOrigin(position);

	btScalar mass;
	if (inverseMass != 0.0f)
		mass = (1 / inverseMass);
	else
		mass = (0.0f);

	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	cylinder->SetBulletPhysicsObject(new btRigidBody(rbInfo));

	world->AddGameObject(cylinder);

	return cylinder;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, bool useBullet) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			int randVal = rand() % 4;
			if (randVal == 0) {

				AddCubeToWorld(position, cubeDims);
			}
			else if (randVal == 1) {

				AddSphereToWorld(position, sphereRadius);
			}
			else if (randVal == 2)
			{

				AddCapsuleToWorld(position, sphereRadius * 2, sphereRadius);
			}
			else if (randVal == 3)
			{
				if (useBullet)
					CreateBulletCylinder(position, sphereRadius * 2, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols + 1; ++x) {
		for (int z = 1; z < numRows + 1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitDefaultFloor(bool useBullet) {

	AddFloorToWorld(Vector3(0, -2, 0));
}

void NCL::CSC8503::TutorialGame::InstantiateCharaters()
{
	player = AddPlayerToWorld(playerOrigin.GetPosition());
	player->GetTransform().SetOrientation(playerOrigin.GetOrientation());
	player->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
	player->SetHUD(new DW_UIHUD("../../Assets/Textures/HUD1.png", Vector2(1, 1), Vector3(0, 4.5, 0)));
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = AddCharacterToWorld(position, charMeshB, nullptr, basicShader, "player");
	character->SetLayer(GameObject::Layer::Player);


	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = AddCharacterToWorld(position, enemyMesh, nullptr, basicShader, "enemy");
	character->SetLayer(GameObject::Layer::Enemy);

	return character;
}

GameObject* NCL::CSC8503::TutorialGame::AddCharacterToWorld(const Vector3& position, OGLMesh* mesh, OGLTexture* texture, OGLShader* shader, string name)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new PlayerObject();

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), mesh, texture, shader));

	if (physics->isUseBulletPhysics())
	{
		btCollisionShape* shape = new btCapsuleShape(btScalar(0.4f * meshSize), btScalar(0.9f * meshSize));

		//Initialize bullet inner transfrom(have no scale), you can set it from gameworld transform directly or create new bulletTransfrom
		btTransform bulletTransform = character->GetTransform();

		//caculate the mass of object, bullet object use mass instead of inverse mass for initialing
		btScalar mass = 60.0f;

		//if mass != zero, we shall caculate the local inertia by calling bullet api
		bool isDynamic = (mass != 0.0f);
		btVector3 localInertia(0, 0, 0);

		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//init motionstate from bullet transform
		btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

		//init bullet rigid body
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

		//give gameobject the new rigidbody, it will automaticlly add it to bullet world for simulation later
		character->SetBulletPhysicsObject(new btRigidBody(rbInfo));

		//limit character angular motion
		character->GetBulletBody()->setAngularFactor(btVector3(0, 0, 0));

	}
	else
	{
		AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);
		character->SetBoundingVolume((CollisionVolume*)volume);
		character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

		character->GetPhysicsObject()->SetInverseMass(inverseMass);
		character->GetPhysicsObject()->InitSphereInertia();

		character->GetPhysicsObject()->SetElasticity(0.1f);
	}

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {

	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(1.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, basicTex, basicShader));


	if (physics->isUseBulletPhysics())
	{
		btCollisionShape* shape = new btSphereShape(btScalar(1.25f));

		//Initialize bullet inner transfrom(have no scale), you can set it from gameworld transform directly or create new bulletTransfrom
		btTransform bulletTransform = apple->GetTransform();

		//caculate the mass of object, bullet object use mass instead of inverse mass for initialing
		btScalar mass = 0.1f;

		//if mass != zero, we shall caculate the local inertia by calling bullet api
		bool isDynamic = (mass != 0.0f);
		btVector3 localInertia(0, 0, 0);

		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//init motionstate from bullet transform
		btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

		//init bullet rigid body
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

		//give gameobject the new rigidbody, it will automaticlly add it to bullet world for simulation later
		apple->SetBulletPhysicsObject(new btRigidBody(rbInfo));
		apple->GetBulletBody()->setLinearFactor(btVector3(0, 0, 0));
		apple->GetBulletBody()->setAngularFactor(btVector3(0, 0, 0));
		//apple->SetIsKinematic(true);
	}
	else
	{
		SphereVolume* volume = new SphereVolume(1.5f);
		apple->SetBoundingVolume((CollisionVolume*)volume);

		apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

		apple->GetPhysicsObject()->SetInverseMass(0.0f);
		apple->GetPhysicsObject()->InitSphereInertia();

		apple->GetPhysicsObject()->SetTrigger(true);
	}

	world->AddGameObject(apple);
	return apple;
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject() {

	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
		if (selectionObject) {	//set colour to deselected;
			selectionObject->GetRenderObject()->SetColour(prevColor);
			selectionObject = nullptr;

		}

		//Raycasting is a bit same as before, you should build ray first
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());


		//you will get RayCollision for raycasting output as well as before
		RayCollision closestCollision;
		if (physics->isUseBulletPhysics() && PhysicsSystem::Raycast(ray, closestCollision, true, 300)) {

			selectionObject = world->GetGameObjectByBulletBody((btCollisionObject*)closestCollision.node);
			prevColor = selectionObject->GetRenderObject()->GetColour();
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

			return true;
		}
		else if (!physics->isUseBulletPhysics() && world->Raycast(ray, closestCollision, true))
		{
			selectionObject = (GameObject*)closestCollision.node;
			prevColor = selectionObject->GetRenderObject()->GetColour();
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() {

	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;// we haven ��t selected anything !
	}

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(
			*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject() ->
					AddForce(ray.GetDirection() * forceMagnitude);

			}

		}
		if (closestCollision.node == selectionObject) {
			selectionObject->GetPhysicsObject()->AddForceAtPosition(
				ray.GetDirection() * forceMagnitude,
				closestCollision.collidedAt);

		}
	}


}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position, const Vector3& dimensions, bool switchD, bool perpendicular) {

	StateGameObject* newStateObject = new StateGameObject("State Cube", switchD, perpendicular);


	newStateObject->GetTransform()
		.SetScale(dimensions * 2)
		.SetPosition(position);

	newStateObject->SetRenderObject(new RenderObject(&newStateObject->GetTransform(), cubeMesh, basicTex, basicShader));

	if (physics->isUseBulletPhysics())
	{
		btCollisionShape* shape = new btBoxShape(dimensions);

		//Initialize bullet inner transfrom(have no scale), you can set it from gameworld transform directly or create new bulletTransfrom
		btTransform bulletTransform = newStateObject->GetTransform();

		//caculate the mass of object, bullet object use mass instead of inverse mass for initialing
		btScalar mass = 10.0f;

		//if mass != zero, we shall caculate the local inertia by calling bullet api
		bool isDynamic = (mass != 0.0f);
		btVector3 localInertia(0, 0, 0);

		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//init motionstate from bullet transform
		btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

		//init bullet rigid body
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

		//give gameobject the new rigidbody, it will automaticlly add it to bullet world for simulation later
		newStateObject->SetBulletPhysicsObject(new btRigidBody(rbInfo));

		//todo
		newStateObject->GetBulletBody()->setLinearFactor(btVector3(0, 0, 0));
		newStateObject->GetBulletBody()->setAngularFactor(btVector3(0, 0, 0));
	}
	else
	{
		AABBVolume* volume = new AABBVolume(dimensions);
		newStateObject->SetBoundingVolume((CollisionVolume*)volume);

		newStateObject->SetPhysicsObject(new PhysicsObject(&newStateObject->GetTransform(), newStateObject->GetBoundingVolume()));

		newStateObject->GetPhysicsObject()->SetInverseMass(0.0f);
	}


	newStateObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));

	world->AddGameObject(newStateObject);

	return newStateObject;
}

GameObject* TutorialGame::AddWallToWorld(const Vector3& position, int x, int y, int z, OGLTexture* tempTex, string name) { //lv2 design
	GameObject* wall = new GameObject(name);
	Vector3 wallSize = Vector3(x, y, z);

	wall->GetTransform().SetScale(wallSize * 2).SetPosition(position);
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, tempTex, basicShader));

	if (physics->isUseBulletPhysics())
	{
		btCollisionShape* shape = new btBoxShape(wallSize);

		//Initialize bullet inner transfrom(have no scale), you can set it from gameworld transform directly or create new bulletTransfrom
		btTransform bulletTransform = wall->GetTransform();

		//caculate the mass of object, bullet object use mass instead of inverse mass for initialing
		btScalar mass = 0.0f;

		//if mass != zero, we shall caculate the local inertia by calling bullet api
		bool isDynamic = (mass != 0.0f);
		btVector3 localInertia(0, 0, 0);

		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//init motionstate from bullet transform
		btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

		//init bullet rigid body
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

		//give gameobject the new rigidbody, it will automaticlly add it to bullet world for simulation later
		wall->SetBulletPhysicsObject(new btRigidBody(rbInfo));
	}
	else
	{
		AABBVolume* volume = new AABBVolume(wallSize);
		wall->SetBoundingVolume((CollisionVolume*)volume);
		wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
		wall->GetPhysicsObject()->SetInverseMass(0);
		wall->GetPhysicsObject()->InitCubeInertia();
	}
	world->AddGameObject(wall);
	return wall;
}

GameObject* TutorialGame::AddDoorToWorld(const Vector3& position, Vector3 dimensions, OGLTexture* tempTex, string name, float inverseMass) {
	GameObject* cube = new GameObject(name);

	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, tempTex, basicShader));

	if (physics->isUseBulletPhysics())
	{
		btCollisionShape* shape = new btBoxShape(dimensions);

		//Initialize bullet inner transfrom(have no scale), you can set it from gameworld transform directly or create new bulletTransfrom
		btTransform bulletTransform = cube->GetTransform();

		//caculate the mass of object, bullet object use mass instead of inverse mass for initialing
		btScalar mass = 0.0f;
		if (inverseMass != 0.0f)
			mass = (1 / inverseMass);

		//if mass != zero, we shall caculate the local inertia by calling bullet api
		bool isDynamic = (mass != 0.0f);
		btVector3 localInertia(0, 0, 0);

		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//init motionstate from bullet transform
		btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

		//init bullet rigid body
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

		//give gameobject the new rigidbody, it will automaticlly add it to bullet world for simulation later
		cube->SetBulletPhysicsObject(new btRigidBody(rbInfo));
	}
	else
	{
		AABBVolume* volume = new AABBVolume(dimensions);

		cube->SetBoundingVolume((CollisionVolume*)volume);
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitCubeInertia();
	}

	world->AddGameObject(cube);

	return cube;
}

//AI
StateAi* TutorialGame::AddAiEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	StateAi* character = new StateAi();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);
	enemy = character;
	return character;
}

void TutorialGame::InitAiEnemy1() {
	testEnemy = AddAiEnemyToWorld(Vector3(-150, 10, 0));
}