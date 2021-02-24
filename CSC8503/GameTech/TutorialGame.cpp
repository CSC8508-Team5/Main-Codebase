#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/OrientationConstraints.h"

#include <list>
using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	//adding for level design
	platformtimer = 0.0f;
	platforms = new GameObject*;
	//end 
	YMax = 0;
	Debug::SetRenderer(renderer);

	InitialiseAssets();
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

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("Male1.msh"	 , &charMeshA);
	loadFunc("courier.msh"	 , &charMeshB);
	loadFunc("security.msh"	 , &enemyMesh);
	loadFunc("coin.msh"		 , &bonusMesh);
	loadFunc("capsule.msh"	 , &capsuleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
	delete[] platforms;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys(dt);

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

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(lockedObject->GetTransform().GetOrientation());
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);

		//Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}
	if (testStateObject) {
		 testStateObject -> Update(dt);
		
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	UpdateLevelOne();
	renderer->Render();
}
void TutorialGame::UpdateLevelOne() {
	for (int i = 0; i < 15; ++i) {
		Vector3 position = platforms[i]->GetTransform().GetPosition();
		if (i % 3 == 0) {
			if ((platformtimer >= 0) && (platformtimer < 100)) {
				platforms[i]->GetTransform().SetPosition(position + Vector3(0, 0, 0.8));
				platformtimer += 0.2;
				continue;
			}
			else if (platformtimer >= 100) {
				platformtimer = 0;
				platformtimer -= 0.1;
				continue;
			}
			else if ((platformtimer <= 0) && (platformtimer > -100)) {
				platforms[i]->GetTransform().SetPosition(position + Vector3(0, 0, -0.8));
				platformtimer -= 0.2;
				continue;
			}
			else if (platformtimer <= -100) {
				platformtimer = 0;
				platformtimer += 0.2;
				continue;
			}
		}
		else if (i % 3 == 2) {
			if ((platformtimer >= 0) && (platformtimer < 100)) {
				platforms[i]->GetTransform().SetPosition(position + Vector3(0, 0, -0.8));
				platformtimer += 0.2;
				continue;
			}
			else if (platformtimer >= 100) {
				platformtimer = 0;
				platformtimer -= 0.2;
				continue;
			}
			else if ((platformtimer <= 0) && (platformtimer > -100)) {
				platforms[i]->GetTransform().SetPosition(position + Vector3(0, 0, 0.8));
				platformtimer -= 0.2;
				continue;
			}
			else if (platformtimer <= -100) {
				platformtimer = 0;
				platformtimer += 0.2;
				continue;
			}
		}
	}
};


void TutorialGame::UpdateKeys(float dt) {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject	= nullptr;
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

	if (lockedObject) {
		LockedObjectMovement(dt);
	}
	//else {
	//	//DebugObjectMovement();
	//}
}

void TutorialGame::LockedObjectMovement(float dt) {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward  = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 100.0f;


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
		if (lockedObject->GetTransform().GetOrientation().ToEuler().y >= -90) {
			YMax = 90;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}
		else {
			YMax = 90;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
		if (lockedObject->GetTransform().GetOrientation().ToEuler().y >= 90) {
			YMax = -90;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}
		else {
			YMax =-90;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
		if (lockedObject->GetTransform().GetOrientation().ToEuler().y <= 0) {
			YMax = -5;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, -7, 0));
		}
		else {
			YMax = 5;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, 7, 0));
		}
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
		if (lockedObject->GetTransform().GetOrientation().ToEuler().y <= 0) {
			YMax = -177;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, -7, 0));
		}
		else {
			YMax = 177;
			lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, 7, 0));
		}



	}
	//world->GetMainCamera()->SetYaw(lockedObject->GetTransform().GetOrientation().y);
	OrientationConstraints constraint = OrientationConstraints(lockedObject, YMax);
	constraint.UpdateConstraint(dt);
}

//void TutorialGame::DebugObjectMovement() {
////If we've selected an object, we can manipulate it with some key presses
//	if (inSelectionMode && selectionObject) {
//		//Twist the selected object!
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
//		}
//
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
//		}
//
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
//		}
//
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
//			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
//		}
//
//		//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
//		//	selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
//		//}
//
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
//			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
//		}
//
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
//			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
//		}
//
//		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
//			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
//		}
//	
//	}
//
//}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();
	platforms = LevelTestOne();
	AddPlayerToWorld(Vector3(-150, 15, -10));
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));
	//InitMixedGridWorld(5, 5, 3.5f, 3.5f);
	//InitGameExamples();
	//InitDefaultFloor();

	//BridgeConstraintTest();
}

GameObject** TutorialGame::LevelTestOne() {
	Vector3 PlatformSize = Vector3(10, 5, 50);
	Vector3 cubeSize = Vector3(10, 5, 10);
	Vector3 middlecubeSize = Vector3(10, 5, 20);

	float invCubeMass = 0; // how heavy the middle pieces are
	int numStairs = 15;
	float cubeDistance = 20; // distance between links
	
	Vector3 startPos = Vector3(-150, 5, 0);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0)
		, PlatformSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numStairs + 1) * cubeDistance, (numStairs + 1) * 5, 0), PlatformSize, 0);

	GameObject* previous = start;
	for (int i = 0; i < numStairs; ++i) {
		if (i % 3 == 0) {
			GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, i * 5, -40), cubeSize, invCubeMass);
			platforms[i] = block;
			previous = block;
		}
		else if (i % 3 == 1) {
			GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, i * 5, 0), middlecubeSize, invCubeMass);
			platforms[i] = block;
			previous = block;
		}
		else if(i % 3 == 2) {
			GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, i * 5, 40), cubeSize, invCubeMass);
			platforms[i] = block;
			previous = block;
		}
	}
	return platforms;
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);
	 float invCubeMass = 5; // how heavy the middle pieces are
	 int numLinks = 10;
	 float maxDistance = 30; // constraint distance
	 float cubeDistance = 20; // distance between links
	
		 Vector3 startPos = Vector3(5, 100, 5);
	
		 GameObject * start = AddCubeToWorld(startPos + Vector3(0, 0, 0)
			, cubeSize, 0);
	 GameObject * end = AddCubeToWorld(startPos + Vector3((numLinks + 2)
		 * cubeDistance, 0, 0), cubeSize, 0);
	
		 GameObject * previous = start;
	
		 for (int i = 0; i < numLinks; ++i) {
		 GameObject * block = AddCubeToWorld(startPos + Vector3((i + 1) *
			 cubeDistance, 0, 0), cubeSize, invCubeMass);
		 PositionConstraint * constraint = new PositionConstraint(previous,
			 block, maxDistance);
		 world -> AddConstraint(constraint);
		 previous = block;
		
	}
	 PositionConstraint * constraint = new PositionConstraint(previous,
		 end, maxDistance);
	 world -> AddConstraint(constraint);
	
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize	= Vector3(200, 2, 200);
	AABBVolume* volume	= new AABBVolume(floorSize);
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
		.SetScale(Vector3(radius* 2, halfHeight, radius * 2))
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


void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

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
	lockedObject = world->getGameObjects().at(world->getGameObjects().size()-1); //Not dangling since charachter will always be added

	//lockedObject = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

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
		.SetScale(Vector3(2.5, 0.25, 0.25))
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
				//lockedObject	= nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
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

	else if(selectionObject){
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