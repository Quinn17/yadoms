//
// Manager.h
//
// Yadoms-plugin manager
//
#pragma once
#include "IFactory.h"
#include "IInstance.h"
#include "database/IDataProvider.h"
#include "database/IPluginRequester.h"
#include <shared/event/EventHandler.hpp>
#include "dataAccessLayer/IDataAccessLayer.h"
#include <shared/plugin/yPluginApi/IBindingQueryRequest.h>
#include <shared/plugin/yPluginApi/IManuallyDeviceCreationRequest.h>
#include <shared/plugin/yPluginApi/IDeviceCommand.h>
#include <shared/plugin/yPluginApi/IExtraCommand.h>
#include <shared/plugin/yPluginApi/historization/PluginState.h>
#include "InstanceRemover.h"
#include <IPathProvider.h>

namespace pluginSystem
{
   //--------------------------------------------------------------
   /// \brief	this class is used to manage plugin. 
   ///         It search for plugins into directories and generate plugin factories
   //--------------------------------------------------------------
   class CManager
   {
   public:
      //--------------------------------------------------------------
      /// \brief			Constructor (protected, use newManager to create instance)
      /// \param [in]   pathProvider            Yadoms paths provider
      /// \param [in]   dataProvider            Database link
      /// \param [in]   dataAccessLayer         The database access layer
      //--------------------------------------------------------------
      CManager(const IPathProvider& pathProvider,
               boost::shared_ptr<database::IDataProvider> dataProvider,
               boost::shared_ptr<dataAccessLayer::IDataAccessLayer> dataAccessLayer);

      //--------------------------------------------------------------
      /// \brief			Start the manager (try to start all active plugins)
      /// \param[in]    timeout  Timeout waiting plugins to start
      /// \details      This function is blocking until all plugins are started or timeout
      //--------------------------------------------------------------
      void start(const boost::posix_time::time_duration& timeout);

      //--------------------------------------------------------------
      /// \brief			Destructor
      //--------------------------------------------------------------
      virtual ~CManager();

      //--------------------------------------------------------------
      /// \brief			Stop all plugin instance
      //--------------------------------------------------------------
      void stop();

      //--------------------------------------------------------------
      /// \brief           Read the available plugin list
      /// \return          The available plugin map (with informations)
      //--------------------------------------------------------------
      IFactory::AvailablePluginMap getPluginList() const;

      //--------------------------------------------------------------
      /// \brief           Read the plugin quality indicator
      /// \param [in] pluginName  Plugin name
      /// \return          The plugin quality indicator, from 0 (worst) to 100 (best),
      ///                  or IQualifier::kNoEnoughData if no measure available.
      /// \throw           CInvalidPluginException if plugin is not available
      //--------------------------------------------------------------
      int getPluginQualityIndicator(const std::string& pluginName) const;

      //--------------------------------------------------------------
      /// \brief           Create a new instance of a plugin
      /// \param [in] data Data fo the plugin instance to create (instance name, plugin name, configuration, etc...)
      /// \return          Id of the created instance
      /// \throw           CException if fails
      /// \throw           CDatabaseException if duplicate record (instance name must be unique)
      //--------------------------------------------------------------
      int createInstance(const database::entities::CPlugin& data);

      //--------------------------------------------------------------
      /// \brief           Delete a plugin instance
      /// \param [in] id   Instance to delete
      /// \throw           CInvalidParameter if instance id is unknown
      //--------------------------------------------------------------
      void deleteInstance(int id);

      //--------------------------------------------------------------
      /// \brief           Get the log of a plugin instance
      /// \param [in] id   Instance to get the log
      /// \return          The log of the instance, if available (empty string if not)
      //--------------------------------------------------------------
      std::string getInstanceLog(int id);

      //--------------------------------------------------------------
      /// \brief           Get the plugin instances list
      /// \return          List of instances ID of all known instances, started or not
      //--------------------------------------------------------------
      std::vector<boost::shared_ptr<database::entities::CPlugin> > getInstanceList() const;

      //--------------------------------------------------------------
      /// \brief           Get the instance configuration
      /// \param [in] id   Instance Id
      /// \return          The instance configuration, if available (empty string if not)
      /// \throw           CException if fails
      //--------------------------------------------------------------
      boost::shared_ptr<database::entities::CPlugin> getInstance(int id) const;

      //--------------------------------------------------------------
      /// \brief           Get a running instance
      /// \param [in] id   Instance Id
      /// \return          The corresponding instance
      /// \throw           CPluginException if instance is unknown or not running
      //--------------------------------------------------------------
      boost::shared_ptr<IInstance> getRunningInstance(int id) const;

      //--------------------------------------------------------------
      /// \brief           Get the instance configuration
      /// \param [in] id   Instance Id
      /// \param [in] newConfiguration   The instance new configuration
      /// \throw           CNotSupported if request to apply unsupported modifications
      /// \throw           CException if fails
      //--------------------------------------------------------------
      void updateInstance(const database::entities::CPlugin& newData);

      //--------------------------------------------------------------
      /// \brief           Start a registered instance of plugin
      /// \param [in] id   Instance Id
      /// \throw           CInvalidParameter if id is unknown
      /// \note            Just start instance, doesn't modify data in base
      //--------------------------------------------------------------
      void startInstance(int id);

      //--------------------------------------------------------------
      /// \brief           Stop a running instance of plugin
      /// \param [in] id   Instance Id
      /// \throw           CInvalidParameter if id is unknown
      //--------------------------------------------------------------
      void requestStopInstance(int id);

      //--------------------------------------------------------------
      /// \brief           Kill a running instance of plugin
      /// \param [in] id   Instance Id
      /// \throw           CInvalidParameter if id is unknown
      //--------------------------------------------------------------
      void killInstance(int id);

      //--------------------------------------------------------------
      /// \brief           Get the running state of a particular instance of plugin
      /// \param [in] id   Instance Id
      /// \return          true if instance is running
      /// \throw           CInvalidParameter if id is unknown
      //--------------------------------------------------------------
      bool isInstanceRunning(int id) const;

      //--------------------------------------------------------------
      /// \brief           Get the state of a particular instance of plugin
      /// \param [in] id   Instance Id
      /// \return          The instance state
      /// \throw           CInvalidParameter if id is unknown
      //--------------------------------------------------------------
      shared::plugin::yPluginApi::historization::EPluginState getInstanceState(int id) const;

      //--------------------------------------------------------------
      /// \brief           Get the state of a particular instance of plugin (with message Id if any)
      /// \param [in] id   Instance Id
      /// \return          The instance state data
      /// \throw           CInvalidParameter if id is unknown
      //--------------------------------------------------------------
      shared::CDataContainer getInstanceFullState(int id) const;


      //--------------------------------------------------------------
      /// \brief        Update the plugin list if needed (after plugin installation for example)
      //--------------------------------------------------------------
      void updatePluginList();

      //--------------------------------------------------------------
      /// \brief                 Post a command to a device on a specific plugin
      /// \param [in] id         Plugin instance Id
      /// \param [in] command    The command to post
      //--------------------------------------------------------------
      void postCommand(int id, boost::shared_ptr<const shared::plugin::yPluginApi::IDeviceCommand> command) const;

      //--------------------------------------------------------------
      /// \brief                 Post an extra command to a device on a specific plugin
      /// \param [in] id         Plugin instance Id
      /// \param [in] command    The command to post
      //--------------------------------------------------------------
      void postExtraCommand(int id, boost::shared_ptr<const shared::plugin::yPluginApi::IExtraCommand> command) const;

      //--------------------------------------------------------------
      /// \brief                 Post a manually device creation request to a plugin
      /// \param [in] id         Plugin instance Id
      /// \param [in] request    Request data
      //--------------------------------------------------------------
      void postManuallyDeviceCreationRequest(int id, boost::shared_ptr<shared::plugin::yPluginApi::IManuallyDeviceCreationRequest>& request) const;

      //--------------------------------------------------------------
      /// \brief                 Post a binding query request to a plugin
      /// \param [in] id         Plugin instance Id
      /// \param [in] request    Request data
      //--------------------------------------------------------------
      void postBindingQueryRequest(int id, boost::shared_ptr<shared::plugin::yPluginApi::IBindingQueryRequest>& request);

      //--------------------------------------------------------------
      /// \brief                 Start all instances matching the plugin name
      /// \param [in] pluginName The plugin name
      //--------------------------------------------------------------
      void startAllInstancesOfPlugin(const std::string& pluginName);

      //--------------------------------------------------------------
      /// \brief                 Stop all instances matching the plugin name
      /// \param [in] pluginName The plugin name
      //--------------------------------------------------------------
      void stopAllInstancesOfPlugin(const std::string& pluginName);

   private:
      //-----------------------------------------------------
      ///\brief               Start all instances
      ///\param[out] startedInstanceIds Instances started
      //-----------------------------------------------------
      void startAllInstances(std::set<int>& startedInstanceIds);

      //-----------------------------------------------------
      ///\brief               Start all specified instances
      ///\param[in] instances Instances to start
      ///\param[out] startedInstanceIds Instances started
      ///\return              true if all instances were successfully started
      //-----------------------------------------------------
      bool startInstances(const std::vector<boost::shared_ptr<database::entities::CPlugin> >& instances,
                          std::set<int>& startedInstanceIds);

      //-----------------------------------------------------
      ///\brief               Stop all started instances
      //-----------------------------------------------------
      void stopInstances();

      //-----------------------------------------------------
      ///\brief               Stop the instance and wait for stopped
      ///\param[in] id        The instance ID
      ///\throw               CPluginException if timeout
      //-----------------------------------------------------
      void stopInstanceAndWaitForStopped(int id);
      void stopInstanceAndWaitForStoppedThreaded(int id);

      void startInternalPlugin();
      void stopInternalPlugin();

   
      //--------------------------------------------------------------
      /// \brief			The plugin system factory
      //--------------------------------------------------------------
      boost::shared_ptr<IFactory> m_factory;

      //--------------------------------------------------------------
      /// \brief			The available plugin map
      //--------------------------------------------------------------
      IFactory::AvailablePluginMap m_availablePlugins;
      mutable boost::recursive_mutex m_availablePluginsMutex;

      //--------------------------------------------------------------
      /// \brief			Global database accessor
      //--------------------------------------------------------------
      boost::shared_ptr<database::IDataProvider> m_dataProvider;

      //--------------------------------------------------------------
      /// \brief			Shortcut to plugins in database
      //--------------------------------------------------------------
      boost::shared_ptr<database::IPluginRequester> m_pluginDBTable;

      //--------------------------------------------------------------
      /// \brief			Plugin qualifier
      //--------------------------------------------------------------
      const boost::shared_ptr<IQualifier> m_qualifier;

      //--------------------------------------------------------------
      /// \brief			Supervisor event handler
      //--------------------------------------------------------------
      boost::shared_ptr<shared::event::CEventHandler> m_supervisor;

      //--------------------------------------------------------------
      /// \brief			Data access layer
      //--------------------------------------------------------------
      boost::shared_ptr<dataAccessLayer::IDataAccessLayer> m_dataAccessLayer;

      //-----------------------------------------------------
      ///\brief         Instance remover when instance is stopped
      //-----------------------------------------------------
      boost::shared_ptr<CInstanceRemover> m_instanceRemover;

      //--------------------------------------------------------------
      /// \brief			Map of all running instances, and its mutex (key are plugin instance id)
      //--------------------------------------------------------------
      typedef std::map<int, boost::shared_ptr<IInstance> > PluginInstanceMap;
      PluginInstanceMap m_runningInstances;
      mutable boost::recursive_mutex m_runningInstancesMutex;
   };
} // namespace pluginSystem
