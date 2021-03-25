#pragma once


#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"


#include "DW_UIText.h"
#include "DW_UIPanel.h"
#include "DW_UIRenderer.h"

#include "DW_UIImage.h"
#include "DW_Quad.h"

#include "DW_Light.h"
#include "DW_ShadowHelper.h"
#include "DW_DeferredRenderingHelper.h"
#include "DW_SkyboxHelper.h"
#include "DW_RenderCombineHelper.h"
#include "DW_BloomHelper.h"
#include "DW_Flame.h"
#include "DW_Rain.h"


namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			void SetSplitscreen(bool s) { splitscreen = s; }
			bool GetSplitscreen() { return splitscreen; }

			void SetSphereMesh(OGLMesh* m) { m_sphereMesh = m; }
			DW_DeferredRenderingHelper* GetDeferredRenderingHelper() { return m_deferredHelper; }
			void SetExp(const bool flag) {
				if (flag)
				{
					m_exp += 0.01f;
				}
				else {
					m_exp -= 0.01f;
				}
			}

			void RenderSplitscreen(int screenMax) {
				BeginFrame();
				for (int i = 0; i < screenMax; i++) 
					RenderFrameSplitscreen(i, screenMax);
				EndFrame();
				SwapBuffers();
			}

			void Update(float dt) override;

			void SetIsRenderFlame(const bool flag) { m_isRenderFlame = flag; }

		protected:
			void RenderFrame()	override;

			void RenderFrameSplitscreen(int screenCur, int screenMax);

			Matrix4 SetupDebugLineMatrix()	const override;
			Matrix4 SetupDebugStringMatrix()const override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 
			void RenderSecondCamera();
			void RenderSkybox();
			void RenderUI();
			void RenderHUD();

			void LoadSkybox();
			void InitLight();

			void FillGBuffer();
			void RenderLighting();
			void CombineBuffer();

			void BlitFBO1();//from deferred rendering fbo to combine fbo
			void BlitFBO2();//from combine fbo to default fbo
			void RenderLights();
			void RenderFinalQuad();
			void BlurLights();

			void RenderFlame();
			void RenderRain();

			//Matrix4 viewMatrix;
			//Matrix4 projectionMatrix;

			vector<const RenderObject*> activeObjects;

			OGLShader*  skyboxShader;
			OGLMesh*	skyboxMesh;
			GLuint		skyboxTex;
			DW_SkyboxHelper* m_skyboxHelper;

			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;
			DW_ShadowHelper* m_shadowHelper;

			DW_Light* m_directionalLight;

			//deferred rendering things
			DW_DeferredRenderingHelper* m_deferredHelper;
			OGLShader* m_fillBufferShader;
			OGLShader* m_lightingShader;
			OGLShader* m_combineShader;
			OGLMesh* m_sphereMesh;

			//forward rendering things
			OGLShader* m_lightShader;
			OGLShader* m_finalQuadShader;
			DW_RenderCombineHelper* m_combineHelper;
			DW_BloomHelper* m_bloomHelper;
			OGLShader* m_gaussianShader;
			float m_exp{ 1.0f };
			OGLShader* m_flameShader;
			DW_Flame* m_flame;
			bool m_isRenderFlame{true};
			OGLShader* m_rainShader;
			DW_Rain* m_rain;

			bool splitscreen;
			int splitcount;
		};
	}
}

