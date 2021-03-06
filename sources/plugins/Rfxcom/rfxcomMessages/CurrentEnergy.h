#pragma once

#include "IRfxcomMessage.h"
#include "RFXtrxHelpers.h"
#include <shared/plugin/yPluginApi/IYPluginApi.h>
#include "CurrentEnergyFilter.h"

namespace yApi = shared::plugin::yPluginApi;

namespace rfxcomMessages
{
   //--------------------------------------------------------------
   /// \brief	The current/energy protocol support (reception only)
   //--------------------------------------------------------------
   class CCurrentEnergy : public IRfxcomMessage
   {
   public:
      //--------------------------------------------------------------
      /// \brief	                        Constructor
      /// \param[in] api                  Yadoms APi context
      /// \param[in] rbuf                 The received buffer
      /// \param[in] rbufSize             Message size, received from Rfxcom
      /// \param[in] unsecuredProtocolFilter The filter for unsecured protocols
      /// \note                           Use this constructor for received messages (to historize received data to Yadoms)
      /// \throw                          shared::exception::CInvalidParameter
      //--------------------------------------------------------------
      CCurrentEnergy(boost::shared_ptr<yApi::IYPluginApi> api,
                     const RBUF& rbuf,
                     size_t rbufSize,
                     boost::shared_ptr<IUnsecuredProtocolFilter> unsecuredProtocolFilter);

      //--------------------------------------------------------------
      /// \brief	Destructor
      //--------------------------------------------------------------
      virtual ~CCurrentEnergy();

      //--------------------------------------------------------------
      /// \brief	Filter
      //--------------------------------------------------------------
      static boost::shared_ptr<IUnsecuredProtocolFilter> createFilter();

      // IRfxcomMessage implementation
      boost::shared_ptr<std::queue<shared::communication::CByteBuffer> > encode(boost::shared_ptr<ISequenceNumberProvider> seqNumberProvider) const override;
      void historizeData(boost::shared_ptr<yApi::IYPluginApi> api) const override;
      const std::string& getDeviceName() const override;
      // [END] IRfxcomMessage implementation

   protected:
      //--------------------------------------------------------------
      /// \brief	Global initialization method
      /// \param[in] api                  Yadoms APi context
      //--------------------------------------------------------------
      void Init(boost::shared_ptr<yApi::IYPluginApi> api);

      //--------------------------------------------------------------
      /// \brief	                        Build the device name
      //--------------------------------------------------------------
      void buildDeviceName();

      //--------------------------------------------------------------
      /// \brief	                        Build the sensor model
      //--------------------------------------------------------------
      void buildDeviceModel();

   private:
      //--------------------------------------------------------------
      /// \brief	The device sub-type
      //--------------------------------------------------------------
      unsigned char m_subType;

      //--------------------------------------------------------------
      /// \brief	The device id
      //--------------------------------------------------------------
      unsigned short m_id;

      //--------------------------------------------------------------
      /// \brief	The filter for unsecured protocols
      //--------------------------------------------------------------
      boost::shared_ptr<IUnsecuredProtocolFilter> m_messageFilter;

      //--------------------------------------------------------------
      /// \brief	The device name
      //--------------------------------------------------------------
      std::string m_deviceName;

      //--------------------------------------------------------------
      /// \brief	The device model
      //--------------------------------------------------------------
      std::string m_deviceModel;

      //--------------------------------------------------------------
      /// \brief	The current (A), for the 3 channels
      //--------------------------------------------------------------
      boost::shared_ptr<yApi::historization::CCurrent> m_current1;
      boost::shared_ptr<yApi::historization::CCurrent> m_current2;
      boost::shared_ptr<yApi::historization::CCurrent> m_current3;

      //--------------------------------------------------------------
      /// \brief	The instant power (W)
      //--------------------------------------------------------------
      boost::shared_ptr<yApi::historization::CPower> m_instantPower;

      //--------------------------------------------------------------
      /// \brief	The total power (Wh)
      //--------------------------------------------------------------
      boost::shared_ptr<yApi::historization::CEnergy> m_totalPower;

      //--------------------------------------------------------------
      /// \brief	The battery level (percent)
      //--------------------------------------------------------------
      boost::shared_ptr<yApi::historization::CBatteryLevel> m_batteryLevel;

      //--------------------------------------------------------------
      /// \brief	The RSSI (percent)
      //--------------------------------------------------------------
      boost::shared_ptr<yApi::historization::CRssi> m_rssi;

      //--------------------------------------------------------------
      /// \brief	The keywords list to historize in one step for better performances
      //--------------------------------------------------------------
      std::vector<boost::shared_ptr<const yApi::historization::IHistorizable> > m_keywords;
   };
} // namespace rfxcomMessages


