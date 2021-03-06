//
// Main object factory
//
#include "stdafx.h"
#include "ObjectFactory.h"
#include <shared/ServiceLocator.h>
#include "automation/script/IpApiAutoLocation.h"
#include "automation/script/DayLight.h"
#include "automation/script/Location.h"
#include "dataAccessLayer/IDataAccessLayer.h"

namespace automation
{
   namespace script
   {
      ObjectFactory::~ObjectFactory()
      {
      }

      boost::shared_ptr<IAutoLocation> ObjectFactory::getAutoLocationService()
      {
         if (!m_autoLocationService)
         {
            // Use ip-api service to try to auto-locate (need an internet connexion)
            m_autoLocationService = boost::make_shared<CIpApiAutoLocation>();
         }
         return m_autoLocationService;
      }

      boost::shared_ptr<ILocation> ObjectFactory::getLocation()
      {
         if (!m_location)
            m_location = boost::make_shared<CLocation>(shared::CServiceLocator::instance().get<dataAccessLayer::IDataAccessLayer>()->getConfigurationManager());
         return m_location;
      }

      boost::shared_ptr<IDayLight> ObjectFactory::getDayLight()
      {
         if (!m_dayLight)
            m_dayLight = boost::make_shared<CDayLight>(getLocation());
         return m_dayLight;
      }
   }
} // namespace automation::script


