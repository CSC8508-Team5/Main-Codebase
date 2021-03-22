#include "HM_Option.h"

HM_Option::HM_Option(NCL::LanguageManager* lm, NCL::CSC8503::AudioSystem* audio) {
	lang = lm;
	// Passing in Global Audio
	globalAudio = audio;
	volume = audio->GetGlobalVolume();

	//1.create many ui components if you need! (they must have a name)
	Option_text = new DW_UIText("Optiontext", lang->GetText("option"), 1.5f, NCL::Maths::Vector3{ 550.0f,600.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // option text	
	std::string str{ NCL::Assets::TEXTUREDIR + "button1.png" };

	Close_btn = new DW_UIImage("CloseButton", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,150.0f,0.0f });  // Close button
	Close_text = new DW_UIText("Closetext", lang->GetText("close"), 0.7f, NCL::Maths::Vector3{ 590.0f,140.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // Close text
	Volume_btn = new DW_UIImage("VolumeBG", str.c_str(), { 0.33f, 0.1f }, NCL::Maths::Vector3{ 630.0f,400.0f,0.0f });  // Volume down button
	Volume_text = new DW_UIText("Volumetext", lang->GetText("volume") + std::to_string((int)(volume * 10)), 0.7f, NCL::Maths::Vector3{ 560.0f,390.0f,0.0f }, NCL::Maths::Vector3{ 1.0f,1.0f,1.0f }); // option text

	std::string str1{ NCL::Assets::TEXTUREDIR + "bt3.png" };
	VolumeUp_btn = new DW_UIImage("VolumeUpButton", str1.c_str(), { 0.1f, 0.1f }, NCL::Maths::Vector3{ 830.0f,400.0f,0.0f });  // Volume up button
	VolumeDown_btn = new DW_UIImage("VolumeDownButton", str1.c_str(), { 0.1f, 0.1f }, NCL::Maths::Vector3{ 430.0f,400.0f,0.0f });  // Volume down button

	str = NCL::Assets::TEXTUREDIR + "bg1.png";
	m_bg = new DW_UIImage("Background", str.c_str(), { 1.82f, 1.0f }, NCL::Maths::Vector3{ 650.0f,360.0f,0.0f });  // Menu background picture


	VolumeUp_text = new DW_UIText("VolumeUp", lang->GetText("volume_up"), 1.5f, NCL::Maths::Vector3{ 815.0f,380.0f,0.0f }, NCL::Maths::Vector3{ 0.86f,0.078f,0.235f }); // Volume up text
	VolumeDown_text = new DW_UIText("VolumeDown", lang->GetText("volume_down"), 1.5f, NCL::Maths::Vector3{ 420.0f,380.0f,0.0f }, NCL::Maths::Vector3{ 0.86f,0.078f,0.235f }); // Volume Down text
	
																																											  //2.create an ui panel (it must have a name)
	m_panel = new DW_UIPanel("OptionMenu");

	//3.add these ui components into the ui panel. 
	//Attention: 
	//if we don't set rendering priority, the rendering order of ui components will follow the order which we add them
	//if we set rendering priority, the rendering order will follow rendering priority. Less priority will render earlier.
	m_panel->AddComponent(m_bg);
	m_panel->AddComponent(VolumeUp_btn);
	m_panel->AddComponent(VolumeDown_btn);
	m_panel->AddComponent(Volume_btn);
	m_panel->AddComponent(Close_btn);

	m_panel->AddComponent(VolumeUp_text);
	m_panel->AddComponent(VolumeDown_text);
	m_panel->AddComponent(Volume_text);
	m_panel->AddComponent(Option_text);
	m_panel->AddComponent(Close_text);

	//4.add callback to this panel, if some of its components be clicked, we can do something here! 
	//event type is panel's name
	std::function<void(std::string)> temp = std::bind(&HM_Option::ClickFunc, this, std::placeholders::_1);
	DW_UIEventMgr::get_instance().RegisterUIEvent(m_panel->GetPanelName(), temp);

	//5.add this panel to a panel container, that means we can have many panels at same time!
	DW_UIRenderer::get_instance().AddPanel(m_panel);
}

HM_Option::~HM_Option() {
	delete m_bg;
	delete VolumeUp_btn;
	delete VolumeDown_btn;
	delete Volume_btn;

	delete VolumeUp_text;
	delete VolumeDown_text;
	delete Volume_text;
	delete Option_text;
	delete m_panel;
}

void HM_Option::ClickFunc(const std::string& str) {
	//which image is clicked, the parameter will equal which image's name, then we can do what we want!
	float epsilon = 0.01f;
	if (str == "CloseButton") {
		this->SetPanelActive(false);
		NCL::CSC8503::AudioSystem::PlaySFX("LQ_Back_Button.wav");
	}
	else if (str == "VolumeUpButton") {
		NCL::CSC8503::AudioSystem::PlaySFX("CGM3_Cute_Chirpy_Button_02_4.wav");
		if (volume < 1.0)
		{
			volume += 0.1;
			Volume_text->SetText("Volume : " + std::to_string((int)(volume * 10)));
			globalAudio->SetGlobalVolume(volume);
		}

	}
	else if (str == "VolumeDownButton") {
		NCL::CSC8503::AudioSystem::PlaySFX("CGM3_Cute_Chirpy_Button_02_4.wav");
		if (volume > 0)
		{
			volume -= 0.1;
			Volume_text->SetText("Volume : " + std::to_string((int)(volume * 10)));
			globalAudio->SetGlobalVolume(volume);
		}
		/*
		if (abs(0.1f - volume) < epsilon) {
			volume = 0;
			Volume_text->SetText("Volume : " + std::to_string(0));
			globalAudio->SetGlobalVolume(volume);
		}*/
	}
	std::cout << str << "\n";
}