#include "JsonManager.h"

Json::Value NCL::JsonManager::m_dataRoot;

NCL::JsonManager::JsonManager(string filename, bool autoSaveOnDelete)
{
	m_fileName = filename;
	m_autoSave = autoSaveOnDelete;
	m_initialized = load(filename);
}

NCL::JsonManager::~JsonManager()
{
	if (m_autoSave)
	{
		save(m_fileName);
	}
}

bool NCL::JsonManager::load(string filename)
{
	std::ifstream ifs(filename, std::ifstream::binary);

	if (ifs.is_open())
	{
		Json::CharReaderBuilder builder;
		builder["collectComments"] = true;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, ifs, &m_dataRoot, &errs)) {
			std::cout << "Json Manager Error: " << errs << std::endl;
			ifs.close();
			return false;
		}
		ifs.close();
		return true;
	}
	else
	{
		ConstructDefault();
		return true;
	}
}

bool NCL::JsonManager::save(string filename)
{
	std::ofstream ofs(filename, std::ofstream::binary | std::ofstream::out | ios::trunc);

	if (ofs.is_open())
	{
		Json::StreamWriterBuilder builder;
		const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(m_dataRoot, &ofs);
		//writer->write(m_dataRoot, &std::cout);
		ofs.close();
	}

	return true;
}
