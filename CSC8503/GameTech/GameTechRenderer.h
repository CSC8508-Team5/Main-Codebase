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


namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			void SetSphereMesh(OGLMesh* m) { m_sphereMesh = m; }
			DW_DeferredRenderingHelper* GetDeferredRenderingHelper() { return m_deferredHelper; }
		protected:
			void RenderFrame()	override;

			Matrix4 SetupDebugLineMatrix()	const override;
			Matrix4 SetupDebugStringMatrix()const override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 
			void RenderSkybox();
			void RenderUI();
			void RenderHUD();

			void LoadSkybox();
			void InitLight();

			void FillGBuffer();
			void RenderLighting();
			void CombineBuffer();

			void BlitFBO();
			void RenderLights();

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


		};
	}
}

