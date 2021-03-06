#pragma once
#include <shared/plugin/information/IInformation.h>
#include <shared/plugin/yPluginApi/IBindingQueryRequest.h>
#include <shared/plugin/yPluginApi/IDeviceCommand.h>
#include <shared/plugin/yPluginApi/IExtraCommand.h>
#include <shared/plugin/yPluginApi/IManuallyDeviceCreationRequest.h>


namespace pluginSystem
{
   //--------------------------------------------------------------
   /// \brief	yScriptApi IPC adapter interface, used by plugin to dial with Yadoms
   //--------------------------------------------------------------
   class IIpcAdapter
   {
   public:
      //--------------------------------------------------------------
      /// \brief	Destructor
      //--------------------------------------------------------------
      virtual ~IIpcAdapter()
      {
      }

      //--------------------------------------------------------------
      /// \brief	Get the context accessor ID (unique on full system)
      /// \return The context accessor ID
      //--------------------------------------------------------------
      virtual std::string id() const = 0;

      //--------------------------------------------------------------
      /// \brief	Post the plugin stop request
      //--------------------------------------------------------------
      virtual void postStopRequest() = 0;

      //--------------------------------------------------------------
      /// \brief	Post the plugin information
      /// \param[in] information The plugin information
      /// \param[in] dataPath    The plugin instance data path
      //--------------------------------------------------------------
      virtual void postInit(boost::shared_ptr<const shared::plugin::information::IInformation> information,
                            const boost::filesystem::path& dataPath) = 0;

      //--------------------------------------------------------------
      /// \brief                 Post a configuration update to plugin instance
      /// \param [in] newConfiguration The new configuration
      //--------------------------------------------------------------
      virtual void postUpdateConfiguration(const shared::CDataContainer& newConfiguration) = 0;

      //--------------------------------------------------------------
      /// \brief                 Post a custom query request to a plugin
      /// \param [in] request    Request data
      //--------------------------------------------------------------
      virtual void postBindingQueryRequest(boost::shared_ptr<shared::plugin::yPluginApi::IBindingQueryRequest> request) = 0;

      //--------------------------------------------------------------
      /// \brief                 Post a device command to a plugin
      /// \param [in] deviceCommand The command
      //--------------------------------------------------------------
      virtual void postDeviceCommand(boost::shared_ptr<const shared::plugin::yPluginApi::IDeviceCommand> deviceCommand) = 0;

      //--------------------------------------------------------------
      /// \brief                 Post an extra command to a plugin
      /// \param [in] extraCommand The command
      //--------------------------------------------------------------
      virtual void postExtraCommand(boost::shared_ptr<const shared::plugin::yPluginApi::IExtraCommand> extraCommand) = 0;

      //--------------------------------------------------------------
      /// \brief                 Post a manually device creation request to a plugin
      /// \param [in] request    Request data
      //--------------------------------------------------------------
      virtual void postManuallyDeviceCreationRequest(boost::shared_ptr<shared::plugin::yPluginApi::IManuallyDeviceCreationRequest> request) = 0;

   };
} // namespace pluginSystem

