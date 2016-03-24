#pragma once

#include <shared/plugin/yPluginApi/historization/IHistorizable.h>

//--------------------------------------------------------------
/// \brief	    Interface for keywords
//--------------------------------------------------------------
class IOpenZWaveNodeKeyword
{
public:
   //--------------------------------------------------------------
   /// \brief	    Destructor
   //--------------------------------------------------------------
   virtual ~IOpenZWaveNodeKeyword()
   {
   }
   
   //--------------------------------------------------------------
   /// \brief	    Send a command 
   /// \param [in]   commandData    The command content
   /// \return	      true if command has been sent (not applied, just sent), else false
   //--------------------------------------------------------------
   virtual bool sendCommand(const std::string & commandData) = 0;

   //--------------------------------------------------------------
   /// \brief	      get the last value of a keyword (as string)
   //--------------------------------------------------------------   
   virtual boost::shared_ptr<shared::plugin::yPluginApi::historization::IHistorizable> getLastKeywordValue() = 0;
};

