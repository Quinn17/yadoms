#include "stdafx.h"
#include "Security1KD101_SA30.h"

namespace yApi = shared::plugin::yPluginApi;

namespace rfxcomMessages
{
   CSecurity1KD101_SA30::CSecurity1KD101_SA30(SubType subtype,
                                              const std::string& model)
      : m_subtype(subtype),
        m_model(model),
        m_alarm(boost::make_shared<yApi::historization::CSwitch>("alarm", yApi::EKeywordAccessMode::kGet)),
        m_keywords({m_alarm})
   {
   }

   CSecurity1KD101_SA30::~CSecurity1KD101_SA30()
   {
   }

   std::string CSecurity1KD101_SA30::getModel() const
   {
      return m_model;
   }

   const std::vector<boost::shared_ptr<const yApi::historization::IHistorizable>>& CSecurity1KD101_SA30::keywords() const
   {
      return m_keywords;
   }

   void CSecurity1KD101_SA30::set(const std::string& /*keyword*/, const std::string& /*yadomsCommand*/)
   {
      // Get-only keywords
   }

   void CSecurity1KD101_SA30::reset()
   {
      m_alarm->set(false);
   }

   void CSecurity1KD101_SA30::setFromProtocolState(unsigned char statusByte)
   {
      m_alarm->set(statusByte == sStatusPanic);
   }

   unsigned char CSecurity1KD101_SA30::toProtocolState() const
   {
      // Get-only keywords
      return 0;
   }

   unsigned long CSecurity1KD101_SA30::idFromProtocol(const RBUF& rbuf) const
   {
      if (m_subtype == rfxValueRM174RF)
         return static_cast<unsigned long>(rbuf.SECURITY1.id1 << 16) | (rbuf.SECURITY1.id2 << 8) | (rbuf.SECURITY1.id3);
      else
         return static_cast<unsigned long>(rbuf.SECURITY1.id1 << 8) | (rbuf.SECURITY1.id2);
   }
} // namespace rfxcomMessages
