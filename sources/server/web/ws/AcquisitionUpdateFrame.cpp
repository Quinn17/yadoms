#include "stdafx.h"
#include "AcquisitionUpdateFrame.h"

namespace web { namespace ws {

   CAcquisitionUpdateFrame::CAcquisitionUpdateFrame(const database::entities::CAcquisition & content)
      :CFrameBase(CFrameBase::EFrameType::kAcquisitionUpdate)
   {
      m_internalContainer.set(CFrameBase::m_dataFieldName, content);
   }
   

   CAcquisitionUpdateFrame::CAcquisitionUpdateFrame(const boost::shared_ptr<database::entities::CAcquisition> & content)
      :CFrameBase(CFrameBase::EFrameType::kAcquisitionUpdate)
   {
      m_internalContainer.set(CFrameBase::m_dataFieldName, *content.get());
   }
   
   CAcquisitionUpdateFrame::~CAcquisitionUpdateFrame()
   {

   }

} //namespace ws
} //namespace web