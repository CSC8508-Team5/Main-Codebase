/*
do some task related to deferred rendering 

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once
#include "glad\glad.h"

#include <iostream>
#include <vector>
#include "../../Common/Vector3.h"
#include "DW_Light.h"


class DW_DeferredRenderingHelper
{
public:
	DW_DeferredRenderingHelper();
	~DW_DeferredRenderingHelper();

	GLuint GetBufferFBO() { return m_gBufferFBO; }
	GLuint GetLightingFBO() { return m_lightingFBO; }
private:
	GLuint m_gBufferFBO,m_gBufferColorTex,m_gBufferDepthTex,m_gBufferNormalTex;

	GLuint m_lightingFBO,m_otherLightingDiffTex,m_otherLightingSpecTex,m_D_LightingDiffTex,m_D_LightingSpecTex;

	const int POINT_LIGHTS_NUM = 32;
	const int WIDTH{ 1280 }, HEIGHT{ 720 };
	std::vector<DW_Light*> m_pointLights;
	DW_Light* m_spotLight;
	DW_Light* m_directionalLight;

	void InitLights();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
};

