#include "HM_StartMenu.h"
#include "../../Common/Window.h"
#include "../CSC8503Common/AudioSystem.h"
#include "GameStatusManager.h"

HM_StartMenu::HM_StartMenu(NCL::LanguageManager* lm) {
	lang = lm;
	//1.create many ui components if you need! (they must have a name)
	//std::cout << "\n\n\n\n\n\n\n\nSOCOREBOARD\n\n\n\n\n\n\n\n";
	//std::cout << GetScoreBoard();
	vector<std::string> sVector = GetScoreBoard();
	
	GameName_text = new DW_UIText("GameNameText","Gall Fuys", 1.5f, NCL::Maths::Vector3{ 500.0f,600.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	ScoreBoard_text = new DW_UIText("Scoreboard","TOP SCORES", 1.0f, NCL::Maths::Vector3{ 200.0f,450.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	ScoreBoard_text1 = new DW_UIText("Scoreboard",sVector.at(0), 0.5f, NCL::Maths::Vector3{ 200.0f,400.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	ScoreBoard_text2 = new DW_UIText("Scoreboard",sVector.at(1), 0.5f, NCL::Maths::Vector3{ 200.0f,350.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	ScoreBoard_text3 = new DW_UIText("Scoreboard",sVector.at(2), 0.5f, NCL::Maths::Vector3{ 200.0f,300.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	ScoreBoard_text4 = new DW_UIText("Scoreboard",sVector.at(3), 0.5f, NCL::Maths::Vector3{ 200.0f,250.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	ScoreBoard_text5 = new DW_UIText("Scoreboard",sVector.at(4), 0.5f, NCL::Maths::Vector3{ 200.0f,200.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	//ScoreBoard_text = new DW_UIText("Scoreboard",GetScoreBoard(), 1.5f, NCL::Maths::Vector3{ 400.0f,500.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name
	//ScoreBoard_text = new DW_UIText(GetScoreBoard(), lang->GetText("score_board"), 1.5f, NCL::Maths::Vector3{ 470.0f,500.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Game name


	std::string str{ NCL::Assets::TEXTUREDIR + "button1.png" };
	Solo_btn = new DW_UIImage("SoloButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,315.0f,0.0f });  // single player mode button
	Duo_btn = new DW_UIImage("DuoButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,215.0f,0.0f });  // mutli player mode button
	Quit_btn = new DW_UIImage("QuitButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,115.0f,0.0f });  // quit button

	str = NCL::Assets::TEXTUREDIR + "bg1.png";
	m_bg = new DW_UIImage("Background", str.c_str(), { 1.82f, 1.0f }, NCL::Maths::Vector3{ 650.0f,360.0f,0.0f });  // Menu background picture


	Solo_text = new DW_UIText("Solo", lang->GetText("mode_solo"), 0.7f, NCL::Maths::Vector3{ 605.0f,305.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // single player mode text
	Duo_text = new DW_UIText("Duo", lang->GetText("mode_duo"), 0.7f, NCL::Maths::Vector3{ 605.0f,205.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // mutli player mode text
	Quit_text = new DW_UIText("Quit", lang->GetText("quit"), 0.7f, NCL::Maths::Vector3{ 605.0f,105.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // quit text
	//2.create an ui panel (it must have a name)
	m_panel = new DW_UIPanel("StartMenu");

	//3.add these ui components into the ui panel. 
	//Attention: 
	//if we don't set rendering priority, the rendering order of ui components will follow the order which we add them
	//if we set rendering priority, the rendering order will follow rendering priority. Less priority will render earlier.
	m_panel->AddComponent(m_bg);
	m_panel->AddComponent(Solo_btn);
	m_panel->AddComponent(Duo_btn);
	m_panel->AddComponent(Quit_btn);

	m_panel->AddComponent(Solo_text);
	m_panel->AddComponent(Duo_text);
	m_panel->AddComponent(Quit_text);

	m_panel->AddComponent(GameName_text);
	m_panel->AddComponent(ScoreBoard_text);
	m_panel->AddComponent(ScoreBoard_text1);
	m_panel->AddComponent(ScoreBoard_text2);
	m_panel->AddComponent(ScoreBoard_text3);
	m_panel->AddComponent(ScoreBoard_text4);
	m_panel->AddComponent(ScoreBoard_text5);

	//4.add callback to this panel, if some of its components be clicked, we can do something here! 
	//event type is panel's name
	std::function<void(std::string)> temp = std::bind(&HM_StartMenu::ClickFunc, this, std::placeholders::_1);
	DW_UIEventMgr::get_instance().RegisterUIEvent(m_panel->GetPanelName(), temp);

	//5.add this panel to a panel container, that means we can have many panels at same time!
	DW_UIRenderer::get_instance().AddPanel(m_panel);
}

HM_StartMenu::~HM_StartMenu() {
	delete m_bg;
	delete Solo_btn;

	delete Solo_text;
	delete m_panel;
	delete ScoreBoard_text;
	delete ScoreBoard_text1;
	delete ScoreBoard_text2;
	delete ScoreBoard_text3;
	delete ScoreBoard_text4;
	delete ScoreBoard_text5;
}

void HM_StartMenu::ClickFunc(const std::string& str) {
	//which image is clicked, the parameter will equal which image's name, then we can do what we want!
	if (str == "SoloButton") {
		//this->SetPanelActive(false);
		input = 1;
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Click_Button.wav");
		//NCL::CSC8503::AudioSystem::PlayAudio("Casual Theme #1 (Looped).ogg", true);

	}
	if (str == "DuoButton") {
		//this->SetPanelActive(false);
		input = 2;
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Click_Button.wav");
		//NCL::CSC8503::AudioSystem::PlayAudio("Casual Theme #1 (Looped).ogg", true);
	
	}
	if (str == "QuitButton") {
		input = 3;
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Back_Button.wav");
		//this->SetPanelActive(false);
		//NCL::GameStateManager::SetGameState(NCL::GameStateManager::GameState::Exit);
	}

	std::cout << str << "\n";
}


vector<std::string> HM_StartMenu::GetScoreBoard() {

	std::ifstream file;
	file.open("HighScore.txt");

	if (file.is_open()) {
		std::string s;
		std::string sFinal = "";
		vector<std::string> sVector;
		int counter = 1;
		(getline(file, s)); //skips first line 
		while ((getline(file, s))) {
			sFinal=std::to_string(counter) + " - " + s;
			sVector.push_back(sFinal);
			counter++;
		}
		file.close();

		return sVector;
	}
}