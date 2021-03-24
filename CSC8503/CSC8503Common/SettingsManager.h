#pragma once

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

		bool GetFullScreen() { return m_dataRoot.get("full_screen", false).asBool(); }
		void SetFullScreen(bool fs) { m_dataRoot["full_screen"] = fs; }

		bool GetUseBulletEngine() { return m_dataRoot.get("bullet_engine", false).asBool(); }
		void SetUseBulletEngine(bool fs) { m_dataRoot["bullet_engine"] = fs; }

		pair<int,int> GetResolution() { return make_pair(m_dataRoot["resolution"].get("width",1280).asInt(), m_dataRoot["resolution"].get("height",720).asInt()); }
		void SetResolution(pair<int,int> rect) { m_dataRoot["resolution"]["width"] = rect.first; m_dataRoot["resolution"]["height"] = rect.second; }

		string GetLanguage() { return GetString("language","en"); }
		void SetLanguage(string lang) { m_dataRoot["language"] = lang; }

		float GetVolume() { return GetFloat("volume",1.0f); }
		void SetVolume(float vol) { vol = max(vol, 0.0f); vol = min(vol, 1.0f); m_dataRoot["volume"] = vol; }
		void SetVolume(double vol) { vol = max(vol, 0.0); vol = min(vol, 1.0); m_dataRoot["volume"] = vol; }

	};
}

