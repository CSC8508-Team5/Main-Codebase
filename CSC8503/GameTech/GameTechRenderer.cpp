
#include "GameTechRenderer.h"
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Camera.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/TextureLoader.h"
#pragma warning
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

	splitscreen = true;

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
	m_skyboxHelper = new DW_SkyboxHelper();
	skyboxShader = new OGLShader("CubemapVertex.glsl", "CubemapFragment.glsl");
	LoadSkybox();
	//skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	/*skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();*/
	
	//forward rendering
	m_combineHelper = new DW_RenderCombineHelper(currentWidth, currentHeight);
	m_lightShader= new OGLShader("LightVertex.glsl", "LightFragment.glsl");
	m_finalQuadShader = new OGLShader("FinalQuadVert.glsl", "FinalQuadFrag.glsl");
	m_gaussianShader = new OGLShader("GaussianVert.glsl", "GaussianFrag.glsl");
	m_bloomHelper = new DW_BloomHelper(currentWidth, currentHeight );
	m_flameShader = new OGLShader("FlameVert.glsl", "FlameFrag.glsl");
	m_flame = new DW_Flame(Vector3{0.0f,1.0f,0.0f}, NCL::Assets::TEXTUREDIR + "fire.jpg");
	m_flame->SetParticlePos(NCL::Maths::Vector3(-150.0f, 20.f, -30.0f));
	m_rainShader = new OGLShader("rainVert.glsl", "rainFrag.glsl","rainGeom.glsl");
	m_rain = new DW_Rain(Vector3{ 0.0f,-0.2f,0.0f });
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
	delete m_lightingShader;
	delete m_combineShader;
	delete m_skyboxHelper;
	delete m_combineHelper;
	delete m_lightShader;
	delete m_finalQuadShader;
	delete m_rainShader;
	delete m_rain;
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

	glUniform3fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "lightDir"), 1, (float*)&m_deferredHelper->GetDirectionLight()->GetDirection());
	
	glUniform1i(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "shadowTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadowHelper->GetTexture());

	for (const auto& i : activeObjects) {

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_fillBufferShader->GetProgramID(), "modelMatrix"), 1, false, (float*)&modelMatrix);
		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "diffuseTex", 1);

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
	glBindFramebuffer(GL_FRAMEBUFFER, m_combineHelper->GetFBO());
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::BlitFBO1() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_deferredHelper->GetBufferFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_combineHelper->GetFBO());
	glBlitFramebuffer(0, 0, currentWidth, currentHeight, 0, 0, currentWidth, currentHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::BlitFBO2() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_combineHelper->GetFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, currentWidth, currentHeight, 0, 0, currentWidth, currentHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::RenderLights() {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projectionMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	glBindFramebuffer(GL_FRAMEBUFFER, m_combineHelper->GetFBO());

	BindShader(m_lightShader);

	glUniformMatrix4fv(glGetUniformLocation(m_lightShader->GetProgramID(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(m_lightShader->GetProgramID(), "projMatrix"), 1, false, (float*)&projectionMatrix);

	std::vector<DW_Light*> temp = m_deferredHelper->GetPointLights();
	for (int i = 0; i < temp.size(); i++)
	{
		Matrix4 model = Matrix4::Translation(temp[i]->GetPosition());
		glUniformMatrix4fv(glGetUniformLocation(m_lightShader->GetProgramID(), "modelMatrix"), 1, false, (float*)&model);
		glUniform4fv(glGetUniformLocation(m_lightShader->GetProgramID(), "color"), 1, (float*)&temp[i]->GetColor());
		BindMesh(m_sphereMesh);
		DrawBoundMesh();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::RenderFinalQuad() {
	BindShader(m_finalQuadShader);

	glUniform1i(glGetUniformLocation(m_finalQuadShader->GetProgramID(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_combineHelper->GetTexture());
	//glBindTexture(GL_TEXTURE_2D, m_bloomHelper->GetPingPangColorBuffer(1));

	glUniform1i(glGetUniformLocation(m_finalQuadShader->GetProgramID(), "bloomTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_bloomHelper->GetPingPangColorBuffer(1));

	glUniform1f(glGetUniformLocation(m_finalQuadShader->GetProgramID(), "exposure"), m_exp);
	
	DW_Quad::get_instance().BindVAO();
	DW_Quad::get_instance().Draw();
}

void GameTechRenderer::BlurLights() {
	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10;
	GLuint tex = m_combineHelper->GetBlurTexture();
	BindShader(m_gaussianShader);

	glUniform1i(glGetUniformLocation(m_gaussianShader->GetProgramID(), "image"), 0);
	glActiveTexture(GL_TEXTURE0);

	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_bloomHelper->GetPingPangFBO(horizontal));

		glUniform1i(glGetUniformLocation(m_gaussianShader->GetProgramID(), "horizontal"), horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? tex : m_bloomHelper->GetPingPangColorBuffer(!horizontal));  // bind texture of other framebuffer (or scene if first iteration)

		DW_Quad::get_instance().BindVAO();
		DW_Quad::get_instance().Draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::RenderFlame() {
	if (!m_isRenderFlame)
	{
		return;
	}
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projectionMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	//glBindFramebuffer(GL_FRAMEBUFFER, m_combineHelper->GetFBO());
	//glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BindShader(m_flameShader);
	glUniformMatrix4fv(glGetUniformLocation(m_flameShader->GetProgramID(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(m_flameShader->GetProgramID(), "projMatrix"), 1, false, (float*)&projectionMatrix);

	glUniform1i(glGetUniformLocation(m_finalQuadShader->GetProgramID(), "tex0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_flame->GetTexture());

	for (std::vector<DW_Particle*>::iterator it = m_flame->GetParticlesBegin(); it != m_flame->GetParticlesEnd(); ++it)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_flameShader->GetProgramID(), "modelMatrix"), 1, false, (float*)&(*it)->GetModelMatrix());

		if ((*it)->GetLifeSpan() <= 0.3) {
			glUniform1f(glGetUniformLocation(m_flameShader->GetProgramID(), "colourRedVar"), 1.0f);
			glUniform1f(glGetUniformLocation(m_flameShader->GetProgramID(), "colourGreenVar"), 1.0f);
			glUniform1f(glGetUniformLocation(m_flameShader->GetProgramID(), "colourBlueVar"), 1.0f);
		}
		else {
			glUniform1f(glGetUniformLocation(m_flameShader->GetProgramID(), "colourGreenVar"), (*it)->GetLifeSpan() / 0.255f);
			glUniform1f(glGetUniformLocation(m_flameShader->GetProgramID(), "colourRedVar"), (*it)->GetLifeSpan() / -1.35f);
			glUniform1f(glGetUniformLocation(m_flameShader->GetProgramID(), "colourBlueVar"), 0.0f);
		}

		/*glBindVertexArray(m_flame->GetVAO());
		for (int j = 0; j < 16; j++) {
			if (j % 4 == 0)
				glDrawArrays(GL_TRIANGLE_STRIP, j, 4);
		}*/
		glBindVertexArray(m_flame->GetVAO());
		glDrawArrays(GL_POINTS, 0, 3000);
	}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderRain() {
	glEnable(GL_PROGRAM_POINT_SIZE);
	BindShader(m_rainShader);
	
	//glUniform1f(glGetUniformLocation(m_rainShader->GetProgramID(), "time"), hostWindow.GetTimer()->GetTotalTimeSeconds());
	glUniform3f(glGetUniformLocation(m_rainShader->GetProgramID(), "color"), 100 / 255.0f, 149 / 255.0f, 237 / 255.0f);

	for (std::vector<DW_Particle*>::iterator it = m_rain->GetParticlesBegin(); it != m_rain->GetParticlesEnd(); ++it) {

		Vector2 temp{ 0.0f,0.0f };
		glUniform2fv(glGetUniformLocation(m_rainShader->GetProgramID(), "offset"), 1, (float*)&(*it)->GetPosition());

		glUniform2fv(glGetUniformLocation(m_rainShader->GetProgramID(), "move"), 1, (float*)&(*it)->GetOriginPos());
		m_rain->Draw();
	}

	

	
	glDisable(GL_PROGRAM_POINT_SIZE);
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
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::Update(float dt) {
	//std::cout << "T:" << hostWindow.GetTimer()->GetTotalTimeSeconds() << "\n";
	m_flame->Update(dt);
	m_rain->Update(dt);
}

void GameTechRenderer::RenderFrame() {
	BuildObjectList();
	SortObjectList();

	//0.shadow map
	RenderShadowMap();

	//1.deferred rendering stage
	FillGBuffer();
	RenderLighting();
	CombineBuffer();

	//2.copy depth and stencil buffer 
	BlitFBO1();

	//3.forward rendering stage
	RenderLights();
	BlurLights();

	RenderSkybox();//TODO, the color of skybox lead to the error of the color of bloom
	RenderFinalQuad();

	//RenderCamera();
	//glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...

	BlitFBO2();

	RenderFlame();
	RenderRain();

	RenderHUD();
	RenderUI();
}

void GameTechRenderer::RenderFrameSplitscreen(int screenCur, int screenMax) {
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	/*for (int i = 0; i < 2; i++) {
		switch (i) {
			case 0:
				//gameWorld.GetMainCamera()->UpdateThirdPersonCamera(player->GetTransform(), Vector3::Up(), dt);
				break;
			case 1:
				gameWorld.GetMainCamera()->UpdateThirdPersonCamera(Transform(), Vector3::Up(), 1);
				break;
			default:
				break;
		}*/
		glViewport((currentWidth / screenMax) * screenCur, 0, currentWidth / screenMax, currentHeight);
		BuildObjectList();
		SortObjectList();
		RenderShadowMap();
		glViewport((currentWidth / screenMax) * screenCur, 0, currentWidth / screenMax, currentHeight);
		RenderSkybox();
		switch (screenCur) {
		case 0:
			RenderCamera();
			break;
		case 1:
			RenderSecondCamera();
			break;
		default:
			break;
		}

	glDisable(GL_CULL_FACE);
	glViewport(0, 0, currentWidth, currentHeight);
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

	if (!splitscreen) {
		glViewport(0, 0, currentWidth, currentHeight);
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox() {
	if (splitscreen) {
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		float screenAspect = (float)currentWidth / (float)currentHeight;
		Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrixWithoutTranlation();
		Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

		BindShader(skyboxShader);

		int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
		int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
		int texLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

		glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
		glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

		glUniform1i(texLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

		m_skyboxHelper->Draw();

		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		glBindFramebuffer(GL_FRAMEBUFFER, m_combineHelper->GetFBO());
		glDepthFunc(GL_LEQUAL);
		BindShader(skyboxShader);

		float screenAspect = (float)currentWidth / (float)currentHeight;
		Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrixWithoutTranlation();
		Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
		int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
		int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
		glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
		glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

		glUniform1i(glGetUniformLocation(skyboxShader->GetProgramID(), "cubemap"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

		m_skyboxHelper->Draw();
		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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

void GameTechRenderer::RenderSecondCamera() {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetSecondCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetSecondCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation = 0;
	int viewLocation = 0;
	int modelLocation = 0;
	int colourLocation = 0;
	int hasVColLocation = 0;
	int hasTexLocation = 0;
	int shadowLocation = 0;

	int lightPosLocation = 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int cameraLocation = 0;

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_shadowHelper->GetTexture());

	for (const auto& i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

		if (activeShader != shader) {
			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");


			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			glUniform3fv(cameraLocation, 1, (float*)&gameWorld.GetSecondCamera()->GetPosition());

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

		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);

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
