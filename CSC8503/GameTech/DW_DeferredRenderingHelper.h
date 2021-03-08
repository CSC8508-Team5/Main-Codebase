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
private:
	GLuint m_gBufferFBO,m_gBufferColorTex,m_gBufferDepthTex,m_gBufferNormalTex;

	GLuint m_lightingFBO,m_lightingDiffTex,m_lightingSpecTex;

	const int POINT_LIGHTS_NUM = 32;
	std::vector<DW_Light*> m_pointLights;
	DW_Light* m_spotLight;
	DW_Light* m_directionalLight;

	void InitLights();
};

