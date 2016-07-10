#pragma once
#include "task/ITask.h"
#include "server/database/IDataBackup.h"
#include <shared/event/EventHandler.hpp>
#include <shared/StringExtension.h>

namespace task { namespace backup {

   //------------------------------------------
   ///\brief   Database backup task
   //-----------------------------------------
   class CDatabase : public ITask
   {
   public:
      //------------------------------------------
      ///\brief   Constructor
      //------------------------------------------
      CDatabase(boost::shared_ptr< database::IDataBackup > dataBackupInterface);

      //------------------------------------------
      ///\brief   Destructor
      //------------------------------------------
      virtual ~CDatabase();

   public:
      // ITask implementation
      virtual const std::string & getName();
      virtual void doWork(TaskProgressFunc pFunctor);
      // [END] ITask implementation

   private:
      //------------------------------------------
      ///\brief   Internal progress handler
      //------------------------------------------
      void OnProgressionUpdatedInternal(int remaining, int total, const std::string & message = shared::CStringExtension::EmptyString);

      //------------------------------------------
      ///\brief   The task name
      //------------------------------------------
      static std::string m_taskName;

      //------------------------------------------
      ///\brief   The backup data interface
      //------------------------------------------
      boost::shared_ptr<database::IDataBackup> m_dataBackupInterface;

      //------------------------------------------
      ///\brief   The function pointer for reporting progression
      //------------------------------------------
      TaskProgressFunc m_reportRealProgress;
   };

} //namespace backup
} //namespace task
