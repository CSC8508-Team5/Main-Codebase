#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include <list>
#include <algorithm>
#include <math.h>

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	//physics engine started here
	physics = new PhysicsSystem(*world, true);
	//irrklang audio system
	audio = new AudioSystem();

	//global play 2D as background music
	audio->PlayAudio("Casual Theme #1 (Looped).ogg");
	//global play 3D
	//audio->PlayAudio("Casual Theme #1 (Looped).ogg", Vector3(0, 0, 0));
	//other format
	//audio->PlayAudio("Funky Chill 2 loop.flac");
	//audio->PlayAudio("keyboardcat.mp3");

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	//adding for level design
	platformtimer = 0.0f;
	platforms = new GameObject*[17];
	coins = new GameObject * [25];
	spinplat = new GameObject;
	player = new GameObject;
	yaw = 0.0f;
	pitch = 0.0f;
	isjump = false;
	isfinish = false;
	numstairs = 14;
	numcoins = 25; // Upper limit of coins
	coincollected = 0;
	//end 
	
	Debug::SetRenderer(renderer);

	InitialiseAssets();
	//-----------------------------------------------------Ui-----------------------------------------------------------------------//

		//StartMenu =new HM_StartMenu(); // main menu
		//PauseMenu = new HM_PauseMenu(); // Pause menu
		//WinScreen = new HM_Win(); // wining screen
		//LoseScreen = new HM_Lose(); // lose screen

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

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
	
	InitWorld();
	InitCamera();
	
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
	delete spinplat;
	delete player;

	delete physics;
	delete renderer;
	delete world;
	delete audio;
}

void TutorialGame::UpdateGame(float dt) {
	/*if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}*/

	UpdateKeys();

	if (isfinish) {
		Debug::Print("You Win", Vector2(45, 25));
	}

	/*if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}*/

	//SelectObject();
	//MoveSelectedObject();
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

		//Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}
	if (testStateObject) {
		testStateObject->Update(dt);

	}

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);

	DW_UIRenderer::get_instance().Update(dt);

	audio->Update(*world->GetMainCamera());

	CollisionDetection::CollisionInfo info;

	UpdateLevelOne();
	UpdateCoins();
	UpdatePlayer(dt);

	UpdateSpinningPlatform();
	renderer->Render();
}

void TutorialGame::UpdateLevelOne() {
	float speed = 30.0f;
		for (int i = 1; i < numstairs-1; ++i) {
		Vector3 position = platforms[i]->GetTransform().GetPosition();
		if (i % 3 == 1) {
			if (position.z <= -60) {
				//platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, speed));
				platforms[i]->GetBulletBody()->setActivationState(true);
				platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, speed));
			}
			else if (position.z >=10) {
				//platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, -speed));
				platforms[i]->GetBulletBody()->setActivationState(true);
				platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, -speed));
			}
		}
		else if (i % 3 == 0) {
			if(position.z <=-10) {
				//platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, speed));
				platforms[i]->GetBulletBody()->setActivationState(true);
				platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, speed));
			}
			else if (position.z >= 60) {
				//platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, -speed));
				platforms[i]->GetBulletBody()->setActivationState(true);
				platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, -speed));
			}
		}
	}
		CollisionDetection::CollisionInfo info;
		for (int i = 0; i < numstairs; ++i) {
			if (CollisionDetection::ObjectIntersection(player, platforms[i], info)) {
				player->GetPhysicsObject()->SetLinearVelocity(platforms[i]->GetPhysicsObject()->GetLinearVelocity());
				isjump = false;
			}
			//finish
			if (CollisionDetection::ObjectIntersection(player, platforms[numstairs - 1], info)) {
				isfinish = true;
			}
		}
};

void TutorialGame::UpdateSpinningPlatform(){
	//spinplat->GetPhysicsObject()->SetAngularVelocity(Vector3(0.0f, 2.0f, 0.0f));
	spinplat->GetBulletBody()->setAngularVelocity(Vector3(0.0f, 2.0f, 0.0f));
	CollisionDetection::CollisionInfo info;
	if (CollisionDetection::ObjectIntersection(player, spinplat, info)) {
		//player->GetPhysicsObject()->SetAngularVelocity(spinplat->GetPhysicsObject()->GetAngularVelocity());
		isjump = false;
	}
}

void TutorialGame::UpdateCoins() {
	SphereVolume* volume = new SphereVolume(0.0f);
	Debug::Print("Number of Coins collected: " + std::to_string(coincollected), Vector2(10, 20),Vector4(0, 1, 1, 1));
	for (int i = 0; i < numcoins; ++i) {
		if (coins[i] != nullptr) {
			//coins[i]->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 2, 0));
			coins[i]->GetBulletBody()->setAngularVelocity(Vector3(0, 2, 0));
			CollisionDetection::CollisionInfo info;
			if (CollisionDetection::ObjectIntersection(player, coins[i], info)) {
				coincollected += 1;
				coins[i]->SetBoundingVolume((CollisionVolume*)volume);
				coins[i]->GetTransform().SetScale(Vector3(0, 0, 0));
			}
		}
	}
}

void TutorialGame::UpdatePlayer(float dt) {
	Vector3 playerposition = player->GetTransform().GetPosition();
	Quaternion playerorientation = player->GetTransform().GetOrientation();
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	if (pitch < -90.0f) {
		pitch = -90.0f;
	}
	if (pitch > 90.0f) {
		pitch =90.0f;
	}
	float frameSpeed = 50 * dt;
	float cameraSpeed = 25 * dt;
	//player turns head
	Quaternion orientation = player->GetTransform().GetOrientation();
	double turnsin, turncos;
	turnsin = sin((3.1415927 / 2) * ((yaw/10-45)/2));
	turncos = cos((3.1415927 / 2) * ((yaw/10-45)/ 2));
	orientation = Quaternion(0,turnsin,0,turncos);
	orientation.Normalise();
	player->GetTransform().SetOrientation(orientation);
	if (playerposition.y <= -5) {
		InitCharaters();
	}

	float playerMoveSpeed = 10.0f;
	btVector3 currentSpeed = player->GetBulletBody()->getLinearVelocity();

	//player movement
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		currentSpeed += player->GetTransform().Forward().Normalised() * playerMoveSpeed*dt;
		//player->GetBulletBody()->applyCentralForce(player->GetTransform().Forward().Normalised() * 600.0f);
		//player->GetBulletBody()->setLinearVelocity(player->GetTransform().Forward().Normalised()* playerMoveSpeed);
		//playerposition -= Matrix4::Rotation(yaw * 10, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
		//player->GetTransform().SetPosition(playerposition);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		currentSpeed += player->GetTransform().Forward().Normalised() * -playerMoveSpeed*dt;
		//player->GetBulletBody()->setLinearVelocity(player->GetTransform().Forward().Normalised() * -playerMoveSpeed);
		//playerposition += Matrix4::Rotation(yaw * 10, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
		//player->GetTransform().SetPosition(playerposition);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		currentSpeed += player->GetTransform().Right().Normalised() * playerMoveSpeed *dt;
		//player->GetBulletBody()->setLinearVelocity(player->GetTransform().Right().Normalised() * playerMoveSpeed);
		//playerposition -= Matrix4::Rotation(yaw * 10, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		//player->GetTransform().SetPosition(playerposition);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		currentSpeed += player->GetTransform().Right().Normalised() * -playerMoveSpeed *dt;
		//player->GetBulletBody()->setLinearVelocity(player->GetTransform().Right().Normalised() * -playerMoveSpeed);
		//playerposition += Matrix4::Rotation(yaw * 10, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		//player->GetTransform().SetPosition(playerposition);
	}

	//jump
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		float jumpHeight = 5.0f;
		//if(!isjump){
			float jumpSpeed = sqrt(-2.0f * -9.8f * jumpHeight);
			//player->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 20, 0))
			currentSpeed += Vector3::Up() * jumpSpeed;
			//player->GetBulletBody()->setLinearVelocity(Vector3(0, 20, 0));
			isjump = true;
		//}

	}
	else{
		//player->GetPhysicsObject()->AddForce(Vector3(0, -100, 0)); 
		//player->GetBulletBody()->applyCentralForce(Vector3(0, -100, 0));
	}
	player->GetBulletBody()->setActivationState(true);
	player->GetBulletBody()->setLinearVelocity(currentSpeed);
	//player->GetBulletBody()->applyCentralForce(currentSpeed);


	// Third person Camera
	const float Deg2Rad = 3.1415926f / 180.0f;
	float cameraYOffset = lockedDistance * sin(-pitch * Deg2Rad);
	Vector3 camerTargetPos = playerposition + Vector3(0, cameraYOffset, 0) + player->GetTransform().Backward().Normalised() * lockedDistance;
	Matrix4 mat = Matrix4::BuildViewMatrix(camerTargetPos, playerposition, Vector3(0, 1, 0));
	Matrix4 modelMat = mat.Inverse();

	Quaternion q(modelMat);
	Vector3 angles = q.ToEuler(); //nearly there now!

	world->GetMainCamera()->SetPosition(camerTargetPos+Vector3(0,3,0));//Adding a distance on Y
	world->GetMainCamera()->SetPitch(angles.x);
	world->GetMainCamera()->SetYaw(angles.y);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}*/
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

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
		PauseMenu->SetPanelActive(true);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 1.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		if (physics->isUseBulletPhysics())
		{
			selectionObject->GetBulletBody()->activate();
			selectionObject->GetBulletBody()->applyCentralImpulse(-rightAxis * force);
		}
		else
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		if (physics->isUseBulletPhysics())
		{
			selectionObject->GetBulletBody()->activate();
			selectionObject->GetBulletBody()->applyCentralImpulse(rightAxis * force);
		}
		else
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		if (physics->isUseBulletPhysics())
		{
			selectionObject->GetBulletBody()->activate();
			selectionObject->GetBulletBody()->applyCentralImpulse(fwdAxis * force);
		}
		else
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		if (physics->isUseBulletPhysics())
		{
			selectionObject->GetBulletBody()->activate();
			selectionObject->GetBulletBody()->applyCentralImpulse(-fwdAxis * force);
		}
		else
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		if (physics->isUseBulletPhysics())
		{
			selectionObject->GetBulletBody()->activate();
			selectionObject->GetBulletBody()->applyCentralImpulse(Vector3(0, -10, 0));
		}
		else
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}

void TutorialGame::DebugObjectMovement() {
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			if (physics->isUseBulletPhysics())
			{
				selectionObject->GetBulletBody()->activate();
				selectionObject->GetBulletBody()->applyCentralForce(Vector3(-10, 0, 0));
			}
			else
				selectionObject->GetPhysicsObject()->AddForce(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			if (physics->isUseBulletPhysics())
			{
				selectionObject->GetBulletBody()->activate();
				selectionObject->GetBulletBody()->applyCentralForce(Vector3(10, 0, 0));
			}
			else
				selectionObject->GetPhysicsObject()->AddForce(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			if (physics->isUseBulletPhysics())
			{
				selectionObject->GetBulletBody()->activate();
				selectionObject->GetBulletBody()->applyTorque(Vector3(0, 10, 0));
			}
			else
				selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			if (physics->isUseBulletPhysics())
			{
				selectionObject->GetBulletBody()->activate();
				selectionObject->GetBulletBody()->applyTorque(Vector3(0, -10, 0));
			}
			else
				selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			if (physics->isUseBulletPhysics())
			{
				selectionObject->GetBulletBody()->activate();
				selectionObject->GetBulletBody()->applyCentralForce(Vector3(0, 0, -10));
			}
			else
				selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			if (physics->isUseBulletPhysics())
			{
				selectionObject->GetBulletBody()->activate();
				selectionObject->GetBulletBody()->applyCentralForce(Vector3(0, 0, 10));
			}
			else
				selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			if (physics->isUseBulletPhysics())
			{
				selectionObject->GetBulletBody()->activate();
				selectionObject->GetBulletBody()->applyCentralForce(Vector3(0, -10, 0));
			}
			else
				selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}

	}

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-5.0f);
	world->GetMainCamera()->SetYaw(270.0f);
	world->GetMainCamera()->SetPosition(player->GetTransform().GetPosition()+Vector3(-15,5,0));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	physics->UseGravity(useGravity);
	world->ClearAndErase();
	physics->Clear();
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));
	//InitMixedGridWorld(5, 5, 3.5f, 3.5f);
	InitCharaters();
	//InitDefaultFloor();
	//BridgeConstraintTest();
	platforms = LevelTestOne();
	//Pendulum();
	spinplat = 	SpinningPlatform();
	coincollected = 0;
}

GameObject** TutorialGame::LevelTestOne() {
	Vector3 PlatformSize = Vector3(10, 5, 50);
	Vector3 cubeSize = Vector3(10, 5, 10);
	Vector3 middlecubeSize = Vector3(10, 5, 20);

	float invCubeMass = 200000.0f; // how heavy the middle pieces are
	float cubeDistance = 20; // distance between links
	
	Vector3 startPos = Vector3(-150, 5, 0);

	platforms[0] = AddCubeToWorld(startPos + Vector3(0, 0, 0), PlatformSize, 0);
	platforms[numstairs-1]= AddCubeToWorld(startPos + Vector3((numstairs-1) * cubeDistance, (numstairs-1)* 5.0f, 0), PlatformSize, 0);
	platforms[0]->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
	platforms[numstairs - 1]->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	//initial coins
	for (int i = 0; i < numcoins; ++i) {
		coins[i] = nullptr;
	}

	for (int i = 1; i < numstairs-1; ++i) {
		if (i % 3 == 1) {
			platforms[i] = AddCubeToWorld(startPos + Vector3(i * cubeDistance, i * 5.0f, -40), cubeSize, invCubeMass);

			platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, 30));
			platforms[i]->GetBulletBody()->setActivationState(true);
			platforms[i]->GetBulletBody()->setAngularFactor(Vector3::Zero());
			platforms[i]->GetBulletBody()->setLinearFactor(Vector3::Zero());
			platforms[i]->GetBulletBody()->setGravity(Vector3::Zero());
			//platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0,30));
			platforms[i]->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
			coins[i] = AddCoins(startPos + Vector3(i * cubeDistance, (i+1)* 5.0f+3, -20));
		}
		else if (i % 3 == 2) {
			platforms[i] = AddCubeToWorld(startPos + Vector3(i* cubeDistance, i * 5.0f, 0), middlecubeSize, 0.0f);
			platforms[i]->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
		}
		else if(i % 3 == 0) {
			platforms[i] = AddCubeToWorld(startPos + Vector3(i* cubeDistance, i * 5.0f, 40), cubeSize, invCubeMass);
			platforms[i]->GetBulletBody()->setLinearVelocity(Vector3(0, 0, -30));
			//platforms[i]->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, -30));
			platforms[i]->GetBulletBody()->setActivationState(true);
			platforms[i]->GetBulletBody()->setAngularFactor(Vector3::Zero());
			platforms[i]->GetBulletBody()->setLinearFactor(Vector3::Zero());
			platforms[i]->GetBulletBody()->setGravity(Vector3::Zero());
			platforms[i]->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
			coins[i] = AddCoins(startPos + Vector3(i * cubeDistance, (i+1) * 5.0f+3, 20));
		}
	}
	return platforms;
}

GameObject* TutorialGame::AddCoins(const Vector3& position) {//No more than 25 coins
	GameObject* coin = new GameObject();
	coin = AddBonusToWorld(position);
	coin->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
	//coin->SetIsKinematic(false);
	coin->GetBulletBody()->setLinearFactor(btVector3(0, 0, 0));
	coin->GetBulletBody()->setAngularFactor(btVector3(0, 1, 0));
	return coin;
}

GameObject* TutorialGame::SpinningPlatform() {
	float radius = 30.0f;
	float hight = 2.5f;
	Vector3 position = Vector3(-150, 5, -80);
	GameObject* spinplat = AddCylinderToWorld(position, radius, hight, 1.0f);
	spinplat->GetBulletBody()->setGravity(Vector3::Zero());
	spinplat->GetBulletBody()->setLinearFactor(Vector3::Zero());
	spinplat->GetBulletBody()->setAngularFactor(Vector3::Zero());
	spinplat->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	return spinplat;
}

void TutorialGame::Pendulum() {
	float sphereradius = 5.0f;
	float cylinderhalfhight = 10.0f;
	Vector3 cylinderposition = Vector3(10, 20, 10);
	Vector3 sphereposition = cylinderposition - Vector3(0, 10 , 0);
	GameObject* sphere = AddSphereToWorld(sphereposition, sphereradius, 0.0f);
	GameObject* cylinder = AddCylinderToWorld(cylinderposition, 0.5f, cylinderhalfhight, 0.0f);

	/*PositionConstraint* constraint = new PositionConstraint(sphere,
		cylinder, 0.0f);
	world->AddConstraint(constraint);*/
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
		* cubeDistance+8, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	//relative transform of objects
	btTransform frameInA;
	btTransform frameInB;

	frameInA.setIdentity();
	frameInA.setOrigin(btVector3(cubeDistance /2, 0, 0));
	frameInB.setIdentity();
	frameInB.setOrigin(btVector3(-cubeDistance /2, 0, 0));

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = CreateBulletCube(startPos + Vector3((i + 1) *
			cubeDistance, 0, 0), cubeSize, invCubeMass);

		//btPoint2PointConstraint()
		//btHingeConstraint* constraint = new btHingeConstraint(*previous->GetBulletBody(), *block->GetBulletBody(), previous->GetTransform().GetPosition(), block->GetTransform().GetPosition(), btVector3(1, 1, 1), btVector3(1, 1, 1));
		//btFixedConstraint* constraint = new btFixedConstraint(*previous->GetBulletBody(), *block->GetBulletBody(), frameInA, frameInB);
		btGeneric6DofSpringConstraint* constraint = new btGeneric6DofSpringConstraint(*previous->GetBulletBody(), *block->GetBulletBody(), frameInA, frameInB,true);
		constraint->setLinearUpperLimit(btVector3(0, 0, 0));
		constraint->setLinearLowerLimit(btVector3(0, 0, 0));
		constraint->setAngularLowerLimit(btVector3(0, -1, -1));
		constraint->setAngularUpperLimit(btVector3(0, 1, 1));
		constraint->enableSpring(3, true);
		constraint->setStiffness(3, 0.01f);

		//previous->GetBulletBody()->addConstraintRef(constraint);
		//block->GetBulletBody()->addConstraintRef(constraint);
		//constraint->setDamping(3, 0.25f);

		//btPoint2PointConstraint* constraint = new btPoint2PointConstraint(*previous->GetBulletBody(), *block->GetBulletBody(), frameInA.getOrigin(), frameInB.getOrigin());
		//btGeneric6DofSpringConstraint* constraint = new btGeneric6DofSpringConstraint();
		//constraint->setLinearLowerLimit(btVector3(-1, -1, 0));
		//constraint->setLinearUpperLimit(btVector3(1, 1, 0));
		//PositionConstraint* constraint = new PositionConstraint(previous,
		//	block, maxDistance);
		
		physics->AddConstraint(constraint);
		previous = block;

	}

	//btPoint2PointConstraint* constraint = new btPoint2PointConstraint(*previous->GetBulletBody(), *end->GetBulletBody(), frameInA.getOrigin(), frameInB.getOrigin());
	btGeneric6DofSpringConstraint* constraint = new btGeneric6DofSpringConstraint(*previous->GetBulletBody(), *end->GetBulletBody(), frameInA, frameInB, true);

	//PositionConstraint* constraint = new PositionConstraint(previous,
	//	end, maxDistance);
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

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
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
	//floor->SetIsKinematic(true);
	btCollisionShape* shape = new btBoxShape(floorSize);
	btTransform bulletTransform = floor->GetTransform();


	btScalar mass = 0.0f;

	btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape);

	//floor->DisableLayerMask(GameObject::Layer::Default);

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
	//cube->SetIsKinematic(true);

	//Initialize shape of the collision object
	btCollisionShape* shape = new btBoxShape(dimensions);

	//Initialize bullet inner transfrom(have no scale), you can set it from gameworld transform directly or create new bulletTransfrom
	btTransform bulletTransform = cube->GetTransform();
	//btTransform bulletTransform;
	//bulletTransform.setIdentity();
	//bulletTransform.setOrigin(position);

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

	//cube->GetBulletBody()->setLinearFactor(btVector3(0, 0, 0));
	//cube->GetBulletBody()->setAngularFactor(Vector3::Zero());
	//add the gameobject to the world
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

	//bulletTransform.setIdentity();
	//bulletTransform.setOrigin(position);

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
	
	//sphere->SetIsKinematic(true);

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
				/*if (useBullet)
					CreateBulletCube(position, cubeDims);
				else*/
					AddCubeToWorld(position, cubeDims);
			}
			else if (randVal == 1) {
				/*if (useBullet)
					CreateBulletSphere(position, sphereRadius);
				else*/
					AddSphereToWorld(position, sphereRadius);
			}
			else if (randVal == 2)
			{
				/*if (useBullet)
					CreateBulletCapsule(position, sphereRadius * 2, sphereRadius);
				else*/
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
	if (useBullet)
		CreateBulletFloor(Vector3(0, -2, 0));
	else
		AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitCharaters() {
	player = AddPlayerToWorld(Vector3(-150, 10, 0));
	/*Quaternion orientation = Quaternion(0, -1, 0, 1);
	orientation.Normalise();
	player->GetTransform().SetOrientation(orientation);*/
	player->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
	/*AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));*/
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {

	std::string str{ NCL::Assets::TEXTUREDIR + "doge.png" };
	DW_UIHUD* hud = new DW_UIHUD(str.c_str(), Vector2{ 3.0f,1.0f }, Vector3{ 0.0f,4.0f ,0.0f });

	GameObject* character = AddCharacterToWorld(position, charMeshA, nullptr, basicShader, "player");

	character->SetHUD(hud);

	/*AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);
	//Don't need random player now.
	/*if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}*/
	//character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	//character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	//character->GetPhysicsObject()->SetInverseMass(inverseMass);
	//character->GetPhysicsObject()->InitSphereInertia();

	//world->AddGameObject(character);

	//lockedObject = character;*/

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = AddCharacterToWorld(position, enemyMesh, nullptr, basicShader, "enemy");
		/*
	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);*/

	return character;
}

GameObject* NCL::CSC8503::TutorialGame::AddCharacterToWorld(const Vector3& position, OGLMesh* mesh, OGLTexture* texture, OGLShader* shader, string name)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject(name);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), mesh, texture, shader));

	if (physics->isUseBulletPhysics())
	{
		btCollisionShape* shape = new btCapsuleShape(btScalar(0.4f*meshSize), btScalar(0.9f * meshSize));

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
		character->GetBulletBody()->setAngularFactor(btVector3(0, 1, 0));
		
		character->SetIsKinematic(false);

	}
	else
	{
		AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);
		character->SetBoundingVolume((CollisionVolume*)volume);
		character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

		character->GetPhysicsObject()->SetInverseMass(inverseMass);
		character->GetPhysicsObject()->InitSphereInertia();
	}
	
	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {

	GameObject* apple = new GameObject();

	btScalar radius = 0.25f;

	apple->GetTransform()
		.SetScale(Vector3(radius, radius, radius))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, basicTex, basicShader));

	//

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
		
		//apple->SetIsKinematic(true);
	}
	else
	{
		SphereVolume* volume = new SphereVolume(radius);
		apple->SetBoundingVolume((CollisionVolume*)volume);

		apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

		apple->GetPhysicsObject()->SetInverseMass(1.0f);
		apple->GetPhysicsObject()->InitSphereInertia();
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
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject = nullptr;
			}

			//Raycasting is a bit same as before, you should build ray first
			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			/*
			//you can also form the raycasting in the other way
			btVector3 from = ray.GetPosition();
			btVector3 to = ray.GetPosition() + ray.GetDirection().Normalised() * 200;

			btCollisionWorld::ClosestRayResultCallback rcb(from, to);
			//btCollisionWorld::AllHitsRayResultCallback rcb(from, to);
			PhysicsSystem::Raycast(from, to, rcb);
			*/

			//you will get RayCollision for raycasting output as well as before
			RayCollision closestCollision;
			//if (world->Raycast(ray, closestCollision, true)) {
			if (PhysicsSystem::Raycast(ray, closestCollision, true, 300)) {
				//if (rcb.hasHit()) {
					//selectionObject = (GameObject*)closestCollision.node;
					//std::cout << "Hit normal" << rcb.m_hitNormalWorld << "\n";
					//selectionObject = world->GetGameObjectByBulletBody(rcb.m_collisionObject);
				selectionObject = world->GetGameObjectByBulletBody((btCollisionObject*)closestCollision.node);
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if (selectionObject) {
		renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
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
	/*renderer->DrawString(" Click Force :" + std::to_string(forceMagnitude),
		Vector2(10, 20)); // Draw debug text at 10 ,20*/
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;// we haven ��t selected anything !
	}
	// Push the selected object !
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

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {

	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}