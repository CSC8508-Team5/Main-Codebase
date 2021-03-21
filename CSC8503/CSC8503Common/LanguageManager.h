#pragma once
#include "JsonManager.h"
namespace NCL
{
	class LanguageManager :
		public JsonManager
	{
	public:
		LanguageManager(string languageFileLocation = Assets::DATADIR, string language="en")
			: JsonManager(languageFileLocation+"lang_"+language+".json",false) {
			m_language = language;
		}
		
		string GetText(string key = "", string defaultStr = "") { return GetString(key, defaultStr); }
		string GetLanguage() { return m_language; }

	protected:
		string m_language;
	};
}

