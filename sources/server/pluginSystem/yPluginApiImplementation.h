#pragma once
#include <shared/plugin/yPluginApi/IYPluginApi.h>
#include <shared/plugin/information/IInformation.h>
#include <shared/DataContainer.h>
#include "dataAccessLayer/IAcquisitionHistorizer.h"
#include "dataAccessLayer/IDeviceManager.h"
#include "dataAccessLayer/IKeywordManager.h"
#include "database/entities/Entities.h"
#include "database/IDataProvider.h"
#include "IInstanceStateHandler.h"


namespace pluginSystem
{
   //-----------------------------------------------------
   ///\brief The IYPluginApi implementation
   //-----------------------------------------------------
   class CYPluginApiImplementation : public shared::plugin::yPluginApi::IYPluginApi
   {
   public:
      //-----------------------------------------------------
      ///\brief                                    Constructor
      /// \param [in]   pluginInformations         the plugin informations (name, description, version, author...)
      /// \param [in]   instanceData               the instance data
      /// \param [in]   dataPath                   the instance data path
      /// \param [in]   instanceStateHandler       the instance state handler
      /// \param [in]   dataProvider               the database accessor
      /// \param [in]   pluginEventLoggerRequester the plugin event logger requester
      /// \param [in]   deviceManager              the device manager
      /// \param [in]   keywordDataAccessLayer     the database keyword access layer
      /// \param [in]   acquisitionRequester       the acquisition requester
      //-----------------------------------------------------
      CYPluginApiImplementation(boost::shared_ptr<const shared::plugin::information::IInformation> pluginInformations,
                                const boost::shared_ptr<const database::entities::CPlugin> instanceData,
                                const boost::filesystem::path& dataPath,
                                boost::shared_ptr<IInstanceStateHandler> instanceStateHandler,
                                boost::shared_ptr<database::IDataProvider> dataProvider,
                                boost::shared_ptr<dataAccessLayer::IDeviceManager> deviceManager,
                                boost::shared_ptr<dataAccessLayer::IKeywordManager> keywordDataAccessLayer,
                                boost::shared_ptr<dataAccessLayer::IAcquisitionHistorizer> acquisitionHistorizer);
      
      //-----------------------------------------------------
      ///\brief Destructor
      //-----------------------------------------------------
      virtual ~CYPluginApiImplementation();

      // IYPluginApi implementation
      void setPluginState( const shared::plugin::yPluginApi::historization::EPluginState& state,
                           const std::string& customMessageId = shared::CStringExtension::EmptyString,
                           const std::map<std::string, std::string> & customMessageDataParams = std::map<std::string, std::string>()) override;

      bool deviceExists(const std::string& device) const override;
      shared::CDataContainer getDeviceDetails(const std::string& device) const override;
      void declareDevice(const std::string& device,
                         const std::string& model,
                         boost::shared_ptr<const shared::plugin::yPluginApi::historization::IHistorizable> keyword,
                         const shared::CDataContainer& details = shared::CDataContainer::EmptyContainer) override;
      void declareDevice(const std::string& device,
                         const std::string& model,
                         const std::vector<boost::shared_ptr<const shared::plugin::yPluginApi::historization::IHistorizable> >& keywords,
                         const shared::CDataContainer& details = shared::CDataContainer::EmptyContainer) override;
      bool keywordExists(const std::string& device,
                         const std::string& keyword) const override;
      bool keywordExists(const std::string& device,
                         boost::shared_ptr<const shared::plugin::yPluginApi::historization::IHistorizable> keyword) const override;
      void declareKeyword(const std::string& device,
                          boost::shared_ptr<const shared::plugin::yPluginApi::historization::IHistorizable> keyword,
                          const shared::CDataContainer& details = shared::CDataContainer::EmptyContainer) override;
      std::string getRecipientValue(int recipientId,
                                    const std::string& fieldName) const override;
      std::vector<int> findRecipientsFromField(const std::string& fieldName,
                                               const std::string& expectedFieldValue) const override;
      bool recipientFieldExists(const std::string& fieldName) const override;
      void historizeData(const std::string& device,
                         boost::shared_ptr<const shared::plugin::yPluginApi::historization::IHistorizable> data) override;
      void historizeData(const std::string& device,
                         const std::vector<boost::shared_ptr<const shared::plugin::yPluginApi::historization::IHistorizable> >& dataVect) override;
      boost::shared_ptr<const shared::plugin::information::IInformation> getInformation() const override;
      shared::CDataContainer getConfiguration() override;
      const boost::filesystem::path& getDataPath() const override;
      shared::event::CEventHandler& getEventHandler() override;
      bool isDeveloperMode() const override;
      // [END] IYPluginApi implementation 

      //-----------------------------------------------------
      ///\brief Get the plugin instance ID
      //-----------------------------------------------------
      int getPluginId() const;

   protected:      

      void declareKeywords(const std::string& device,
                           const std::vector<boost::shared_ptr<const shared::plugin::yPluginApi::historization::IHistorizable>>& keywords) const;


   private:
      //--------------------------------------------------------------
      /// \brief			Plugin informations
      //--------------------------------------------------------------
      boost::shared_ptr<const shared::plugin::information::IInformation> m_informations;

      //--------------------------------------------------------------
      /// \brief			The instance data path
      //--------------------------------------------------------------
      const boost::filesystem::path m_dataPath;

      //--------------------------------------------------------------
      /// \brief			The database accessor
      //--------------------------------------------------------------
      const boost::shared_ptr<const database::entities::CPlugin> m_instanceData;

      //--------------------------------------------------------------
      /// \brief			The instance state handler
      //--------------------------------------------------------------
      boost::shared_ptr<IInstanceStateHandler> m_instanceStateHandler;

      //--------------------------------------------------------------
      /// \brief			The device requester
      //--------------------------------------------------------------
      boost::shared_ptr<dataAccessLayer::IDeviceManager> m_deviceManager;

      //--------------------------------------------------------------
      /// \brief			The keyword requester
      //--------------------------------------------------------------
      boost::shared_ptr<dataAccessLayer::IKeywordManager> m_keywordDataAccessLayer;

      //--------------------------------------------------------------
      /// \brief			The recipient requester
      //--------------------------------------------------------------
      boost::shared_ptr<database::IRecipientRequester> m_recipientRequester;
      
      //--------------------------------------------------------------
      /// \brief			The Acquisition historizer
      //--------------------------------------------------------------
      boost::shared_ptr<dataAccessLayer::IAcquisitionHistorizer> m_acquisitionHistorizer;
   };
	
} // namespace pluginSystem	
	
	