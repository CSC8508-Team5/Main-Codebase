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

		static bool GetFullScreen() { return m_dataRoot.get("full-screen", false).asBool(); }
		static void SetFullScreen(bool fs) { m_dataRoot["full-screen"] = fs; }

		static Vector2 GetResolution() { return Vector2(m_dataRoot["resolution"].get("width",1280).asInt(), m_dataRoot["resolution"].get("height",720).asInt()); }
		static void SetResolution(Vector2 rect) { m_dataRoot["resolution"]["width"] = rect.x; m_dataRoot["resolution"]["height"] = rect.y; }

		static string GetLanguage() { return GetString("language","en"); }
		static void SetLanguage(string lang) { m_dataRoot["language"] = lang; }

		static float GetVolume() { return GetFloat("volume",1.0f); }
		static void SetVolume(float vol) { vol = max(vol, 0.0f); vol = min(vol, 1.0f); m_dataRoot["volume"] = vol; }
		static void SetVolume(double vol) { vol = max(vol, 0.0); vol = min(vol, 1.0); m_dataRoot["volume"] = vol; }

	};
}

