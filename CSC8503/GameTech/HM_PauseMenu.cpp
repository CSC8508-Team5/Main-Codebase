#include "HM_PauseMenu.h"


HM_PauseMenu::HM_PauseMenu() {
	//1.create many ui components if you need! (they must have a name)


	PauseMenu_text = new DW_UIText("PauseMenuText", "Pause Menu", 1.5f, NCL::Maths::Vector3{ 470.0f,600.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name

	std::string str{ NCL::Assets::TEXTUREDIR + "button1.png" };
	

	Continue_btn = new DW_UIImage("ContinueButton", str.c_str(), { 0.5f, 0.6f }, NCL::Maths::Vector3{ 630.0f,340.0f,0.0f });  // Continue button
	Back_btn = new DW_UIImage("MenuButton", str.c_str(), { 0.5f, 0.6f }, NCL::Maths::Vector3{ 630.0f,240.0f,0.0f });  // Back to main menu button

	str = NCL::Assets::TEXTUREDIR + "bg1.png";
	m_bg = new DW_UIImage("Background", str.c_str(), { 1.82f, 1.0f }, NCL::Maths::Vector3{ 650.0f,360.0f,0.0f });  // Menu background picture


	
	Continue_text = new DW_UIText("Continue", "Continue", 0.7f, NCL::Maths::Vector3{ 570.0f,200.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Continue text
	Back_text = new DW_UIText("Menu", "Menu", 0.7f, NCL::Maths::Vector3{ 590.0f,100.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Back to main menu text
	//2.create an ui panel (it must have a name)
	m_panel = new DW_UIPanel("ExamplePanel");

	//3.add these ui components into the ui panel. 
	//Attention: 
	//if we don't set rendering priority, the rendering order of ui components will follow the order which we add them
	//if we set rendering priority, the rendering order will follow rendering priority. Less priority will render earlier.
	m_panel->AddComponent(m_bg);
	
	m_panel->AddComponent(Continue_btn);
	m_panel->AddComponent(Back_btn);
	
	m_panel->AddComponent(Continue_text);
	m_panel->AddComponent(Back_text);

	m_panel->AddComponent(PauseMenu_text);

	//4.add callback to this panel, if some of its components be clicked, we can do something here! 
	//event type is panel's name
	std::function<void(std::string)> temp = std::bind(&HM_PauseMenu::ClickFunc, this, std::placeholders::_1);
	DW_UIEventMgr::get_instance().RegisterUIEvent(m_panel->GetPanelName(), temp);

	//5.add this panel to a panel container, that means we can have many panels at same time!
	DW_UIRenderer::get_instance().AddPanel(m_panel);
}

HM_PauseMenu::~HM_PauseMenu() {
	delete m_bg;
	delete Continue_btn;
	delete Back_btn;

	delete Continue_text;
	delete Back_btn;
	delete PauseMenu_text;

	delete m_panel;
}

void HM_PauseMenu::ClickFunc(const std::string& str) {
	//which image is clicked, the parameter will equal which image's name, then we can do what we want!
	if (str == "ContinueButton") {

	}
	else if (str == "MenuButton") {

	}
	

	std::cout << str << "\n";
}