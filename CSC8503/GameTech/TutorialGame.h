#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/AudioSystem.h"
#include "StateGameObject.h"
#include "StateAi.h"
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
			int GetCurrentLevel() { return currentLevel; }
			void SetCurrentLevel(int level) { currentLevel = level; }
			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitLevel2();
			void InitLevel2design();

			void InitLevel3();



			//AI

			void InitAiEnemy1();
			StateAi* AddAiEnemyToWorld(const Vector3& position);
			StateAi* testEnemy = nullptr;
			int checkAitime = 0;
			int checkAitime1 = 0;
			int checknum = 1;





			void InitLevel1();

			void InitCharaters(Vector3 position);

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
			GameObject* AddCharacterToWorld(const Vector3& position, OGLMesh* mesh, OGLTexture* texture, OGLShader* shader, string name = "char");
			GameObject* AddBonusToWorld(const Vector3& position);

			GameObject* AddWallToWorld(const Vector3& position, int x, int y, int z, OGLTexture* tempTex, string name);
			GameObject* AddDoorToWorld(const Vector3& position, Vector3 dimensions, OGLTexture* tempTex, string name, float inverseMass = 10.0f);
			//adding for level design
			GameObject** LevelTestOne();
			GameObject* SpinningPlatform();

			GameObject* AddCoins(const Vector3& position);
			DW_UIText* Score_text;
			DW_UIText* Timer_text;
			DW_UIPanel* InGameUI;
			void Pendulum();
			void UpdateLevelOne();
			void UpdateCoins();
			void UpdateSpinningPlatform();
			void UpdatePlayer(float dt);
			void Reload();
			//end

			StateGameObject* AddStateObjectToWorld(const Vector3& position, const Vector3& dimensions, bool switchD = false, bool perpendicular = false);

			StateGameObject* testStateObject = nullptr;

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
			bool		isdead;
			int			currentLevel = 1;
			int			numstairs;
			int			numcoins;
			int			coincollected;
			int			timer;
			int			pausetime;
			DWORD startTime;
			DWORD pauseStart;
			GameObject** platforms;
			GameObject** coins;
			GameObject* spinplat;
			GameObject* player;
			GameObject* finishLine;
			//end

			// Conor Level (Level Three)
			void LevelThree();
			void UpdateLevelThree(float dt);
			bool isLevelThree = false; // Make sure this is changed in final build!
			std::vector<GameObject*> sliderVector;

			GameObject* selectionObject = nullptr;


			OGLMesh* capsuleMesh = nullptr;
			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;
			OGLTexture* whiteTex = nullptr;
			OGLTexture* greyTex = nullptr;
			OGLTexture* greenTex = nullptr;
			OGLTexture* redTex = nullptr;
			OGLTexture* yellowTex = nullptr;
			OGLTexture* finishTex = nullptr;


			//Coursework Meshes
			OGLMesh* charMeshA = nullptr;
			OGLMesh* charMeshB = nullptr;
			OGLMesh* enemyMesh = nullptr;
			OGLMesh* bonusMesh = nullptr;

			//Teamwork Meshes
			OGLMesh* spinplatMesh = nullptr;
			OGLMesh* cylinderMesh = nullptr;
			//end

			//AI
			GameObject* enemy = nullptr;
			GameTimer  gametime;

			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 14, 20);
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

