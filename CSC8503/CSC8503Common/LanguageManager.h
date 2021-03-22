#pragma once
#include "JsonManager.h"
namespace NCL
{
	class LanguageManager :
		public JsonManager
	{
	public:
		LanguageManager(string language="en", string languageFileLocation = Assets::DATADIR, string filePrefix = "lang_")
			: JsonManager(languageFileLocation + filePrefix + language+".json", false) {
			m_language = language;
		}
		
		string GetText(string key = "", string subTree = "",string defaultStr = "") { return GetString(key, defaultStr, subTree); }
		string GetLanguage() { return m_language; }

	protected:
		string m_language;
		Json::Value m_dataRoot;
	};
}

