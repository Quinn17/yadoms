#include "stdafx.h"
#include "Thermostat4.h"

// Shortcut to yPluginApi namespace
namespace yApi = shared::plugin::yPluginApi;

namespace rfxcomMessages
{
   CThermostat4::CThermostat4(boost::shared_ptr<yApi::IYPluginApi> api,
                              const std::string& keyword,
                              const std::string& command,
                              const shared::CDataContainer& deviceDetails)
      : m_onOff(boost::make_shared<yApi::historization::CSwitch>("onOff")),
        m_flame(boost::make_shared<yApi::historization::CDimmable>("flame")),
        m_keywords({m_onOff, m_flame})
   {
      if (boost::iequals(keyword, m_onOff->getKeyword()))
      {
         m_onOff->setCommand(command);
         m_flame->set(0);
      }
      else if (boost::iequals(keyword, m_flame->getKeyword()))
      {
         m_onOff->set(true);
         m_flame->set(command);
      }
      else
      {
         std::cerr << "Unsupported keyword \"" << keyword << "\" for CThermostat4 device " << std::endl;
         return;
      }

      m_unitCode = deviceDetails.get<unsigned int>("unitCode");

      Init(api);
   }

   CThermostat4::CThermostat4(boost::shared_ptr<yApi::IYPluginApi> api,
                              unsigned int subType,
                              const shared::CDataContainer& manuallyDeviceCreationConfiguration)
      : m_onOff(boost::make_shared<yApi::historization::CSwitch>("onOff")),
        m_flame(boost::make_shared<yApi::historization::CDimmable>("flame")),
        m_keywords({m_onOff, m_flame})
   {
      m_onOff->set(false);
      m_flame->set(0);

      m_subType = static_cast<unsigned char>(subType);
      if (m_subType != sTypeMCZ1 && m_subType != sTypeMCZ2)
         throw shared::exception::COutOfRange("Manually device creation : subType is not supported");

      m_unitCode = manuallyDeviceCreationConfiguration.get<unsigned int>("unitCode");

      Init(api);
   }

   CThermostat4::CThermostat4(boost::shared_ptr<yApi::IYPluginApi> api,
                              const RBUF& rbuf,
                              size_t rbufSize)
      : m_onOff(boost::make_shared<yApi::historization::CSwitch>("onOff")),
        m_flame(boost::make_shared<yApi::historization::CDimmable>("flame")),
        m_keywords({m_onOff, m_flame})
   {
      m_subType = 0;
      m_unitCode = 0;

      // Should not be called (transmitter-only device)
      BOOST_ASSERT_MSG(false, "Constructing CThermostat4 object from received buffer is not possible, CThermostat4 is transmitter-only device");
   }

   CThermostat4::~CThermostat4()
   {
   }

   void CThermostat4::Init(boost::shared_ptr<yApi::IYPluginApi> api)
   {
      // Build device description
      buildDeviceModel();
      buildDeviceName();

      // Create device and keywords if needed
      if (!api->deviceExists(m_deviceName))
      {
         shared::CDataContainer details;
         details.set("type", pTypeThermostat4);
         details.set("subType", m_subType);
         details.set("unitCode", m_unitCode);
         api->declareDevice(m_deviceName, m_deviceModel, m_keywords, details);
      }
   }

   boost::shared_ptr<std::queue<shared::communication::CByteBuffer>> CThermostat4::encode(boost::shared_ptr<ISequenceNumberProvider> seqNumberProvider) const
   {
      RBUF rbuf;
      MEMCLEAR(rbuf.THERMOSTAT4);

      rbuf.THERMOSTAT4.packetlength = ENCODE_PACKET_LENGTH(LIGHTING3);
      rbuf.THERMOSTAT4.packettype = pTypeThermostat4;
      rbuf.THERMOSTAT4.subtype = m_subType;
      rbuf.THERMOSTAT4.seqnbr = seqNumberProvider->next();
      rbuf.THERMOSTAT4.unitcode1 = static_cast<unsigned char>(0xFF & (m_unitCode >> 16));
      rbuf.THERMOSTAT4.unitcode2 = static_cast<unsigned char>(0xFF & (m_unitCode >> 8));
      rbuf.THERMOSTAT4.unitcode3 = static_cast<unsigned char>(0xFF & m_unitCode);
      rbuf.THERMOSTAT4.beep = 0;
      rbuf.THERMOSTAT4.fan1_speed = 6; // Auto
      rbuf.THERMOSTAT4.fan2_speed = 6; // Auto
      rbuf.THERMOSTAT4.flame_power = (m_flame->switchLevel() == 100) ? 5 : floor(m_flame->switchLevel() / 20) + 1;
      rbuf.THERMOSTAT4.mode = m_onOff->get() ? thermostat4_sManual : thermostat4_sOff;
      rbuf.THERMOSTAT4.rssi = 0;
      rbuf.THERMOSTAT4.filler = 0;

      return toBufferQueue(rbuf, GET_RBUF_STRUCT_SIZE(LIGHTING3));
   }

   void CThermostat4::historizeData(boost::shared_ptr<yApi::IYPluginApi> api) const
   {
      api->historizeData(m_deviceName, m_keywords);
   }

   const std::string& CThermostat4::getDeviceName() const
   {
      return m_deviceName;
   }

   void CThermostat4::buildDeviceName()
   {
      std::ostringstream ssdeviceName;
      ssdeviceName << m_deviceModel << "." << m_unitCode;
      m_deviceName = ssdeviceName.str();
   }

   void CThermostat4::buildDeviceModel()
   {
      std::ostringstream ssModel;

      switch (m_subType)
      {
      case sTypeMCZ1: ssModel << "MCZ Pellet Stove (1 fan model)";
      case sTypeMCZ2: ssModel << "MCZ Pellet Stove (2 fans model)";
         break;
      default: ssModel << std::to_string(m_subType);
         break;
      }

      m_deviceModel = ssModel.str();
   }
} // namespace rfxcomMessages
