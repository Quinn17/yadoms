#pragma once
#include "../OpenZWaveSingleHistorizableData.h"

namespace historizers
{
   class CCurrent : public COpenZWaveSingleHistorizableData<double>
   {
   public:
      //-----------------------------------------------------
      ///\brief                     Constructor
      ///\param[in] keywordName     Yadoms keyword name
      ///\param[in] accessMode      The access mode
      ///\param[in] measureType     The measure type (normally kAbsolute)
      //-----------------------------------------------------
      CCurrent(const std::string& name,
               shared::plugin::yPluginApi::EKeywordAccessMode accessMode);

      //-----------------------------------------------------
      ///\brief                     Constructor
      ///\param[in] keywordName     Yadoms keyword name
      ///\param[in] accessMode      The access mode
      ///\param[in] measureType     The measure type (normally kAbsolute)
      //-----------------------------------------------------
      CCurrent(const std::string& name,
               shared::plugin::yPluginApi::EKeywordAccessMode accessMode,
               shared::plugin::yPluginApi::historization::EMeasureType measureType);

      //-----------------------------------------------------
      ///\brief                     Destructor
      //-----------------------------------------------------
      virtual ~CCurrent();

      // COpenZWaveSingleHistorizableData<Poco::Int64> override ------------------------
      void setWithUnits(double value,
                        const std::string& unit) override;
      double getWithUnits(const std::string& unit) const override;
      // [END] - COpenZWaveSingleHistorizableData<double> override ------------------------
   };
} //namespace historizers 


