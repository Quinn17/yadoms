#pragma once

#include "IKeyword.h"
#include "../specificHistorizer/Period.h"

namespace yApi = shared::plugin::yPluginApi;

//--------------------------------------------------------------
/// \brief	The Load class
//--------------------------------------------------------------
class CRunningPeriod : public IKeyword
{
public:
   //--------------------------------------------------------------
   /// \brief	                      Constructor
   /// \param[in] api                  Yadoms APi context
   /// \param[in] KeyWordName          The keyword name
   /// \note                           Use this constructor initialising the keyword for this plugin
   //--------------------------------------------------------------
   CRunningPeriod(std::string KeyWordName);

   //--------------------------------------------------------------
   /// \brief	                      Set the value of the keyword
   /// \param[in] Value                The new value
   //--------------------------------------------------------------
   void set(std::string& Value) const;

   //--------------------------------------------------------------
   /// \brief	Destructor
   //--------------------------------------------------------------
   virtual ~CRunningPeriod();

   // IKeyword implementation
   boost::shared_ptr<yApi::historization::IHistorizable> GetHistorizable() const override;
   // [END] IKeyword implementation

private:
   //--------------------------------------------------------------
   /// \brief	The device name
   //--------------------------------------------------------------
   std::string m_PluginName;

   //--------------------------------------------------------------
   /// \brief	The period type
   //--------------------------------------------------------------
   boost::shared_ptr<teleInfo::specificHistorizers::CPeriod> m_runningPeriod;

   //--------------------------------------------------------------
   /// \brief	The map of returned value to the IHM
   //--------------------------------------------------------------
   typedef std::map<std::string, unsigned int> EnumPeriod;
};

