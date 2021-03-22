#include "HM_Loading.h"
#include "../../Common/Window.h"

HM_Loading::HM_Loading()
{
	std::string str = "../../Assets/Textures/bg.png";

	//m_bg1 = new DW_UIImage("Background", str.c_str(), { 1.82f, 1.0f }, NCL::Maths::Vector3{ 650.0f,360.0f,0.0f });  // Menu background picture
	m_panel1 = new DW_UIPanel("LoadingMenu");

	//m_panel1->AddComponent(m_bg1);


	//5.add this panel to a panel container, that means we can have many panels at same time!
	DW_UIRenderer::get_instance().AddPanel(m_panel1);
}

HM_Loading::~HM_Loading()
{
	delete m_bg1;

}
