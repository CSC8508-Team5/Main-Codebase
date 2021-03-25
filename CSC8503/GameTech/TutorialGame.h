#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/AudioSystem.h"
#include "../GameTech/PlayerObject.h"
#include "StateGameObject.h"
#include "StateAi.h"
#include "../../Common/Vector3.h"
#include "../../Common/Assets.h"

#include "../CSC8503Common/LanguageManager.h"
#include "../CSC8503Common/SettingsManager.h"

#include "DW_UIRenderer.h"
#include "DW_UIText.h"
#include "DW_UIPanel.h"
#include "DW_UIImage.h"
#include "HM_StartMenu.h"
#include "HM_PauseMenu.h"
#include "HM_Win.h"
#include "HM_Lose.h"
#include "HM_Option.h"
#include "HM_NextLevel.h"
#include "GameStatusManager.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame {
		public:
			TutorialGame(SettingsManager* s = nullptr);
			~TutorialGame();

			int GetCurrentLevel() { return currentLevel; }
			void SetCurrentLevel(int level) { currentLevel = level; }
			int GetMaxLevel() { return 3; }

			virtual void UpdateGame(float dt);

			std::string GetScoreBoard();//returns a formatted string of top 5 scores
			void AddBoardScore(int score);//adds score to high score file

			//int GetScore() { return score + timer * 10 + coincollected * 50;}
			void UpdateScore() { AddScore(timer * 10 + coincollected * 50); }
			void AddScore(int s) { score += s; }
			void SetScore(int s) { score = s; }
			int GetScore() { return score; }
			int GetAddingScore() { return score + timer * 10 + coincollected * 50; }

			void ChangeLevel() { currentLevel += 1; }
			void ResetLevel() { InitWorld(); }

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitLevel1();
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

			//void InitCharaters(Vector3 position) { playerOrigin = Transform().SetPosition(position); };
			void InitCharaters(Vector3 position, Quaternion orientation = Quaternion(0, -1, 0, 1)) { orientation.Normalise(); playerOrigin = Transform().SetPosition(position).SetOrientation(orientation); };
			void InstantiateCharaters();
			void ResetCharacters() { if (player) player->GetTransform().SetPosition(playerOrigin.GetPosition()).SetOrientation(playerOrigin.GetOrientation()); player->GetPhysicsObject()->SetLinearVelocity(Vector3::Zero()); };

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

			void AddCoinAttributesToObject(GameObject* obj) {
				/*obj->SetSoundSource(new SoundSource(Assets::SFXDIR + "PP_Collect_Coin_1_2.wav", obj->GetTransform().GetPosition()));
				obj->GetSoundSource()->SetMinDistance(50.0f);
				obj->GetSoundSource()->SetVolume(1.0f);*/
				obj->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
				obj->SetOnCollisionBeginFunction([=](GameObject* other) {
					if (other->GetLayer() == GameObject::Layer::Player)
					{
						AudioSystem::PlaySFX("PP_Collect_Coin_1_2.wav");
						//obj->GetSoundSource()->Play();
						coincollected += 1;
						obj->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
						world->RemoveGameObject(obj, false);
						std::cout << "Coin Triggered\n";
					}
					});
			}
			void AddPlayerhAttributesToObject(GameObject* obj){
				obj->SetOnCollisionStayFunction([=](GameObject* other) {
					if (other->GetLayer() == GameObject::Layer::Default)
					{
						//onGround = true;
					}
					});
				obj->SetOnCollisionEndFunction([=](GameObject* other) {
					if (other->GetLayer() == GameObject::Layer::Default)
					{
						//onGround = false;
					}
					});
			}

			void AddFinishAttributesToObject(GameObject* obj) {
				obj->SetOnCollisionBeginFunction([=](GameObject* other) {
					if (other->GetLayer() == GameObject::Layer::Player)
					{
						GameStateManager::SetGameState(GameStateManager::State::Win);
						world->RemoveGameObject(obj, false);
						std::cout << "Finish Triggered\n";
					}
					});
			}

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
			GameObject* AddDoorToWorld(const Vector3& position, Vector3 dimensions, OGLTexture* tempTex, string name, float inverseMass = 0.1f);
			//adding for level design
			GameObject** LevelOne();
			GameObject* AddCannonToWorld(const Vector3& position, string orientation);
			Vector4 prevColor;

			GameObject* AddCoins(const Vector3& position);
			DW_UIText* Score_text;
			DW_UIText* Debug_text1;
			float fps;
			DW_UIText* Debug_text2;
			DW_UIText* Debug_text3;
			DW_UIText* Debug_text4;
			DW_UIText* Timer_text;
			DW_UIPanel* InGameUI;
			DW_UIPanel* InGameUI1;

			void UpdateLevelOne();
			void UpdateLevelTwo();
			void UpdateCoins();
			void UpdateCannonBullet(GameObject* bullet, const Vector3& startPosition, string direction);

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
			bool inDebugMode;

			float		forceMagnitude;
			//adding for level design
			float		platformtimer;
			float		yaw;
			float		pitch;
			bool		isjump;
			bool		ispause;
			int			currentLevel = 1;
			int			numstairs;
			int			numcoins;
			int			coincollected;
			int			timer;
			int			pausetime;
			int			score;
			bool		scoreAdded;
			DWORD startTime;
			DWORD pauseStart;
			GameObject** platforms;
			GameObject** coins;
			GameObject** cannonBullet;
			GameObject* spinplat;
			GameObject* player;
			GameObject* level3finishLine;
			GameObject* level3Floor;
			GameObject* level2finishLine;
			GameObject* level2Floor;

			//end

			// Conor Level (Level Three)
			void LevelThree();
			void UpdateLevelThree(float dt);
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

			Transform playerOrigin;

			//HM UI 
			HM_StartMenu* StartMenu;
			HM_PauseMenu* PauseMenu;
			HM_Win* WinScreen;
			HM_Lose* LoseScreen;
			HM_Option* OptionMenu;
			HM_NextLevel* NextLevel;
			//Language Manager
			LanguageManager* langContent;
			SettingsManager* settings;

			const double PI = 3.1415927;
		};
	}
}

