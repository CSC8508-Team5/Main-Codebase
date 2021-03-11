#include "GameTechRenderer.h"
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Camera.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/TextureLoader.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world)	{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//deferred rendering
	m_deferredHelper = new DW_DeferredRenderingHelper(currentWidth,currentHeight);
	m_fillBufferShader = new OGLShader("FillBufferVert.glsl", "FillBufferFrag.glsl");
	m_lightingShader = new OGLShader("LightingVert.glsl", "LightingFrag.glsl");
	m_combineShader= new OGLShader("CombineVert.glsl", "CombineFrag.glsl");
	InitLight();


	//Shadow
	m_shadowHelper = new DW_ShadowHelper();
	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	//Skybox!
	skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	LoadSkybox();
}

GameTechRenderer::~GameTechRenderer()	{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	delete defaultShader;
	delete skyboxMesh;
	delete skyboxShader;
	delete m_directionalLight;
	delete m_shadowHelper;
	delete shadowShader;
	delete m_deferredHelper;
	delete m_fillBufferShader;
}


void GameTechRenderer::InitLight() {
	m_directionalLight = new DW_Light(DW_Light::LightType::Directional);
	m_directionalLight->SetPosition(Vector3(-200.0f, 230.0f, -200.0f));
	m_directionalLight->SetDirection(Vector3(0, 0, 0) - m_directionalLight->GetPosition());
	m_directionalLight->SetColor(Vector4(0.6f, 0.6f, 0.6f, 1));
	m_directionalLight->SetAmbient(0.05f);
}

void GameTechRenderer::FillGBuffer() {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projectionMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	/*float screenAspect = (float)currentWidth / (float)currentHeight;
	viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	projectionMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);*/

	glBindFramebuffer(GL_FRAMEBUFFER, m_deferredHelper->GetBufferFBO());
	glClear( GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT );

	BindShader(m_fillBufferShader);

	glUniformMatrix4fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "projMatrix"), 1, false, (float*)&projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "shadowMatrix"), 1, false, (float*)&shadowMatrix);

	glUniform1i(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "shadowTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_shadowHelper->GetTexture());

	glUniform3fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "lightDir"), 1, (float*)&m_deferredHelper->GetDirectionLight()->GetDirection());
	

	for (const auto& i : activeObjects) {

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "modelMatrix"), 1, false, (float*)&modelMatrix);
		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "diffuseTex", 0);

		glUniform4fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "objectColour"), 1, (float*)&i->GetColour());
		glUniform1i(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "hasVertexColours"), !(*i).GetMesh()->GetColourData().empty());
		glUniform1i(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "hasTexture"), (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);
		
		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::RenderLighting() {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projectionMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	glBindFramebuffer(GL_FRAMEBUFFER, m_deferredHelper->GetLightingFBO());
	BindShader(m_lightingShader);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniformMatrix4fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "projMatrix"), 1, false, (float*)&projectionMatrix);

	glUniform1i(glGetUniformLocation(m_lightingShader->GetProgramID(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetGBufferDepthTex());

	glUniform1i(glGetUniformLocation(m_lightingShader->GetProgramID(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetGBufferNormalTex());

	glUniform1i(glGetUniformLocation(m_lightingShader->GetProgramID(), "shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetGBufferShadowTex());

	glUniform2f(glGetUniformLocation(m_lightingShader->GetProgramID(), "pixelSize"), 1.0f / (float)currentWidth, 1.0f / (float)currentHeight);
	glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "cameraPos"), 1, (float*)&gameWorld.GetMainCamera()->GetPosition());

	Matrix4 invViewProj = (projectionMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "inverseProjView"), 1, false, (float*)&invViewProj);

	glCullFace(GL_BACK);
	//directional light
	glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "directionalLight.direction"), 1, (float*)&m_deferredHelper->GetDirectionLight()->GetDirection());
	glUniform4fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "directionalLight.color"), 1, (float*)&m_deferredHelper->GetDirectionLight()->GetColor());
	glUniform1f(glGetUniformLocation(m_lightingShader->GetProgramID(), "directionalLight.ambient"), m_deferredHelper->GetDirectionLight()->GetAmbient());

	glUniform1i(glGetUniformLocation(m_lightingShader->GetProgramID(), "lightType"), m_deferredHelper->GetDirectionLight()->GetType());

	DW_Quad::get_instance().BindVAO();
	DW_Quad::get_instance().Draw();


	glCullFace(GL_FRONT);
	//point light
	std::vector<DW_Light*> temp = m_deferredHelper->GetPointLights();
	for (int i = 0; i < temp.size(); i++)
	{
		glUniform1i(glGetUniformLocation(m_lightingShader->GetProgramID(), "lightType"), temp[i]->GetType());
		glUniform1f(glGetUniformLocation(m_lightingShader->GetProgramID(), "lightRadius"), temp[i]->GetRadius());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "lightPos"), 1, (float*)&temp[i]->GetPosition());

		glUniform1f(glGetUniformLocation(m_lightingShader->GetProgramID(), "pointLight.ambient"), temp[i]->GetAmbient());
		glUniform1f(glGetUniformLocation(m_lightingShader->GetProgramID(), "pointLight.radius"), temp[i]->GetRadius());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "pointLight.position"), 1, (float*)&temp[i]->GetPosition());
		glUniform4fv(glGetUniformLocation(m_lightingShader->GetProgramID(), "pointLight.color"), 1, (float*)&temp[i]->GetColor());

		BindMesh(m_sphereMesh);
		DrawBoundMesh();
	}


	glCullFace(GL_BACK);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GameTechRenderer::CombineBuffer() {
	BindShader(m_combineShader);
	glUniform1i(glGetUniformLocation(m_combineShader->GetProgramID(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetGBufferColorTex());

	glUniform1i(glGetUniformLocation(m_combineShader->GetProgramID(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetLightingDiffTex());

	glUniform1i(glGetUniformLocation(m_combineShader->GetProgramID(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,m_deferredHelper->GetLightingSpecTex());

	DW_Quad::get_instance().BindVAO();
	DW_Quad::get_instance().Draw();
}

void GameTechRenderer::BlitFBO() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_deferredHelper->GetBufferFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, currentWidth, currentHeight, 0, 0, currentWidth, currentHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::LoadSkybox() {
	string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	int width[6]	= { 0 };
	int height[6]	= { 0 };
	int channels[6] = { 0 };
	int flags[6]	= { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::RenderFrame() {
	//glEnable(GL_CULL_FACE);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	FillGBuffer();
	RenderLighting();
	CombineBuffer();

	BlitFBO();

	//RenderSkybox();
	//RenderCamera();
	//glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...

	RenderHUD();
	RenderUI();
	
}

void GameTechRenderer::RenderUI() {
	glDisable(GL_DEPTH_TEST);
	DW_UIRenderer::get_instance().Render();//UI SYSTEM render
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderHUD() {
	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()&&o->GetHUD()!=nullptr) {
				float screenAspect = (float)currentWidth / (float)currentHeight;
				Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
				Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
				DW_Quad::get_instance().Draw(o->GetHUD()->GetTexture(), viewMatrix, projMatrix, o->GetTransform().GetPosition()+ o->GetHUD()->GetPosOffset(), Vector3{ o->GetHUD()->GetScale().x,o->GetHUD()->GetScale().y,1.0f }, gameWorld.GetMainCamera()->GetRotationMatrix());
			}
		}
	);
}

void GameTechRenderer::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);
}

void GameTechRenderer::SortObjectList() {
	//Who cares!
}

void GameTechRenderer::RenderShadowMap() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowHelper->GetFBO());
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	
	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(m_deferredHelper->GetDirectionLight()->GetPosition(), m_deferredHelper->GetDirectionLight()->GetPosition() + m_deferredHelper->GetDirectionLight()->GetDirection(), Vector3(0,1,0));
	//Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);
	Matrix4 shadowProjMatrix = Matrix4::Orthographic(1.0f, 400.0f, 100.0f, -100.0f, 100.0f, -100.0f);//directional light use orth matrix to cast shadow

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	//shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on
	shadowMatrix =  mvMatrix;

	for (const auto&i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}

	glViewport(0, 0, currentWidth, currentHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	BindShader(skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation  = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(skyboxMesh);
	DrawBoundMesh();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera() {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation	= 0;
	int viewLocation	= 0;
	int modelLocation	= 0;
	int colourLocation  = 0;
	int hasVColLocation = 0;
	int hasTexLocation  = 0;
	int shadowLocation  = 0;

	int lightPosLocation	= 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int cameraLocation = 0;

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_shadowHelper->GetTexture());

	for (const auto&i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

		if (activeShader != shader) {
			projLocation	= glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation	= glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation	= glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation  = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation  = glGetUniformLocation(shader->GetProgramID(), "hasTexture");


			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			glUniform3fv(cameraLocation, 1, (float*)&gameWorld.GetMainCamera()->GetPosition());

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			glUniform3fv(glGetUniformLocation(shader->GetProgramID(), "directionalLight.direction"), 1, (float*)&m_directionalLight->GetDirection());
			glUniform4fv(glGetUniformLocation(shader->GetProgramID(), "directionalLight.color"), 1, (float*)&m_directionalLight->GetColor());
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), "directionalLight.ambient"), m_directionalLight->GetAmbient());

			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 1);

			activeShader = shader;
		}

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);			
		
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&shadowMatrix);

		glUniform4fv(colourLocation, 1, (float*)&i->GetColour());

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1:0);

		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}
}

Matrix4 GameTechRenderer::SetupDebugLineMatrix()	const  {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

Matrix4 GameTechRenderer::SetupDebugStringMatrix()	const {
	return Matrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}
