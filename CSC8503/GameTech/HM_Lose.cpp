#include "HM_Lose.h"
#include "../CSC8503Common/AudioSystem.h"


HM_Lose::HM_Lose() {
	//1.create many ui components if you need! (they must have a name)


	//LoseScreen_text = new DW_UIText("WinScreenText", "You Win !", 1.5f, NCL::Maths::Vector3{ 520.0f,600.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // win text
	//Score_text = new DW_UIText("ScoreText", "Score", 1.1f, NCL::Maths::Vector3{ 520.0f,400.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); //  score text

	std::string str{ NCL::Assets::TEXTUREDIR + "button1.png" };
	PlayAgain_btn = new DW_UIImage("PlayAgainButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,200.0f ,0.0f });  // Continue button
	Back_btn = new DW_UIImage("MenuButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,100.0f,0.0f });  // Back to main menu button

	str = NCL::Assets::TEXTUREDIR + "gameover.png";
	m_lose = new DW_UIImage("Background", str.c_str(), { 0.7f, 0.4f }, NCL::Maths::Vector3{ 630.0f,550.0f,0.0f });  // Menu background picture

	str = NCL::Assets::TEXTUREDIR + "bg1.png";
	m_bg = new DW_UIImage("Background", str.c_str(), { 1.82f, 1.0f }, NCL::Maths::Vector3{ 650.0f,360.0f,0.0f });  // Menu background picture

	PlayAgain_text = new DW_UIText("PlayAgain", "Play Again", 0.7f, NCL::Maths::Vector3{ 570.0f,190.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // play again text
	Back_text = new DW_UIText("Menu", "Menu", 0.7f, NCL::Maths::Vector3{ 590.0f,90.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Back to main menu text
	//2.create an ui panel (it must have a name)
	m_panel = new DW_UIPanel("ExamplePanel");

	//3.add these ui components into the ui panel. 
	//Attention: 
	//if we don't set rendering priority, the rendering order of ui components will follow the order which we add them
	//if we set rendering priority, the rendering order will follow rendering priority. Less priority will render earlier.
	m_panel->AddComponent(m_bg);

	m_panel->AddComponent(PlayAgain_btn);
	m_panel->AddComponent(Back_btn);

	m_panel->AddComponent(PlayAgain_text);
	m_panel->AddComponent(Back_text);

	//m_panel->AddComponent(Score_text);
	m_panel->AddComponent(m_lose);


	//4.add callback to this panel, if some of its components be clicked, we can do something here! 
	//event type is panel's name
	std::function<void(std::string)> temp = std::bind(&HM_Lose::ClickFunc, this, std::placeholders::_1);
	DW_UIEventMgr::get_instance().RegisterUIEvent(m_panel->GetPanelName(), temp);

	//5.add this panel to a panel container, that means we can have many panels at same time!
	DW_UIRenderer::get_instance().AddPanel(m_panel);
}

HM_Lose::~HM_Lose() {
	delete m_bg;
	delete PlayAgain_btn;
	delete Back_btn;

	delete PlayAgain_text;
	delete Back_btn;


	delete m_panel;
}

void HM_Lose::ClickFunc(const std::string& str) {
	//which image is clicked, the parameter will equal which image's name, then we can do what we want!
	if (str == "PlayAgainButton") {
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Click_Button.wav");
		this->SetPanelActive(false);
		restart = true;
	}
	else if (str == "MenuButton") {
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Click_Button.wav");
		this->SetPanelActive(false);
		StartMenu = new HM_StartMenu();
		restart = false;
	}


	std::cout << str << "\n";
}