#pragma once
#include "../OpenZWaveSingleHistorizableData.h"

namespace historizers
{
   class CRainRate : public COpenZWaveSingleHistorizableData<double>
   {
   public:
      //-----------------------------------------------------
      ///\brief                     Constructor
      ///\param[in] keywordName     Yadoms keyword name
      ///\param[in] accessMode      The access mode
      ///\param[in] measureType     The measure type (normally kAbsolute)
      //-----------------------------------------------------
      CRainRate(const std::string& name);

      //-----------------------------------------------------
      ///\brief                     Constructor
      ///\param[in] keywordName     Yadoms keyword name
      ///\param[in] accessMode      The access mode
      ///\param[in] measureType     The measure type (normally kAbsolute)
      //-----------------------------------------------------
      CRainRate(const std::string& name,
                shared::plugin::yPluginApi::historization::EMeasureType measureType);

      //-----------------------------------------------------
      ///\brief                     Destructor
      //-----------------------------------------------------
      virtual ~CRainRate();

      // COpenZWaveSingleHistorizableData<Poco::Int64> override ------------------------
      void setWithUnits(double value,
                        const std::string& unit) override;
      double getWithUnits(const std::string& unit) const override;
      // [END] - COpenZWaveSingleHistorizableData<double> override ------------------------
   };
} //namespace historizers 


