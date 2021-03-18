#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/AudioSystem.h"
#include "StateGameObject.h"

#include "../../Common/Vector3.h"
#include "../../Common/Assets.h"
#include "DW_UIRenderer.h"
#include "DW_UIText.h"
#include "DW_UIPanel.h"
#include "DW_UIImage.h"
#include "HM_StartMenu.h"
#include "HM_PauseMenu.h"
#include "HM_Win.h"
#include "HM_Lose.h"
#include "HM_Option.h"
namespace NCL {
	namespace CSC8503 {
		class TutorialGame {
		public:
			TutorialGame();
			~TutorialGame();
			bool IfRestart();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitCharaters();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, bool useBullet = false);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor(bool useBullet = false);

			void BridgeConstraintTest();
			void BridgeBulletConstraintTest();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			GameObject* AddCylinderToWorld(const Vector3& position, float radius, float hight, float inverseMass = 10.0f);

			GameObject* CreateFloor(const Vector3& position);
			GameObject* CreateSphere(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* CreateCube(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* CreateCapsule(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			GameObject* CreateCylinder(const Vector3& position, float radius, float hight, float inverseMass = 10.0f);

			GameObject* CreateBulletFloor(const Vector3& position);
			GameObject* CreateBulletCube(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* CreateBulletSphere(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* CreateBulletCapsule(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			GameObject* CreateBulletCylinder(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position,OGLMesh* mesh, OGLTexture* texture, OGLShader* shader,string name="char");
			GameObject* AddBonusToWorld(const Vector3& position);

			//adding for level design
			GameObject** LevelTestOne();
			GameObject* SpinningPlatform();
			
			GameObject* AddCoins(const Vector3& position);
			DW_UIText*	 Coin_text;
			DW_UIText*   Timer_text;
			DW_UIPanel* InGameUI;
			void Pendulum();
			void UpdateLevelOne();
			void UpdateCoins();
			void UpdateSpinningPlatform();
			void UpdatePlayer(float dt);
			void Reload();
			//end

		StateGameObject* AddStateObjectToWorld(const Vector3& position);
		  StateGameObject * testStateObject = nullptr;

			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;
			AudioSystem* audio;
			bool useGravity;
			bool inSelectionMode;


			float		forceMagnitude;
			//adding for level design
			float		platformtimer;
			float		yaw;
			float		pitch;
			bool		isjump;
			bool		isfinish;
			bool		ispause;
			int			numstairs;
			int			numcoins;
			int			coincollected;
			GameObject** platforms;
			GameObject** coins;
			GameObject* spinplat;
			GameObject* player;
			//end

			// Conor Level (Level Three)
			void LevelThree();
			void UpdateLevelThree(float dt);
			bool isLevelThree = false; // Make sure this is changed in final build!
			std::vector<GameObject*> sliderVector;
			GameObject* AddBouncer(const Vector3& position, float radius, float height, float inverseMass = 10.0f);

			GameObject* selectionObject = nullptr;

			OGLMesh* capsuleMesh = nullptr;
			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLMesh* cylinderMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;

			//Coursework Meshes
			OGLMesh* charMeshA = nullptr;
			OGLMesh* charMeshB = nullptr;
			OGLMesh* enemyMesh = nullptr;
			OGLMesh* bonusMesh = nullptr;

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

