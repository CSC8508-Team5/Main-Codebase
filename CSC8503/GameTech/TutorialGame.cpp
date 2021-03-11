#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
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

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete cylinderMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
	delete audio;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}

	SelectObject();
	MoveSelectedObject();
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

	renderer->Render();
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

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
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
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	physics->Clear();
	world->ClearAndErase();
	testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));
	InitMixedGridWorld(5, 5, 3.5f, 3.5f, physics->isUseBulletPhysics());
	InitGameExamples();

	InitDefaultFloor(physics->isUseBulletPhysics());

	if (physics->isUseBulletPhysics())
		BridgeBulletConstraintTest();
	else
		BridgeConstraintTest();

	//create audio system agent object 
	audioAgent = AddSphereToWorld(Vector3(0, 1, 0), 1);
	audioAgent->GetRenderObject()->SetColour(Debug::BLUE);
	audioAgent->SetSoundSource(new SoundSource(Assets::SFXDIR + "bell.wav", audioAgent->GetTransform().GetPosition()));

	//create Bullet Cube 
	//AddBulletCubeToWorld(Vector3(1, 10, 1), Vector3(1, 1, 1));
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

	GameObject* start = AddBulletCubeToWorld(startPos + Vector3(0, 0, 0)
		, cubeSize, 0);
	GameObject* end = AddBulletCubeToWorld(startPos + Vector3((numLinks + 1)
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
		GameObject* block = AddBulletCubeToWorld(startPos + Vector3((i + 1) *
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

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
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

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
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

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
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

GameObject* NCL::CSC8503::TutorialGame::AddBulletFloorToWorld(const Vector3& position)
{
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(200, 2, 200);

	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));

	btCollisionShape* shape = new btBoxShape(floorSize);
	btTransform bulletTransform;

	bulletTransform.setIdentity();
	bulletTransform.setOrigin(position);

	btScalar mass = 0.0f;

	btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape);

	floor->SetBulletPhysicsObject(new btRigidBody(rbInfo));
	physics->AddBulletBody(floor->GetBulletBody());

	world->AddGameObject(floor);

	return floor;
}

GameObject* NCL::CSC8503::TutorialGame::AddBulletCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject();

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));

	//btVector3 btDimensions(btScalar(dimensions.x), btScalar(dimensions.y), btScalar(dimensions.z));


	//btCollisionShape* shape = nullptr;
	//shape = new btBoxShape(dimensions);
	btCollisionShape* shape = new btBoxShape(dimensions);
	//btBoxShape* shape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
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

	//btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, motionState, shape, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	//btRigidBody* body = new btRigidBody(*rbInfo);
	cube->SetBulletPhysicsObject(new btRigidBody(rbInfo));
	physics->AddBulletBody(cube->GetBulletBody());


	world->AddGameObject(cube);

	return cube;
}

GameObject* NCL::CSC8503::TutorialGame::AddBulletSphereToWorld(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));


	btCollisionShape* shape = new btSphereShape(btScalar(radius));
	//btBoxShape* shape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
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

	//btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, motionState, shape, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	//btRigidBody* body = new btRigidBody(*rbInfo);
	sphere->SetBulletPhysicsObject(new btRigidBody(rbInfo));
	physics->AddBulletBody(sphere->GetBulletBody());


	world->AddGameObject(sphere);

	return sphere;
}

GameObject* NCL::CSC8503::TutorialGame::AddBulletCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass)
{
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));

	btCollisionShape* shape = new btCapsuleShape(btScalar(radius), btScalar(halfHeight));
	//btBoxShape* shape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
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

	//btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, motionState, shape, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	//btRigidBody* body = new btRigidBody(*rbInfo);
	capsule->SetBulletPhysicsObject(new btRigidBody(rbInfo));
	physics->AddBulletBody(capsule->GetBulletBody());

	world->AddGameObject(capsule);

	return capsule;
}



GameObject* NCL::CSC8503::TutorialGame::AddBulletCylinderToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass)
{
	GameObject* cylinder = new GameObject();

	cylinder->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	cylinder->SetRenderObject(new RenderObject(&cylinder->GetTransform(), cylinderMesh, basicTex, basicShader));

	btCollisionShape* shape = new btCylinderShape(btVector3(btScalar(radius), btScalar(halfHeight), btScalar(radius)));
	//btBoxShape* shape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
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

	//btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, motionState, shape, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	//btRigidBody* body = new btRigidBody(*rbInfo);
	cylinder->SetBulletPhysicsObject(new btRigidBody(rbInfo));
	physics->AddBulletBody(cylinder->GetBulletBody());

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
				if (useBullet)
					AddBulletCubeToWorld(position, cubeDims);
				else
					AddCubeToWorld(position, cubeDims);
			}
			else if (randVal == 1) {
				if (useBullet)
					AddBulletSphereToWorld(position, sphereRadius);
				else
					AddSphereToWorld(position, sphereRadius);
			}
			else if (randVal == 2)
			{
				if (useBullet)
					AddBulletCapsuleToWorld(position, sphereRadius * 2, sphereRadius);
				else
					AddCapsuleToWorld(position, sphereRadius * 2, sphereRadius);
			}
			else if (randVal == 3)
			{
				if (useBullet)
					AddBulletCylinderToWorld(position, sphereRadius * 2, sphereRadius);
				//else
				//	AddCapsuleToWorld(position, sphereRadius * 2, sphereRadius);
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
		AddBulletFloorToWorld(Vector3(0, -2, 0));
	else
		AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(-5, 5, -5));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	std::string str{ NCL::Assets::TEXTUREDIR + "doge.png" };
	DW_UIHUD* hud = new DW_UIHUD(str.c_str(), Vector2{ 3.0f,1.0f }, Vector3{ 0.0f,4.0f ,0.0f });

	GameObject* character = new GameObject();
	character->SetHUD(hud);

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

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

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {

	GameObject* apple = new GameObject();

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

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			/*
			btVector3 from = ray.GetPosition();
			btVector3 to = ray.GetPosition() + ray.GetDirection().Normalised() * 200;

			btCollisionWorld::ClosestRayResultCallback rcb(from, to);
			//btCollisionWorld::AllHitsRayResultCallback rcb(from, to);
			PhysicsSystem::Raycast(from, to, rcb);
		*/

			RayCollision closestCollision;
			//if (world->Raycast(ray, closestCollision, true)) {
			if (PhysicsSystem::Raycast(ray, closestCollision, 300)) {
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
	renderer->DrawString(" Click Force :" + std::to_string(forceMagnitude),
		Vector2(10, 20)); // Draw debug text at 10 ,20
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;// we haven ¡¯t selected anything !
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