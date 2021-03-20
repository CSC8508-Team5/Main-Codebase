#pragma once

#include <fstream>
#include <iostream>

#include <json/json.h>

#include "../../Common/Assets.h"
#include "../../Common/Vector2.h"

#include "JsonManager.h"

using namespace std;
namespace NCL
{
	class SettingsManager : public JsonManager
	{
	public:
		SettingsManager(string settingsFileName = Assets::DATADIR + "settings.json") : JsonManager(settingsFileName) {}
		static bool GetFullScreen() { return m_dataRoot.get("full-screen", false).asBool(); }
		static void SetFullScreen(bool fs) { m_dataRoot["full-screen"] = fs; }
		static Vector2 GetResolution() { return Vector2(m_dataRoot["resolution"].get("width",1280).asInt(), m_dataRoot["resolution"].get("height",720).asInt()); }
	};
}

