#include "stdafx.h"
#include "8ChannelAddressableSwitch.h"
#include "Identification.h"

namespace device
{
   C8ChannelAddressableSwitch::C8ChannelAddressableSwitch(EOneWireFamily family,
                                                          const std::string& id,
                                                          boost::shared_ptr<ioInterfaces::IMultiSwitch> io)
      :m_identification(boost::make_shared<device::CIdentification>(family, id, "DS2408")),
      m_io(io)
   {
      BOOST_ASSERT_MSG(m_identification->family() == k8ChannelAddressableSwitch, "Invalid family number");

      for (unsigned char i = 0; i < NbIo; ++i)
      {
         std::string keywordName("io_");
         keywordName.append(1, '0' + i);

         m_keyword[i] = boost::make_shared<yApi::historization::CSwitch>(keywordName);
         m_keywords.push_back(m_keyword[i]);
      }
   }

   C8ChannelAddressableSwitch::~C8ChannelAddressableSwitch()
   {
   }

   void C8ChannelAddressableSwitch::read() const
   {
      for (unsigned char i = 0; i < NbIo; ++i)
         m_keyword[i]->set(m_io->read(i));
   }

   void C8ChannelAddressableSwitch::write(const std::string& keyword, const std::string& command)
   {
      for (unsigned char i = 0; i < NbIo; ++i)
      {
         if (m_keyword[i]->getKeyword() == keyword)
         {
            if (m_keyword[i]->getAccessMode() != yApi::EKeywordAccessMode::kGetSetValue)
            {
               std::cerr << "Try to drive the read-only keyword " << keyword << std::endl;
               return;
            }
            m_keyword[i]->setCommand(command);
            m_io->write(i, m_keyword[i]->get());
         }
      }
   }
} // namespace device
