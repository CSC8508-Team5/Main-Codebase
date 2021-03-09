#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "StateGameObject.h"
#include "StateAi.h"
namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame(string name="");
			~TutorialGame();

			virtual void UpdateGame(float dt);
			virtual void UpdateIntroGame(float dt);
			int GetScore() {
				return fenshu;
			}
			bool GetFinal() {
				return testfinalpoint;
			}
			int GetOpenOrExit() {
				return OpenOrExit;
			}
			void SetOpenOrExit(int openorexit) {
				OpenOrExit = openorexit;
			}
		protected:
			//intro page
			void InitIntroWorld();
			void InitIntroCamera();
			void InitOpenCube();
			void InitExitCube();
			bool IntroSelectObject();

			void InitAiEnemy1();
			StateAi* AddAiEnemyToWorld(const Vector3& position);
			StateAi* testEnemy = nullptr;
			int checkAitime = 0;
			int checkAitime1 = 0;
			int checknum = 1;


			GameObject* OpenCube = nullptr;
			GameObject* ExitCube = nullptr;
			int OpenOrExit = 0;



			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

			void InitCoins();


			void InitDefaultFloor();
			void BridgeConstraintTest();
			void BridgeConstraintTest1();
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions, float inverseMass=0.0f);
			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);


		StateGameObject* AddStateObjectToWorld(const Vector3& position, float radius, float inverseMass);
		  StateGameObject * testStateObject = nullptr;
		  StateGameObject* testStateObject1 = nullptr;
		  /*StateGameObject* testStateObject2 = nullptr;*/
			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;
			GameTimer  gametime;
			bool useGravity;
			bool inSelectionMode;
			int fenshu = 1000;
			int nowtime;
			int nexttime;
			int num = 1;
			int testfinalpoint;
			string name;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			GameObject* player = nullptr;
			GameObject* enemy = nullptr;

			GameObject* coins = nullptr;
			GameObject* groupcoins[20]{};

			Vector4 nowcolor;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

		};
	}
}

