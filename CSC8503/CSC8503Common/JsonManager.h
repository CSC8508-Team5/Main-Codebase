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
		static bool GetBool(string key, bool defaultValue = false, string subTree = "")
		{
			if (subTree.empty())
				return m_dataRoot.get(key, defaultValue).asBool();
			else
				return m_dataRoot[subTree].get(key, defaultValue).asBool();
		}
		static int GetInt(string key, int defaultValue = 0, string subTree = "")
		{ 
			if (subTree.empty())
			return m_dataRoot.get(key, defaultValue).asInt();
			else
				return m_dataRoot[subTree].get(key, defaultValue).asInt();
		}
		static float GetFloat(string key,float defaultValue = 0.0f, string subTree = "")
		{ 
			if (subTree.empty())
			return m_dataRoot.get(key, defaultValue).asFloat(); 
			else
				return m_dataRoot[subTree].get(key, defaultValue).asFloat();
		}
		static double GetDouble(string key, float defaultValue = 0.0f, string subTree = "")
		{
			if (subTree.empty())
				return m_dataRoot.get(key, defaultValue).asDouble();
			else
				return m_dataRoot[subTree].get(key, defaultValue).asDouble();
		}
		static string GetString(string key,string defaultValue = "null", string subTree = "")
		{ 
			if (subTree.empty())
				return m_dataRoot.get(key, defaultValue).asString(); 
			else
				return m_dataRoot[subTree].get(key, defaultValue).asString();
		}

		template <typename T>
		static void SetData(string key, T& data, string subTree = "")
		{
			m_dataRoot[subTree][key] = data;
		}

		static void ForceSave() { save(m_fileName); }
		static void ForceAndReset() { save(m_fileName); m_dataRoot.clear();}

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
		static bool save(string filename);
		bool m_initialized = false;
		bool m_autoSave;
		static string m_fileName;
		static Json::Value m_dataRoot;

	};
}

