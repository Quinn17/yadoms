#pragma once

#include "IRestService.h"
#include "database/IDataProvider.h"
#include "pluginSystem/Manager.h"
#include "communication/ISendMessageAsync.h"

namespace web { namespace rest { namespace service {

   class CPlugin : public IRestService
   {
   public:
      explicit CPlugin(boost::shared_ptr<database::IDataProvider> dataProvider, boost::shared_ptr<pluginSystem::CManager> pluginManager, communication::ISendMessageAsync & messageSender);
      virtual ~CPlugin();

   public:
      // IRestService implementation
      virtual void configureDispatcher(CRestDispatcher & dispatcher);
      // [END] IRestService implementation

      const std::string & getRestKeyword();
      shared::CDataContainer getOnePlugin(const std::vector<std::string> & parameters, const std::string & requestContent);   
      shared::CDataContainer getAllPluginsInstance(const std::vector<std::string> & parameters, const std::string & requestContent);  
      shared::CDataContainer getAllPluginsInstanceForManualDeviceCreation(const std::vector<std::string> & parameters, const std::string & requestContent);
      shared::CDataContainer getAllAvailablePlugins(const std::vector<std::string> & parameters, const std::string & requestContent) const;  
      shared::CDataContainer getAllAvailablePluginsWithPackage(const std::vector<std::string> & parameters, const std::string & requestContent);
      shared::CDataContainer getPluginDevices(const std::vector<std::string> & parameters, const std::string & requestContent);
      shared::CDataContainer createPlugin(const std::vector<std::string> & parameters, const std::string & requestContent);  
      shared::CDataContainer updatePlugin(const std::vector<std::string> & parameters, const std::string & requestContent);  
      shared::CDataContainer deletePlugin(const std::vector<std::string> & parameters, const std::string & requestContent);  
      shared::CDataContainer deleteAllPlugins(const std::vector<std::string> & parameters, const std::string & requestContent);  
      shared::CDataContainer sendExtraCommand(const std::vector<std::string> & parameters, const std::string & requestContent);
      shared::CDataContainer getInstanceState(const std::vector<std::string> & parameters, const std::string & requestContent);
      shared::CDataContainer startInstance(const std::vector<std::string> & parameters, const std::string & requestContent);
      shared::CDataContainer stopInstance(const std::vector<std::string> & parameters, const std::string & requestContent);   
      shared::CDataContainer getInstanceLog(const std::vector<std::string> & parameters, const std::string & requestContent);

      shared::CDataContainer createDevice(const std::vector<std::string> & parameters, const std::string & requestContent);
      
      shared::CDataContainer getBinding(const std::vector<std::string> & parameters, const std::string & requestContent);

      shared::CDataContainer transactionalMethod(CRestDispatcher::CRestMethodHandler realMethod, const std::vector<std::string> & parameters, const std::string & requestContent);

   private:
      boost::shared_ptr<database::IDataProvider> m_dataProvider;
      boost::shared_ptr<pluginSystem::CManager> m_pluginManager;
      std::string m_restKeyword;

      //-----------------------------------------
      ///\brief   The send message interface
      //-----------------------------------------
      communication::ISendMessageAsync & m_messageSender;
   };



} //namespace service
} //namespace rest
} //namespace web 
