#pragma once

#include "IRunningInformation.h"

//-----------------------------------------------------------------------------
/// \class   Class which handle information about server system
//-----------------------------------------------------------------------------
class CRunningInformation : public IRunningInformation
{
public:
   //-----------------------------------------------------------------------------
   /// \brief		                     Constructor
   //-----------------------------------------------------------------------------
   CRunningInformation();

   //-----------------------------------------------------------------------------
   /// \brief		                     Destructor
   //-----------------------------------------------------------------------------
   virtual ~CRunningInformation();

   // IRunningInformation implementation
   virtual const boost::posix_time::ptime  & getStartupDateTime() const;
   virtual const tools::CVersion  & getSoftwareVersion() const;
   virtual const std::string getOperatingSystemName() const;
   // [END] IRunningInformation implementation
   
private:
   //-----------------------------------------------------------------------------
   /// \brief		                     Yadoms startup date/time
   //-----------------------------------------------------------------------------
   boost::posix_time::ptime   m_startupDateTime;
   
   //-----------------------------------------------------------------------------
   /// \brief		                     Yadoms version
   //-----------------------------------------------------------------------------
   tools::CVersion   m_softwareVersion;
};
