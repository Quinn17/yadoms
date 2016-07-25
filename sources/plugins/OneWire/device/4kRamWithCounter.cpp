#include "stdafx.h"
#include "4kRamWithCounter.h"
#include "Identification.h"

namespace device
{
   C4kRamWithCounter::C4kRamWithCounter(EOneWireFamily family,
                                        const std::string& id,
                                        boost::shared_ptr<ioInterfaces::IMultiCounter> io)
      :m_identification(boost::make_shared<device::CIdentification>(family, id, "DS2423")),
      m_io(io),
      m_countA(boost::make_shared<yApi::historization::CCounter>("counter_A", yApi::EKeywordAccessMode::kGet, yApi::historization::EMeasureType::kCumulative)),
      m_countB(boost::make_shared<yApi::historization::CCounter>("counter_B", yApi::EKeywordAccessMode::kGet, yApi::historization::EMeasureType::kCumulative)),
      m_keywords({ m_countA, m_countB })
   {
      BOOST_ASSERT_MSG(m_identification->family() == k4kRamWithCounter, "Invalid family number");
   }

   C4kRamWithCounter::~C4kRamWithCounter()
   {
   }

   void C4kRamWithCounter::read() const
   {
      m_countA->set(m_io->read(0));
      m_countB->set(m_io->read(1));
   }

   void C4kRamWithCounter::write(const std::string& keyword, const std::string& command)
   {
      std::cerr << "Try to drive the read-only keyword " << keyword << std::endl;
   }
} // namespace device
