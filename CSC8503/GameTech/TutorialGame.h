#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/AudioSystem.h"
#include "StateGameObject.h"

#include "DW_UIRenderer.h"

#include "HM_StartMenu.h"
#include "HM_PauseMenu.h"
#include "HM_Win.h"
#include "HM_Lose.h"
#include "HM_Option.h"
namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitCharaters();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest();
	
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			//adding for level design
			GameObject** LevelTestOne();
			GameObject* SpinningPlatform();
			GameObject* AddCylinderToWorld(const Vector3& position, float radius, float hight, float inverseMass = 10.0f);
			GameObject* AddCoins(const Vector3& position);
			void Pendulum();
			void UpdateLevelOne();
			void UpdateCoins();
			void UpdateSpinningPlatform();
			void UpdatePlayer(float dt);
			//end

			// Conor Level
			void LevelThree();
			void UpdateLevelThree();

		StateGameObject* AddStateObjectToWorld(const Vector3& position);
		  StateGameObject * testStateObject = nullptr;

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;
			AudioSystem*		audio;
			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;
			//adding for level design
			float		platformtimer;
			float		yaw;
			float		pitch;
			bool		isjump;
			bool		isfinish;
			int			numstairs;
			int			numcoins;
			int			coincollected;
			GameObject** platforms;
			GameObject** coins;
			GameObject* spinplat;
			GameObject* player;
			//end

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

			//Teamwork Meshes
			OGLMesh* spinplatMesh = nullptr;
			//end

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			float lockedDistance = 15.0f;
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* audioAgent;

			//DW UI example
			HM_StartMenu* StartMenu;
			HM_PauseMenu* PauseMenu;
			HM_Win* WinScreen;
			HM_Lose* LoseScreen;
			HM_Option* OptionMenu;
		};
	}
}

