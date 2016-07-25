#include "stdafx.h"
#include "CurrentEnergyFilter.h"
#include <shared/currentTime/Provider.h>


static const int NbOccurencesToBeValid = 3;

bool CCurrentEnergyFilter::isValid(const std::string& deviceName)
{
   // Add the new device
   m_recentlySeenDevices.push_back(std::pair<boost::posix_time::ptime, std::string>(shared::currentTime::Provider().now(), deviceName));

   // Remove too old elements
   auto tooOldThreshold = shared::currentTime::Provider().now() - boost::posix_time::hours(12);
   {
      m_recentlySeenDevices.erase(std::remove_if(m_recentlySeenDevices.begin(),
                                                 m_recentlySeenDevices.end(),
                                                 [&](std::pair<boost::posix_time::ptime, std::string> value)
      {
         return value.first < tooOldThreshold;
      }),
                                  m_recentlySeenDevices.end());
   }

   // Count occurences of this device
   auto nbOccurences = 0;
   auto isValid = false;
   std::for_each(m_recentlySeenDevices.begin(),
                 m_recentlySeenDevices.end(),
                 [&](std::pair<boost::posix_time::ptime, std::string> value)
   {
      if (value.second == deviceName)
      {
         ++nbOccurences;
         if (nbOccurences >= NbOccurencesToBeValid)
            isValid = true;
      }
   });

   // If found, remove elements corresponding to deviceName before return
   if (isValid)
   {
      m_recentlySeenDevices.erase(std::remove_if(m_recentlySeenDevices.begin(),
                                                 m_recentlySeenDevices.end(),
                                                 [&](std::pair<boost::posix_time::ptime, std::string> value)
      {
         return value.second == deviceName;
      }),
                                  m_recentlySeenDevices.end());
   }

   return isValid;
}

