#pragma once

#include "IKeyword.h"
#include "../specificHistorizer/Color.h"

namespace yApi = shared::plugin::yPluginApi;

//--------------------------------------------------------------
/// \brief	The Load class
//--------------------------------------------------------------
class CForecastTomorrow : public IKeyword
{
public:
   //--------------------------------------------------------------
   /// \brief	                      Constructor
   /// \param[in] KeyWordName          The keyword name
   /// \note                           Use this constructor initialising the keyword for this plugin
   //--------------------------------------------------------------
   CForecastTomorrow(std::string KeyWordName);

   //--------------------------------------------------------------
   /// \brief	                      Set the value of the keyword
   /// \param[in] Value                The new value
   //--------------------------------------------------------------
   void set(std::string& Value) const;

   //--------------------------------------------------------------
   /// \brief	Destructor
   //--------------------------------------------------------------
   virtual ~CForecastTomorrow();

   // IKeyword implementation
   boost::shared_ptr<yApi::historization::IHistorizable> GetHistorizable() const override;
   // [END] IKeyword implementation

private:

   //--------------------------------------------------------------
   /// \brief	The period type
   //--------------------------------------------------------------
   boost::shared_ptr<teleInfo::specificHistorizers::CColor> m_forecastPeriod;

   //--------------------------------------------------------------
   /// \brief	The map of returned value to the IHM
   //--------------------------------------------------------------
   typedef std::map<std::string, unsigned int> EnumColorMap;
};

