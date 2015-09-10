#include "stdafx.h"
#include "Factory.h"
#include "InterpreterNotFound.hpp"
#include <shared/Log.h>
#include <shared/exception/InvalidParameter.hpp>
#include <shared/exception/OutOfRange.hpp>
#include <shared/script/IInterpreter.h>
#include <shared/DynamicLibrary.h>
#include "InterpreterLibrary.h"
#include "InternalScriptApiImplementation.h"
#include "GeneralInfo.h"
#include "Properties.h"
#include "Logger.h"
#include "tools/SupportedPlatformsChecker.h"

namespace automation { namespace script
{

CFactory::CFactory(const std::string& interpretersPath,
      boost::shared_ptr<communication::ISendMessageAsync> pluginGateway,
      boost::shared_ptr<dataAccessLayer::IConfigurationManager> configurationManager,
      boost::shared_ptr<database::IAcquisitionRequester> dbAcquisitionRequester,
      boost::shared_ptr<database::IDeviceRequester> dbDeviceRequester,
      boost::shared_ptr<database::IKeywordRequester> dbKeywordRequester)
   :m_interpretersPath(interpretersPath),
   m_pluginGateway(pluginGateway),
   m_configurationManager(configurationManager),
   m_dbAcquisitionRequester(dbAcquisitionRequester),
   m_dbDeviceRequester(dbDeviceRequester),
   m_dbKeywordRequester(dbKeywordRequester),
   m_generalInfo(new CGeneralInfo(configurationManager))
{
}

CFactory::~CFactory()
{         
}

void CFactory::loadInterpreters()
{
   boost::lock_guard<boost::recursive_mutex> lock(m_loadedInterpretersMutex);

   std::vector<boost::filesystem::path> interpreterDirectories = findInterpreterDirectories();

   for (std::vector<boost::filesystem::path>::const_iterator interpreterDirectory = interpreterDirectories.begin();
      interpreterDirectory != interpreterDirectories.end(); ++interpreterDirectory)
   {
      std::string interperterKeyName = interpreterDirectory->filename().string();
      if (m_loadedInterpreters.find(interperterKeyName) == m_loadedInterpreters.end())
      {
         // Not already loaded

         // Check if compatible with current platform
         try
         {
            if (isInterpreterCompatibleWithPlatform(interperterKeyName))
            {
               boost::shared_ptr<IInterpreterLibrary> library(new CInterpreterLibrary(toLibraryPath(interpreterDirectory->filename().string())));
               m_loadedInterpreters[interperterKeyName] = library;
            }
         }
         catch (shared::exception::CInvalidParameter& e)
         {
            YADOMS_LOG(warning) << "Interpreter " << interpreterDirectory->filename().string() << " is not valid and will not be loaded : " << e.what();
         }
      }
   }
}

bool CFactory::isInterpreterCompatibleWithPlatform(const std::string& interpreterName) const
{
   shared::CDataContainer container;
   try
   {
      boost::filesystem::path packageFile;
      packageFile = m_interpretersPath / interpreterName / "package.json";
      container.deserializeFromFile(packageFile.string());
   }
   catch (shared::exception::CException& e)
   {
      throw shared::exception::CInvalidParameter(std::string("Error reading package.json : ") + e.what());   	
   }

   return tools::CSupportedPlatformsChecker::isSupported(container.get<shared::CDataContainer>("supportedPlatforms"));
}

boost::filesystem::path CFactory::toLibraryPath(const std::string& interpreterName) const
{
   boost::filesystem::path path(m_interpretersPath);
   path /= interpreterName;
   path /= shared::CDynamicLibrary::ToFileName(interpreterName);
   return path;
}

std::vector<boost::filesystem::path> CFactory::findInterpreterDirectories()
{
   // Look for all subdirectories in m_interpretersPath directory, where it contains library with same name,
   // for example a subdirectory "Python" containing a "Python.dll|so" file
   std::vector<boost::filesystem::path> interpreters;

   if (boost::filesystem::exists(m_interpretersPath) && boost::filesystem::is_directory(m_interpretersPath))
   {
      // Check all subdirectories in m_pluginPath
      for(boost::filesystem::directory_iterator subDirIterator(m_interpretersPath) ; subDirIterator != boost::filesystem::directory_iterator() ; ++subDirIterator)
      {
         if (boost::filesystem::is_directory(subDirIterator->status()))
         {
            // Subdirectory, check if it is a interpreter (= contains a dynamic library with same name)
            const std::string expectedLibName(shared::CDynamicLibrary::ToFileName(subDirIterator->path().filename().string()));
            for (boost::filesystem::directory_iterator fileIterator(subDirIterator->path()); fileIterator != boost::filesystem::directory_iterator() ; ++fileIterator)
            {
               if (boost::filesystem::is_regular_file(fileIterator->status()) &&                // It's a file...
                  boost::iequals(fileIterator->path().filename().string(), expectedLibName))    // ...with the same name as sub-directory...
               {
                  interpreters.push_back(subDirIterator->path());
               }
            }
         }
      }
   }

   return interpreters;
}

std::vector<std::string> CFactory::getAvailableInterpreters() 
{
   boost::lock_guard<boost::recursive_mutex> lock(m_loadedInterpretersMutex);

   std::vector<std::string> interpreters;

   // Update loaded interpreters list if necessary
   loadInterpreters();

   // Now find corresponding interpreter
   for (std::map<std::string, boost::shared_ptr<IInterpreterLibrary> >::const_iterator itInterpreter = m_loadedInterpreters.begin();
      itInterpreter != m_loadedInterpreters.end(); ++itInterpreter)
   {
      boost::shared_ptr<shared::script::IInterpreter> interpreter(itInterpreter->second->getInterpreter());
      interpreters.push_back(interpreter->name());
   }

   return interpreters;
}

void CFactory::unloadInterpreter(const std::string& interpreterName)
{
   boost::lock_guard<boost::recursive_mutex> lock(m_loadedInterpretersMutex);

   std::map<std::string, boost::shared_ptr<IInterpreterLibrary> >::const_iterator interpreter = m_loadedInterpreters.find(interpreterName);
   if (interpreter != m_loadedInterpreters.end())
      m_loadedInterpreters.erase(interpreter);
}

boost::shared_ptr<shared::script::IInterpreter> CFactory::getAssociatedInterpreter(const std::string& interpreterName)
{
   boost::lock_guard<boost::recursive_mutex> lock(m_loadedInterpretersMutex);

   // Update loaded interpreters list if necessary
   loadInterpreters();

   // Now find corresponding interpreter (warning interpreter name is the name returned by the interpreter, not the map key, which is the interpreter filename)
   for (std::map<std::string, boost::shared_ptr<IInterpreterLibrary> >::const_iterator itInterpreter = m_loadedInterpreters.begin(); itInterpreter != m_loadedInterpreters.end(); ++itInterpreter)
   {
      boost::shared_ptr<shared::script::IInterpreter> interpreter(itInterpreter->second->getInterpreter());
      if (interpreter->name() == interpreterName)
      {
         if (!interpreter->isAvailable())
            throw CInterpreterNotFound(interpreterName);

         return interpreter;
      }
   }

   throw CInterpreterNotFound(interpreterName);
}


boost::shared_ptr<IProperties> CFactory::createScriptProperties(boost::shared_ptr<const database::entities::CRule> ruleData)
{
   boost::shared_ptr<IProperties> properties(new CProperties(ruleData));
   return properties;
}

std::string CFactory::getScriptFile(boost::shared_ptr<const database::entities::CRule> ruleData)
{
   boost::shared_ptr<IProperties> scriptProperties(createScriptProperties(ruleData));

   // Create the file and put the code in (delegate to the interpreter)
   boost::shared_ptr<shared::script::IInterpreter> scriptInterpreter = getAssociatedInterpreter(scriptProperties->interpreterName());
   return scriptInterpreter->loadScriptContent(scriptProperties->scriptPath()); 
}

void CFactory::updateScriptFile(boost::shared_ptr<const database::entities::CRule> ruleData, const std::string& code)
{
   boost::shared_ptr<IProperties> scriptProperties(createScriptProperties(ruleData));

   // First build directory tree
   boost::filesystem::remove_all(scriptProperties->scriptPath());
   boost::filesystem::create_directories(scriptProperties->scriptPath());

   // Create the file and put the code in (delegate to the interpreter)
   boost::shared_ptr<shared::script::IInterpreter> scriptInterpreter = getAssociatedInterpreter(scriptProperties->interpreterName());
   scriptInterpreter->saveScriptContent(scriptProperties->scriptPath(), code);
}

void CFactory::deleteScriptFile(boost::shared_ptr<const database::entities::CRule> ruleData, bool doBackup)
{
   boost::shared_ptr<IProperties> scriptProperties(createScriptProperties(ruleData));

   if (doBackup)
   {
      const std::string backupPath(scriptProperties->scriptPath() + ".bak");
      boost::filesystem::remove_all(backupPath);
      boost::filesystem::rename(scriptProperties->scriptPath(), backupPath);
   }
   else
   {
      boost::filesystem::remove_all(scriptProperties->scriptPath());
   }
}

std::string CFactory::getScriptLogFile(boost::shared_ptr<const database::entities::CRule> ruleData)
{
   boost::shared_ptr<IProperties> scriptProperties(createScriptProperties(ruleData));
   const boost::filesystem::path scriptLogFile(CLogger::logFile(scriptProperties->scriptPath()));

   if (!boost::filesystem::exists(scriptLogFile))
      throw shared::exception::CInvalidParameter(scriptLogFile.string());

   std::ifstream file(scriptLogFile.string().c_str());
   if (!file.is_open())
      throw shared::exception::CInvalidParameter(scriptLogFile.string());

   std::istreambuf_iterator<char> eos;
   return std::string(std::istreambuf_iterator<char>(file), eos);
}

boost::shared_ptr<shared::script::IRunner> CFactory::createScriptRunner(boost::shared_ptr<const IProperties> scriptProperties)
{
   try
   {
      boost::shared_ptr<shared::script::IInterpreter> scriptInterpreter = getAssociatedInterpreter(scriptProperties->interpreterName());

      boost::filesystem::path interpreterPath;
      interpreterPath = m_interpretersPath / scriptProperties->interpreterName();

      return scriptInterpreter->createRunner(scriptProperties->scriptPath(), scriptProperties->configuration(), interpreterPath.string());
   }
   catch (CInterpreterNotFound& e)
   {
      YADOMS_LOG(error) << "Unable to run script : " << e.what();
      throw shared::exception::CInvalidParameter("Unable to run script");
   }
   catch (shared::exception::COutOfRange& e)
   {
      YADOMS_LOG(error) << "Script type is not supported : " << e.what();
      throw shared::exception::CInvalidParameter("script type");
   }
}

boost::shared_ptr<shared::script::ILogger> CFactory::createScriptLogger(const std::string& scriptPath)
{
   boost::shared_ptr<shared::script::ILogger> logger(new CLogger(scriptPath));
   return logger;
}

boost::shared_ptr<IInternalScriptApiImplementation> CFactory::createScriptContext(boost::shared_ptr<shared::script::ILogger> scriptLogger)
{
   boost::shared_ptr<IInternalScriptApiImplementation> context(
      new InternalScriptApiImplementation(scriptLogger, m_pluginGateway, m_configurationManager, m_dbAcquisitionRequester, m_dbDeviceRequester, m_dbKeywordRequester, m_generalInfo));
   return context;
}

} } // namespace automation::script

