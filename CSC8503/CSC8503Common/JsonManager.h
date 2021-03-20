#pragma once

#include <fstream>
#include <iostream>

#include <json/json.h>

#include "../../Common/Assets.h"

using namespace std;
namespace NCL
{
	class JsonManager
	{
	public:
		JsonManager(string filename, bool autoSaveOnDelete = true);
		~JsonManager();

		bool GetSettingsInitialized() { return m_initialized; }

		template <typename T>
		static void GetData(string key, T& data, string subTree = "") 
		{
			if(subTree.empty())
				data = m_dataRoot.get(key, (T)0).as<T>(); 
			else
				data = m_dataRoot[subTree].get(key, (T)0).as<T>();
		}
		static bool GetBool(string key, string subTree, bool defaultValue = false) 
		{ 
			if (subTree.empty())
				return m_dataRoot.get(key, defaultValue).asBool(); 
			else
				return m_dataRoot[subTree].get(key, defaultValue).asBool();
		}
		static int GetInt(string key, string subTree, int defaultValue = 0) 
		{ 
			if (subTree.empty())
			return m_dataRoot.get(key, defaultValue).asInt();
			else
				return m_dataRoot[subTree].get(key, defaultValue).asInt();
		}
		static float GetFloat(string key, string subTree,float defaultValue = 0.0f)
		{ 
			if (subTree.empty())
			return m_dataRoot.get(key, defaultValue).asFloat(); 
			else
				return m_dataRoot[subTree].get(key, defaultValue).asFloat();
		}
		static string GetString(string key, string subTree,string defaultValue = "null") 
		{ 
			if (subTree.empty())
				return m_dataRoot.get(key, defaultValue).asString(); 
			else
				return m_dataRoot[subTree].get(key, defaultValue).asString();
		}

		void ForceSave() { save(m_fileName); }

		virtual void ConstructDefault() 
		{
			m_dataRoot["Version"] = "1.9.4";
		};

		void GetIterators(Json::Value::iterator& first, Json::Value::iterator& last, string subTree="") const 
		{ 
			if (subTree.empty())
			{
				first = m_dataRoot.begin();
				last = m_dataRoot.end();
			}
			else
			{
				first = m_dataRoot[subTree].begin();
				last = m_dataRoot[subTree].end();
			}
		}
	protected:

		bool load(string filename);
		bool save(string filename);
		bool m_initialized = false;
		bool m_autoSave;
		string m_fileName;
		static Json::Value m_dataRoot;

	};
}

