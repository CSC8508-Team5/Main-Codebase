#include "DW_DeferredRenderingHelper.h"


DW_DeferredRenderingHelper::DW_DeferredRenderingHelper() {
	InitLights();

}


DW_DeferredRenderingHelper::~DW_DeferredRenderingHelper() {

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