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
	DW_DeferredRenderingHelper(const int w,const int h);
	~DW_DeferredRenderingHelper();

	GLuint GetBufferFBO() { 
		return m_gBufferFBO; 
	}

	GLuint GetLightingFBO() { 
		return m_lightingFBO; 
	}

	DW_Light* GetDirectionLight() { 
		return m_directionalLight; 
	}

	std::vector<DW_Light*> GetPointLights() {
		return m_pointLights;
	}

	GLuint GetGBufferDepthTex() {
		return m_gBufferDepthTex;
	}

	GLuint GetGBufferNormalTex() { 
		return m_gBufferNormalTex; 
	}

	GLuint GetGBufferColorTex() { 
		return m_gBufferColorTex; 
	}

	GLuint GetGBufferShadowTex() {
		return m_gBufferShadowTex;
	}

	GLuint GetLightingDiffTex() { return m_lightingDiffTex; }
	GLuint GetLightingSpecTex() { return m_lightingSpecTex;}

	void SetPointLights(const std::vector<NCL::Maths::Vector3>& poses);

private:
	std::vector<NCL::Maths::Vector4> COLORS{ {5.0f, 5.0f, 5.0f,1.0f},{15.0f, 0.0f, 0.0f,1.0f},{0.0f, 0.0f, 15.0f,1.0f},{0.0f, 5.0f, 0.0f,1.0f},{5.0f, 5.0f, 0.0f,1.0f} };
	GLuint m_gBufferFBO,m_gBufferColorTex,m_gBufferDepthTex,m_gBufferNormalTex, m_gBufferShadowTex;

	GLuint m_lightingFBO, m_lightingDiffTex, m_lightingSpecTex;

	const int POINT_LIGHTS_NUM = 32;
	int m_width, m_height;
	std::vector<DW_Light*> m_pointLights;
	DW_Light* m_spotLight;
	DW_Light* m_directionalLight;

	void InitLights();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
};

