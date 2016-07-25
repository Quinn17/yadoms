#include "stdafx.h"
#include "4kEepromWithPio.h"
#include "Identification.h"

namespace device
{
   C4kEepromWithPio::C4kEepromWithPio(EOneWireFamily family,
                                      const std::string& id,
                                      boost::shared_ptr<ioInterfaces::IMultiSwitch> io)
      :m_identification(boost::make_shared<device::CIdentification>(family, id, "DS28E04-100")),
      m_io(io),
      m_ioA(boost::make_shared<yApi::historization::CSwitch>("io_A")),
      m_ioB(boost::make_shared<yApi::historization::CSwitch>("io_B")),
      m_keywords({ m_ioA, m_ioB })
   {
      BOOST_ASSERT_MSG(m_identification->family() == k4kEepromWithPio, "Invalid family number");
   }

   C4kEepromWithPio::~C4kEepromWithPio()
   {
   }

   void C4kEepromWithPio::read() const
   {
      m_ioA->set(m_io->read(0));
      m_ioB->set(m_io->read(1));
   }

   void C4kEepromWithPio::write(const std::string& keyword, const std::string& command)
   {
      boost::shared_ptr<yApi::historization::CSwitch> kw;
      unsigned char unit;

      if (m_ioA->getKeyword() == keyword)
      {
         kw = m_ioA;
         unit = 0;
      }
      else if (m_ioB->getKeyword() == keyword)
      {
         kw = m_ioB;
         unit = 1;
      }
      else
      {
         std::cerr << "Unknown keyword " << keyword << std::endl;
         return;
      }

      if (kw->getAccessMode() != yApi::EKeywordAccessMode::kGetSetValue)
      {
         std::cerr << "Try to drive the read-only keyword " << keyword << std::endl;
         return;
      }
      kw->setCommand(command);
      m_io->write(unit, kw->get());
   }

} // namespace device
