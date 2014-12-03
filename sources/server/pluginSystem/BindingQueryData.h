#pragma once
#include <shared/plugin/yadomsApi/IBindingQueryData.h>

namespace pluginSystem
{
   //-----------------------------------------------------
   ///\brief The IBindingQueryData implementation
   //-----------------------------------------------------
   class CBindingQueryData : public shared::plugin::yadomsApi::IBindingQueryData
   {
   public:
      //-----------------------------------------------------
      ///\brief                        Constructor
      ///\param[in] query              The query
      //-----------------------------------------------------
      CBindingQueryData(const std::string& query);

      //-----------------------------------------------------
      ///\brief               Destructor
      //-----------------------------------------------------
      virtual ~CBindingQueryData();

      // IManuallyDeviceCreationData implementation
      virtual const std::string& getQuery() const;
      // [END] IManuallyDeviceCreationData implementation

   private:
      //-----------------------------------------------------
      ///\brief               Query
      //-----------------------------------------------------
      std::string m_query;
   };  
} // namespace pluginSystem	
	
	