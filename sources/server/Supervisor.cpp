﻿#include "stdafx.h"
#include "Supervisor.h"
#include "automation/script/ObjectFactory.h"
#include "pluginSystem/Manager.h"
#include "database/Factory.h"
#include <shared/Log.h>
#include "web/poco/WebServer.h"
#include "authentication/BasicAuthentication.h"
#include "web/rest/service/Acquisition.h"
#include "web/rest/service/AutomationRule.h"
#include "web/rest/service/Plugin.h"
#include "web/rest/service/Device.h"
#include "web/rest/service/Page.h"
#include "web/rest/service/Widget.h"
#include "web/rest/service/Configuration.h"
#include "web/rest/service/PluginEventLogger.h"
#include "web/rest/service/EventLogger.h"
#include "web/rest/service/System.h"
#include "web/rest/service/Task.h"
#include "web/rest/service/Recipient.h"
#include "web/rest/service/Update.h"
#include "web/rest/service/Maintenance.h"
#include <shared/ThreadBase.h>
#include "task/Scheduler.h"
#include "communication/PluginGateway.h"
#include "dataAccessLayer/DataAccessLayer.h"
#include "notification/NotificationCenter.h"
#include "automation/RuleManager.h"
#include <shared/ServiceLocator.h>
#include "startupOptions/IStartupOptions.h"
#include "dateTime/DateTimeNotifier.h"


CSupervisor::CSupervisor(const IPathProvider& pathProvider)
   :m_pathProvider(pathProvider)
{
}

CSupervisor::~CSupervisor()
{
}


void CSupervisor::run()
{
   YADOMS_LOG_CONFIGURE("Supervisor");
   YADOMS_LOG(information) << "Supervisor is starting";

   boost::shared_ptr<dataAccessLayer::IDataAccessLayer> dal;
   try
   {
      shared::CServiceLocator::instance().push<automation::script::IObjectFactory>(boost::make_shared<automation::script::ObjectFactory>());

      //create the notification center
      auto notificationCenter(boost::make_shared<notification::CNotificationCenter>());
      shared::CServiceLocator::instance().push<notification::CNotificationCenter>(notificationCenter);

      //retrieve startup options
      auto startupOptions = shared::CServiceLocator::instance().get<startupOptions::IStartupOptions>();

      //start database system
      auto pDataProvider = database::CFactory::create(m_pathProvider);
      if (!pDataProvider->load())
         throw shared::exception::CException("Fail to load database");

      //create the data access layer
      dal = boost::make_shared<dataAccessLayer::CDataAccessLayer>(pDataProvider);
      shared::CServiceLocator::instance().push<dataAccessLayer::IDataAccessLayer>(dal);

      // Create the Plugin manager
      auto pluginManager(boost::make_shared<pluginSystem::CManager>(m_pathProvider,
                                                                    pDataProvider,
                                                                    dal));

      // Start Task manager
      auto taskManager(boost::make_shared<task::CScheduler>(dal->getEventLogger()));
      taskManager->start();

      // Create the update manager
      auto updateManager(boost::make_shared<update::CUpdateManager>(taskManager,
                                                                    pluginManager));

      // Start the plugin gateway
      auto pluginGateway(boost::make_shared<communication::CPluginGateway>(pDataProvider, dal->getAcquisitionHistorizer(), pluginManager));

      // Start the plugin manager (start all plugin instances)
      pluginManager->start();

      // Start automation rules manager
      boost::shared_ptr<automation::IRuleManager> automationRulesManager(boost::make_shared<automation::CRuleManager>(m_pathProvider,
                                                                                                                      pDataProvider->getRuleRequester(),
                                                                                                                      pluginGateway,
                                                                                                                      pDataProvider->getAcquisitionRequester(),
                                                                                                                      pDataProvider->getDeviceRequester(),
                                                                                                                      dal->getKeywordManager(),
                                                                                                                      pDataProvider->getRecipientRequester(),
                                                                                                                      dal->getConfigurationManager(),
                                                                                                                      dal->getEventLogger()));
      shared::CServiceLocator::instance().push<automation::IRuleManager>(automationRulesManager);

      // Start Web server
      const auto webServerIp = startupOptions->getWebServerIPAddress();
      const auto webServerPort = boost::lexical_cast<std::string>(startupOptions->getWebServerPortNumber());
      const auto webServerPath = m_pathProvider.webServerPath().string();
      const auto scriptInterpretersPath = m_pathProvider.scriptInterpretersPath().string();

      auto webServer(boost::make_shared<web::poco::CWebServer>(webServerIp,
                                                               webServerPort,
                                                               webServerPath,
                                                               "/rest/",
                                                               "/ws"));

      webServer->getConfigurator()->websiteHandlerAddAlias("plugins", m_pathProvider.pluginsPath().string());
      webServer->getConfigurator()->websiteHandlerAddAlias("scriptInterpreters", scriptInterpretersPath);

      if (pDataProvider->getDatabaseRequester()->backupSupported())
      {
         std::string filename = m_pathProvider.databaseSqliteBackupFile().filename().string();
         webServer->getConfigurator()->websiteHandlerAddAlias(filename, m_pathProvider.databaseSqliteBackupFile().string());
      }
            
      webServer->getConfigurator()->configureAuthentication(boost::make_shared<authentication::CBasicAuthentication>(dal->getConfigurationManager(), startupOptions->getNoPasswordFlag()));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CPlugin>(pDataProvider, pluginManager, *pluginGateway));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CDevice>(pDataProvider, *pluginGateway));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CPage>(pDataProvider));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CWidget>(pDataProvider, webServerPath));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CConfiguration>(dal->getConfigurationManager()));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CPluginEventLogger>(pDataProvider));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CEventLogger>(dal->getEventLogger()));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CSystem>());
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CAcquisition>(pDataProvider));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CAutomationRule>(pDataProvider, automationRulesManager));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CTask>(taskManager));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CRecipient>(pDataProvider));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CUpdate>(updateManager));
      webServer->getConfigurator()->restHandlerRegisterService(boost::make_shared<web::rest::service::CMaintenance>(pDataProvider->getDatabaseRequester(), taskManager));

      webServer->start();

      // Register to event logger started event
      dal->getEventLogger()->addEvent(database::entities::ESystemEventCode::kStarted, "yadoms", shared::CStringExtension::EmptyString);

      //create and start the dateTime notification scheduler
      dateTime::CDateTimeNotifier dateTimeNotificationService;
      dateTimeNotificationService.start();

      // Main loop
      YADOMS_LOG(information) << "Supervisor is running...";
      while (m_EventHandler.waitForEvents() != kStopRequested){}

      YADOMS_LOG(information) << "Supervisor is stopping...";

      //stop datetime notification service
      dateTimeNotificationService.stop();

      //stop web server
      webServer->stop();
      webServer.reset();

      //stop the automation rules
      shared::CServiceLocator::instance().remove<automation::IRuleManager>(automationRulesManager);
      automationRulesManager->stop();
      automationRulesManager.reset();

      //stop task manager
      taskManager->stop();
      taskManager.reset();

      //stop all plugins
      //force to stop all plugin, the reset() will call stop only at pluginManager deletion, which could happen in the future if it is used elsewhere
      pluginManager->stop();
      pluginManager.reset();

      //stop database tasks
      pDataProvider->stopMaintenanceTasks();

      YADOMS_LOG(information) << "Supervisor is stopped";

      dal->getEventLogger()->addEvent(database::entities::ESystemEventCode::kStopped, "yadoms", shared::CStringExtension::EmptyString);
   }
   catch (std::exception& e)
   {
      YADOMS_LOG(error) << "Supervisor : unhandled exception " << e.what();
      if (dal && dal->getEventLogger())
         dal->getEventLogger()->addEvent(database::entities::ESystemEventCode::kYadomsCrash, "yadoms", e.what());
   }
   catch (...)
   {
      YADOMS_LOG(error) << "Supervisor : unhandled exception.";
      if (dal && dal->getEventLogger())
         dal->getEventLogger()->addEvent(database::entities::ESystemEventCode::kYadomsCrash, "yadoms", "unknwon error");
   }

   //notify application that supervisor ends
   YADOMS_LOG(debug) << "Supervisor stopped";
}

void CSupervisor::requestToStop()
{
   m_EventHandler.postEvent(kStopRequested);
}

