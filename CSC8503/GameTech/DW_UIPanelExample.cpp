#include "DW_UIPanelExample.h"


DW_UIPanelExample::DW_UIPanelExample() {
	std::string str{ NCL::Assets::TEXTUREDIR + "doge.png" };
	m_btn = new DW_UIImage(str.c_str(), { 0.15f, 0.15f }, NCL::Maths::Vector3{ 460.0f,360.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,0.0f });
	m_btn->SetName("Button");

	str = NCL::Assets::TEXTUREDIR + "checkerboard.png";
	m_bg = new DW_UIImage(str.c_str(), { 0.8f, 0.5f }, NCL::Maths::Vector3{ 640.0f,360.0f,0.0f });
	m_bg->SetName("Background");

	str = NCL::Assets::TEXTUREDIR + "close.png";
	m_close_btn = new DW_UIImage(str.c_str(), { 0.1f, 0.1f }, NCL::Maths::Vector3{ 800.0f,480.0f,0.0f });
	m_close_btn->SetName("CloseButton");

	m_text = new DW_UIText("0", 1.0f, NCL::Maths::Vector3{ 700.0f,360.0f,0.0f }, NCL::Maths::Vector3{ 0.0f,0.0f,1.0f });

	m_panel = new DW_UIPanel();

	m_panel->AddComponent(m_bg);
	m_panel->AddComponent(m_btn);
	m_panel->AddComponent(m_close_btn);
	m_panel->AddComponent(m_text);

	DW_UIRenderer::get_instance().AddPanel(m_panel);

}

DW_UIPanelExample::~DW_UIPanelExample() {
	delete m_bg;
	delete m_btn;
	delete m_close_btn;
	delete m_text;
}