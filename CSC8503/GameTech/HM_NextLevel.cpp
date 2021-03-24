#include "HM_NextLevel.h"

#include "../CSC8503Common/AudioSystem.h"


HM_NextLevel::HM_NextLevel(NCL::LanguageManager* lm) {
	lang = lm;
	//1.create many ui components if you need! (they must have a name)

	WinScreen_text = new DW_UIText("WinScreenText", "Level Finish !", 1.5f, NCL::Maths::Vector3{ 520.0f,600.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // win text
	Score_text = new DW_UIText("ScoreText", "Score:    "+ std::to_string(score), 1.1f, NCL::Maths::Vector3{ 520.0f,500.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); //  score text

	std::string str{ NCL::Assets::TEXTUREDIR + "button1.png" };


	NextLevel_btn = new DW_UIImage("NextLevelButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,300.0f,0.0f });  // Continue button
	PlayAgain_btn = new DW_UIImage("PlayAgainButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,200.0f,0.0f });  // Restart button
	Back_btn = new DW_UIImage("MenuButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,100.0f,0.0f });  // Back to main menu button

	str = NCL::Assets::TEXTUREDIR + "bg1.png";
	m_bg = new DW_UIImage("Background", str.c_str(), { 1.82f, 1.0f }, NCL::Maths::Vector3{ 650.0f,360.0f,0.0f });  // Menu background picture



	NextLevel_text = new DW_UIText("NextLevelText", lang->GetText("next_level"), 0.7f, NCL::Maths::Vector3{ 570.0f,290.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Next level text
	PlayAgain_text = new DW_UIText("PlayAgain", lang->GetText("play_agian"), 0.7f, NCL::Maths::Vector3{ 570.0f,190.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // play again text
	Back_text = new DW_UIText("Menu", "Menu", 0.7f, NCL::Maths::Vector3{ 590.0f,90.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Back to main menu text
	//2.create an ui panel (it must have a name)
	m_panel = new DW_UIPanel("WinMenu");

	//3.add these ui components into the ui panel. 
	//Attention: 
	//if we don't set rendering priority, the rendering order of ui components will follow the order which we add them
	//if we set rendering priority, the rendering order will follow rendering priority. Less priority will render earlier.
	m_panel->AddComponent(m_bg);

	m_panel->AddComponent(NextLevel_btn);
	m_panel->AddComponent(PlayAgain_btn);
	m_panel->AddComponent(Back_btn);

	m_panel->AddComponent(NextLevel_text);
	m_panel->AddComponent(PlayAgain_text);
	m_panel->AddComponent(Back_text);

	m_panel->AddComponent(Score_text);
	m_panel->AddComponent(WinScreen_text);

	//4.add callback to this panel, if some of its components be clicked, we can do something here! 
	//event type is panel's name
	std::function<void(std::string)> temp = std::bind(&HM_NextLevel::ClickFunc, this, std::placeholders::_1);
	DW_UIEventMgr::get_instance().RegisterUIEvent(m_panel->GetPanelName(), temp);

	//5.add this panel to a panel container, that means we can have many panels at same time!
	DW_UIRenderer::get_instance().AddPanel(m_panel);
}

HM_NextLevel::~HM_NextLevel() {
	delete m_bg;
	delete NextLevel_btn;
	delete PlayAgain_btn;
	delete Back_btn;

	delete NextLevel_text;
	delete PlayAgain_text;
	delete Back_btn;
	delete WinScreen_text;

	delete m_panel;
}

void HM_NextLevel::ClickFunc(const std::string& str) {

	//which image is clicked, the parameter will equal which image's name, then we can do what we want!
	if (str == "NextLevelButton") {
		//not finish
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Back_Button.wav");
		//this->SetPanelActive(false);
		//nextlevel = true;
		//restart = false;
		input = 1;
	}
	else if (str == "PlayAgainButton") {
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Click_Button.wav");
		//this->SetPanelActive(false);
		//restart = true;
		//nextlevel = false;
		input = 2;
	}
	else if (str == "MenuButton") {
		//this->SetPanelActive(false);
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Back_Button.wav");
		//StartMenu = new HM_StartMenu(lang);
		//restart = false;
		//nextlevel = false;
		input = 3;
	}


	std::cout << str << "\n";
}

void HM_NextLevel::SetScore(const int currentscore) {
	score = currentscore;
	Score_text->SetText("Score:    " + std::to_string(score));
}