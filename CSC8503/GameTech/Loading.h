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
		class Loading
		{
		public:
			Loading();
			~Loading();
			void UpdateGame(float dt);
		private:
			void InitCamera();
			void InitialAssets();
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);



			GameObject* player;
			GameTechRenderer* renderer;

			GameWorld* world;
			float		yaw;
			float		pitch;

			HM_StartMenu* StartMenu;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;
			OGLMesh* cubeMesh = nullptr;



		};
		
	}
}

