#include "stdafx.h"
#include "DateTime.h"
#include <shared/exception/InvalidParameter.hpp>

// Shortcut to yPluginApi namespace
namespace yApi = shared::plugin::yPluginApi;

namespace rfxcomMessages
{
   CDateTime::CDateTime(boost::shared_ptr<yApi::IYPluginApi> api,
                        const RBUF& rbuf,
                        size_t rbufSize)
      : m_dateTime(boost::make_shared<yApi::historization::CDateTime>("datetime")),
      m_batteryLevel(boost::make_shared<yApi::historization::CBatteryLevel>("battery")),
      m_rssi(boost::make_shared<yApi::historization::CRssi>("rssi")),
      m_keywords({ m_dateTime, m_batteryLevel , m_rssi })
   {
      CheckReceivedMessage(rbuf,
                           rbufSize,
                           pTypeDT,
                           DONT_CHECK_SUBTYPE,
                           GET_RBUF_STRUCT_SIZE(DT),
                           DONT_CHECK_SEQUENCE_NUMBER);

      m_subType = rbuf.DT.subtype;

      m_id = rbuf.DT.id1 | (rbuf.DT.id2 << 8);

      m_dateTime->set(boost::posix_time::ptime(boost::gregorian::date(2000 + rbuf.DT.yy, rbuf.DT.mm, rbuf.DT.dd),
                                               boost::posix_time::time_duration(rbuf.DT.hr, rbuf.DT.min, rbuf.DT.sec)));

      m_batteryLevel->set(NormalizeBatteryLevel(rbuf.DT.battery_level));
      m_rssi->set(NormalizeRssiLevel(rbuf.DT.rssi));

      Init(api);
   }

   CDateTime::~CDateTime()
   {
   }

   void CDateTime::Init(boost::shared_ptr<yApi::IYPluginApi> api)
   {
      // Build device description
      buildDeviceModel();
      buildDeviceName();

      // Create device and keywords if needed
      if (!api->deviceExists(m_deviceName))
      {
         shared::CDataContainer details;
         details.set("type", pTypeDT);
         details.set("subType", m_subType);
         details.set("id", m_id);
         api->declareDevice(m_deviceName,
                            m_deviceModel,
                            m_keywords,
                            details);
      }
   }

   boost::shared_ptr<std::queue<shared::communication::CByteBuffer> > CDateTime::encode(boost::shared_ptr<ISequenceNumberProvider> seqNumberProvider) const
   {
      throw shared::exception::CInvalidParameter("Temp is a read-only message, can not be encoded");
   }

   void CDateTime::historizeData(boost::shared_ptr<yApi::IYPluginApi> api) const
   {
      api->historizeData(m_deviceName, m_keywords);
   }

   const std::string& CDateTime::getDeviceName() const
   {
      return m_deviceName;
   }

   void CDateTime::buildDeviceName()
   {
      std::ostringstream ssdeviceName;
      ssdeviceName << static_cast<unsigned int>(m_subType) << "." << static_cast<unsigned int>(m_id);
      m_deviceName = ssdeviceName.str();
   }

   void CDateTime::buildDeviceModel()
   {
      std::ostringstream ssModel;

      switch (m_subType)
      {
      case sTypeDT1: ssModel << "RTGR328N";
         break;
      default: ssModel << boost::lexical_cast<std::string>(m_subType);
         break;
      }

      m_deviceModel = ssModel.str();
   }
} // namespace rfxcomMessages


