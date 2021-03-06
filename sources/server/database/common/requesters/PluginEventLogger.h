#pragma once

#include "server/database/IPluginEventLoggerRequester.h"
#include "server/database/IDatabaseRequester.h"

namespace database { namespace common { namespace requesters {

      //--------------------------------------------------------------
      /// \Brief		   PluginEventLogger requester for SQLite database
      //--------------------------------------------------------------
      class CPluginEventLogger : public IPluginEventLoggerRequester
      {
      public:
         //--------------------------------------------------------------
         /// \Brief		   Constructor
         /// \param [in]	pDatabaseHandler: the database handler
         //--------------------------------------------------------------
         explicit CPluginEventLogger(boost::shared_ptr<IDatabaseRequester> databaseRequester);

         //--------------------------------------------------------------
         /// \Brief		   Destructor
         //--------------------------------------------------------------
         virtual ~CPluginEventLogger();

         // IPluginEventLoggerRequester implementation
         virtual int addEvent(const std::string & pluginName, const std::string & pluginVersion, const shared::versioning::EReleaseType & pluginReleaseType, const entities::EEventType & eventType, const std::string & message = shared::CStringExtension::EmptyString);
         virtual int addEvent(const entities::CPluginEventLogger & pluginLogEntry);
         virtual std::vector<boost::shared_ptr<entities::CPluginEventLogger> > getPluginEvents(const std::string & pluginName, const std::string & pluginVersion, const shared::versioning::EReleaseType & pluginReleaseType);
         virtual std::vector<boost::shared_ptr<entities::CPluginEventLogger> > getPluginEvents(const std::string & pluginName, const std::string & pluginVersion, const shared::versioning::EReleaseType & pluginReleaseType, const boost::posix_time::ptime & fromDate);
         // [END] IPluginEventLoggerRequester implementation

      private:
         //--------------------------------------------------------------
         /// \Brief		   Reference to IDatabaseRequester
         //--------------------------------------------------------------
         boost::shared_ptr<IDatabaseRequester> m_databaseRequester;
      };

} //namespace requesters
} //namespace common
} //namespace database 

