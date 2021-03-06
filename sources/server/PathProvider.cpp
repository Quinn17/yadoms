﻿#include "stdafx.h"
#include "PathProvider.h"

CPathProvider::CPathProvider(const boost::shared_ptr<startupOptions::IStartupOptions> startupOptions)
   : m_logsPath("logs"),
     m_dataPath("data"),
     m_webServerPath(startupOptions->getWebServerInitialPath()),
     m_pluginsLogPath(m_logsPath / "plugins"),
     m_pluginsDataPath(m_dataPath / "plugins"),
     m_scriptsLogPath(m_logsPath / "scripts"),
     m_pluginsPath(startupOptions->getPluginsPath()),
     m_scriptsPath("scripts"),
     m_scriptInterpreters(startupOptions->getScriptInterpretersPath()),
     m_databaseSqliteFile(startupOptions->getDatabaseSqliteFile()),
     m_databaseSqliteBackupFile(startupOptions->getDatabaseSqliteBackupFile())
{
   if (!boost::filesystem::exists(m_logsPath))
      boost::filesystem::create_directory(m_logsPath);
   if (!boost::filesystem::exists(m_dataPath))
      boost::filesystem::create_directory(m_dataPath);
   if (!boost::filesystem::exists(m_pluginsLogPath))
      boost::filesystem::create_directory(m_pluginsLogPath);
   if (!boost::filesystem::exists(m_pluginsDataPath))
      boost::filesystem::create_directory(m_pluginsDataPath);
   if (!boost::filesystem::exists(m_scriptsLogPath))
      boost::filesystem::create_directory(m_scriptsLogPath);
   if (!boost::filesystem::exists(m_pluginsPath))
      boost::filesystem::create_directory(m_pluginsPath);
   if (!boost::filesystem::exists(m_scriptsPath))
      boost::filesystem::create_directory(m_scriptsPath);
   if (!boost::filesystem::exists(m_scriptInterpreters))
      boost::filesystem::create_directory(m_scriptInterpreters);
}

CPathProvider::~CPathProvider()
{
}


const boost::filesystem::path& CPathProvider::logsPath() const
{
   return m_logsPath;
}

const boost::filesystem::path& CPathProvider::webServerPath() const
{
   return m_webServerPath;
}

const boost::filesystem::path& CPathProvider::pluginsLogPath() const
{
   return m_pluginsLogPath;
}

const boost::filesystem::path& CPathProvider::pluginsDataPath() const
{
   return m_pluginsDataPath;
}

const boost::filesystem::path& CPathProvider::pluginsPath() const
{
   return m_pluginsPath;
}
const boost::filesystem::path& CPathProvider::scriptsPath() const
{
   return m_scriptsPath;
}

const boost::filesystem::path& CPathProvider::scriptsLogPath() const
{
   return m_scriptsLogPath;
}

const boost::filesystem::path& CPathProvider::scriptInterpretersPath() const
{
   return m_scriptInterpreters;
}

const boost::filesystem::path& CPathProvider::databaseSqliteFile() const
{
   return m_databaseSqliteFile;
}

const boost::filesystem::path& CPathProvider::databaseSqliteBackupFile() const
{
   return m_databaseSqliteBackupFile;
}