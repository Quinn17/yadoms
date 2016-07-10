#pragma once

#include "sqlite3.h"
#include "database/common/Query.h"
#include "database/IDatabaseRequester.h"

namespace database
{
   namespace sqlite
   {
      class CSQLiteRequester : public IDatabaseRequester
      {
      public:
         explicit CSQLiteRequester(const std::string& dbFile, const std::string& dbBackupFile);
         virtual ~CSQLiteRequester();

         // IDatabaseEngine implementation
         void initialize() override;
         void finalize() override;
         shared::CDataContainer getInformation() override;
         // [END] IDatabaseEngine implementation

         // IDatabaseRequester implementation
         common::CQuery newQuery() override;
         void queryEntities(common::adapters::IResultAdapter* pAdapter, const common::CQuery& querytoExecute) override;
         int queryStatement(const common::CQuery& querytoExecute, bool throwIfFails = true) override;
         int queryCount(const common::CQuery& querytoExecute) override;
         QueryRow querySingleLine(const common::CQuery& querytoExecute) override;
         QueryResults query(const common::CQuery& querytoExecute) override;
         bool checkTableExists(const common::CDatabaseTable& tableName) override;
         bool dropTableIfExists(const common::CDatabaseTable& tableName) override;
         bool createTableIfNotExists(const common::CDatabaseTable& tableName, const std::string& tableScript) override;
         void createIndex(const common::CDatabaseTable& tableName, const std::string& indexScript) override;
         void vacuum() override;
         boost::shared_ptr<ITableCreationScriptProvider> getTableCreationScriptProvider() override;
         bool supportInsertOrUpdateStatement() override;
         // [END] IDatabaseRequester implementation

         // ITransactionalProvider implementation
         bool transactionSupport() override;
         void transactionBegin() override;
         void transactionCommit() override;
         void transactionRollback() override;
         bool transactionIsAlreadyCreated() override;
         // [END] ITransactionalProvider implementation

         // IDataBackup implementation
         bool backupSupported() override;
         void backupData(ProgressFunc reporter) override;
         boost::filesystem::path lastBackupData() override;
         // [END] IDataBackup implementation


      private:
         //--------------------------------------------------------------
         /// \Brief		Inject C functions in sqlite engine
         //--------------------------------------------------------------
         void registerExtendedFunctions() const;

         //--------------------------------------------------------------
         /// \Brief		database handler
         //--------------------------------------------------------------
         sqlite3* m_pDatabaseHandler;

         //--------------------------------------------------------------
         /// \Brief		Database path
         //--------------------------------------------------------------
         const std::string m_dbFile;

         //--------------------------------------------------------------
         /// \Brief		Backup database path
         //--------------------------------------------------------------
         const std::string m_dbBackupFile;

         //--------------------------------------------------------------
         /// \Brief		true if a transaction is already begin
         //--------------------------------------------------------------
         bool m_bOneTransactionActive;

         //--------------------------------------------------------------
         /// \Brief		In case of some errors, (database locked,...) the query may be retried
         //--------------------------------------------------------------
         static int m_maxTries;
      };
   } //namespace sqlite
} //namespace database 


