#include "DW_UIPanelExample.h"


DW_UIPanelExample::DW_UIPanelExample() {
	//1.create many ui components if you need! (they must have a name)
	std::string str{ NCL::Assets::TEXTUREDIR + "doge.png" };
	m_btn = new DW_UIImage("Button",str.c_str(), { 0.15f, 0.15f }, NCL::Maths::Vector3{ 460.0f,360.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,0.0f });


	str = NCL::Assets::TEXTUREDIR + "checkerboard.png";
	m_bg = new DW_UIImage("Background", str.c_str(), { 0.8f, 0.5f }, NCL::Maths::Vector3{ 640.0f,360.0f,0.0f });

	str = NCL::Assets::TEXTUREDIR + "close.png";
	m_close_btn = new DW_UIImage("CloseButton", str.c_str(), { 0.1f, 0.1f }, NCL::Maths::Vector3{ 800.0f,480.0f,0.0f });

	m_text = new DW_UIText("text","0", 1.0f, NCL::Maths::Vector3{ 700.0f,360.0f,0.0f }, NCL::Maths::Vector3{ 0.0f,0.0f,1.0f });

	//2.create an ui panel (it must have a name)
	m_panel = new DW_UIPanel("ExamplePanel");

	//3.add these ui components into the ui panel. 
    //Attention: 
	//if we don't set rendering priority, the rendering order of ui components will follow the order which we add them
	//if we set rendering priority, the rendering order will follow rendering priority. Less priority will render earlier.
	m_panel->AddComponent(m_bg);
	m_panel->AddComponent(m_btn);
	m_panel->AddComponent(m_close_btn);
	m_panel->AddComponent(m_text);

	//4.add callback to this panel, if some of its components be clicked, we can do something here! 
	//event type is panel's name
	std::function<void(std::string)> temp = [&](std::string str) {
		ClickFunc(str);
	};
	DW_UIEventMgr::get_instance().RegisterUIEvent(m_panel->GetPanelName(),temp);

	//5.add this panel to a panel container, that means we can have many panels at same time!
	DW_UIRenderer::get_instance().AddPanel(m_panel);
}

DW_UIPanelExample::~DW_UIPanelExample() {
	delete m_bg;
	delete m_btn;
	delete m_close_btn;
	delete m_text;

	delete m_panel;
}

void DW_UIPanelExample::ClickFunc(const std::string& str) {
	//which image is clicked, the parameter will equal which image's name, then we can do what we want!
	if (str=="Button")
	{
		m_number++;
		m_text->SetText(std::to_string(m_number));
	}
	else if (str == "CloseButton") {
		m_panel->SetPanelIsEnable(false);
	}
	std::cout << str << "\n";
}