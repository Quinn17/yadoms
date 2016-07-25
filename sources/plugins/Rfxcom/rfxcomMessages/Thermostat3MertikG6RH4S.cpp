#include "stdafx.h"
#include "Thermostat3MertikG6RH4S.h"
#include "RFXtrxHelpers.h"
#include <shared/exception/InvalidParameter.hpp>

namespace yApi = shared::plugin::yPluginApi;

namespace rfxcomMessages
{
   CThermostat3MertikG6RH4S::CThermostat3MertikG6RH4S()
      : m_statusByte(0),
      m_onOff(boost::make_shared<yApi::historization::CSwitch>("onOff")),
      m_upDown(boost::make_shared<yApi::historization::CUpDownStop>("upDown", yApi::EKeywordAccessMode::kGetSet)),
      m_keywords({ m_onOff, m_upDown })
   {
   }

   CThermostat3MertikG6RH4S::~CThermostat3MertikG6RH4S()
   {
   }

   std::string CThermostat3MertikG6RH4S::getModel() const
   {
      return "Mertik G6R-H4S";
   }

   const std::vector<boost::shared_ptr<const yApi::historization::IHistorizable> >& CThermostat3MertikG6RH4S::keywords() const
   {
      return m_keywords;
   }

   void CThermostat3MertikG6RH4S::set(const std::string& keyword, const std::string& yadomsCommand)
   {
      if (boost::iequals(keyword, m_onOff->getKeyword()))
      {
         m_onOff->setCommand(yadomsCommand);
         m_statusByte = m_onOff.get() ? thermostat3_sOn : thermostat3_sOff;
      }
      else if (boost::iequals(keyword, m_upDown->getKeyword()))
      {
         m_upDown->setCommand(yadomsCommand);
         switch (m_upDown->get())
         {
         case yApi::historization::EUpDownStopCommand::kUpValue: m_statusByte = thermostat3_sUp;
            break;
         case yApi::historization::EUpDownStopCommand::kDownValue: m_statusByte = thermostat3_sDown;
            break;
         case yApi::historization::EUpDownStopCommand::kStopValue: m_statusByte = thermostat3_sStop;
            break;
         default: throw shared::exception::CInvalidParameter("yadomsCommand");
         }
      }
   }

   void CThermostat3MertikG6RH4S::reset()
   {
      m_onOff->set(false);
      m_upDown->set(yApi::historization::EUpDownStopCommand::kStop);
   }

   void CThermostat3MertikG6RH4S::setFromProtocolState(unsigned char cmd)
   {
      m_statusByte = cmd;
      switch (m_statusByte)
      {
      case thermostat3_sOff: m_onOff->set(false);
         break;
      case thermostat3_sOn: m_onOff->set(true);
         break;

      case thermostat3_sUp: m_upDown->set(yApi::historization::EUpDownStopCommand::kUp);
         break;
      case thermostat3_sDown: m_upDown->set(yApi::historization::EUpDownStopCommand::kDown);
         break;
      case thermostat3_sStop: m_upDown->set(yApi::historization::EUpDownStopCommand::kStop);
         break;

      default:
         throw shared::exception::CInvalidParameter("state, " + boost::lexical_cast<std::string>(m_statusByte));
      }
   }

   void CThermostat3MertikG6RH4S::toProtocolState(unsigned char& cmd) const
   {
      cmd = m_statusByte;
   }
} // namespace rfxcomMessages


