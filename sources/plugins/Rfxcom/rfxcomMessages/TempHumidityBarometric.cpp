#include "stdafx.h"
#include "TempHumidityBarometric.h"
#include <shared/plugin/yadomsApi/StandardCapacities.h>
#include <shared/exception/InvalidParameter.hpp>

// Shortcut to yadomsApi namespace
namespace yApi = shared::plugin::yadomsApi;

namespace rfxcomMessages
{

CTempHumidityBarometric::CTempHumidityBarometric(boost::shared_ptr<yApi::IYadomsApi> context, const RBUF& rbuf, boost::shared_ptr<const ISequenceNumberProvider> seqNumberProvider)
   :m_temperature("temperature"), m_humidity("humidity"), m_pressure("pressure"), m_batteryLevel("battery"), m_rssi("rssi")
{
   CheckReceivedMessage(rbuf, pTypeTEMP_HUM_BARO, GET_RBUF_STRUCT_SIZE(TEMP_HUM_BARO), DONT_CHECK_SEQUENCE_NUMBER);

   m_subType = rbuf.TEMP_HUM_BARO.subtype;

   m_id = rbuf.TEMP_HUM_BARO.id1 | (rbuf.TEMP_HUM_BARO.id2 << 8);

   double temperature = (double)((rbuf.TEMP_HUM_BARO.temperatureh << 8) | rbuf.TEMP_HUM_BARO.temperaturel) / 10;
   if (rbuf.TEMP_HUM_BARO.tempsign)
      temperature = -temperature;
   m_temperature.set(temperature);

   m_humidity.set(rbuf.TEMP_HUM_BARO.humidity);

   m_pressure.set(rbuf.TEMP_HUM_BARO.baroh << 8 | (rbuf.TEMP_HUM_BARO.barol));

   m_batteryLevel.set(rbuf.TEMP_HUM_BARO.battery_level == 0x09 ? 100 : 0);

   m_rssi.set(rbuf.TEMP_HUM_BARO.rssi * 100 / 0x0F);

   Init(context);
}

CTempHumidityBarometric::~CTempHumidityBarometric()
{
}

void CTempHumidityBarometric::Init(boost::shared_ptr<yApi::IYadomsApi> context)
{
   // Build device description
   buildDeviceModel();
   buildDeviceName();

   // Create device and keywords if needed
   if (!context->deviceExists(m_deviceName))
   {
      shared::CDataContainer details;
      details.set("type", pTypeTEMP_HUM_BARO);
      details.set("subType", m_subType);
      details.set("id", m_id);
      context->declareDevice(m_deviceName, m_deviceModel, details.serialize());

      context->declareKeyword(m_deviceName, m_temperature);
      context->declareKeyword(m_deviceName, m_humidity);
      context->declareKeyword(m_deviceName, m_pressure);
      context->declareKeyword(m_deviceName, m_batteryLevel);
      context->declareKeyword(m_deviceName, m_rssi);
   }
}

const shared::communication::CByteBuffer CTempHumidityBarometric::encode(boost::shared_ptr<ISequenceNumberProvider> seqNumberProvider) const
{
   // Nothing to do (message type is read-only)
   return shared::communication::CByteBuffer();
}

void CTempHumidityBarometric::historizeData(boost::shared_ptr<yApi::IYadomsApi> context) const
{
   context->historizeData(m_deviceName, m_temperature);
   context->historizeData(m_deviceName, m_humidity);
   context->historizeData(m_deviceName, m_pressure);
   context->historizeData(m_deviceName, m_batteryLevel);
   context->historizeData(m_deviceName, m_rssi);
}

void CTempHumidityBarometric::buildDeviceName()
{
   std::ostringstream ssdeviceName;
   ssdeviceName << (unsigned int)m_subType << "." << (unsigned int)m_id;
   m_deviceName = ssdeviceName.str();
}

void CTempHumidityBarometric::buildDeviceModel()
{
   std::ostringstream ssModel;

   switch(m_subType)
   {
   case sTypeTHB1: ssModel << "BTHR918"; break;
   case sTypeTHB2: ssModel << "BTHR918N, BTHR968"; break;
   default: ssModel << boost::lexical_cast<std::string>(m_subType); break;
   }

   m_deviceModel = ssModel.str();
}

} // namespace rfxcomMessages