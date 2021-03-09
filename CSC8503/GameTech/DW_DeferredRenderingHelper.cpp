#include "DW_DeferredRenderingHelper.h"


DW_DeferredRenderingHelper::DW_DeferredRenderingHelper() {
	InitLights();

	glGenFramebuffers(1, &m_gBufferFBO);
	glGenFramebuffers(1, &m_lightingFBO);

	GLenum buffers[2] = {GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1};
	GLenum buffers1[4] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };

	GenerateScreenTexture(m_gBufferDepthTex, true);
	GenerateScreenTexture(m_gBufferColorTex);
	GenerateScreenTexture(m_gBufferNormalTex);
	GenerateScreenTexture(m_otherLightingDiffTex);
	GenerateScreenTexture(m_otherLightingSpecTex);
	GenerateScreenTexture(m_D_LightingDiffTex);
	GenerateScreenTexture(m_D_LightingSpecTex);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBufferColorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gBufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_lightingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_otherLightingDiffTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_otherLightingSpecTex, 0);
	glDrawBuffers(4, buffers1);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


DW_DeferredRenderingHelper::~DW_DeferredRenderingHelper() {
	for (auto item:m_pointLights )
	{
		delete item;
	}
	delete m_spotLight;
	delete m_directionalLight;
}

void DW_DeferredRenderingHelper::InitLights() {
	float space{ 10.0f };
	//point lights
	for ( int i = 0; i < POINT_LIGHTS_NUM; i++)
	{
		DW_Light* temp = new DW_Light(DW_Light::LightType::Point);
		NCL::Maths::Vector3 pos = NCL::Maths::Vector3(i * space, 50.0f, i * space);
		temp->SetPosition(pos);
		temp->SetColor(NCL::Maths::Vector4{ 1.0f,0.0f,0.0f,1.0f });
		temp->SetAmbient(0.05f);
		temp->SetRadius(200.0f);

		m_pointLights.push_back(temp);
	}

	//spot light
	m_spotLight=new DW_Light(DW_Light::LightType::Spot);
	m_spotLight->SetPosition(NCL::Maths::Vector3{100.0f,100.0f,100.0f});
	m_spotLight->SetColor(NCL::Maths::Vector4{ 0.0f,0.0f,1.0f,1.0f });
	m_spotLight->SetAmbient(0.05f);
	m_spotLight->SetRadius(200.0f);
	m_spotLight->SetDirection(NCL::Maths::Vector3(0, -1.0f, 0));


	//directional light
	m_directionalLight = new DW_Light(DW_Light::LightType::Directional);
	m_directionalLight->SetPosition(NCL::Maths::Vector3(-200.0f, 230.0f, -200.0f));
	m_directionalLight->SetDirection(NCL::Maths::Vector3(0, 0, 0) - m_directionalLight->GetPosition());
	m_directionalLight->SetColor(NCL::Maths::Vector4(0.6f, 0.6f, 0.6f, 1));
	m_directionalLight->SetAmbient(0.05f);
}

void DW_DeferredRenderingHelper::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, WIDTH, HEIGHT, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}