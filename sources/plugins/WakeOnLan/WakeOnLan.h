#pragma once
#include <plugin_cpp_api/IPlugin.h>

// Shortcut to yPluginApi namespace
namespace yApi = shared::plugin::yPluginApi;

//--------------------------------------------------------------
/// \brief	WakeOnLan plugin. Allow to wake a computer from Yadoms
//--------------------------------------------------------------
class CWakeOnLan : public plugin_cpp_api::IPlugin
{

	

public:
   //--------------------------------------------------------------
   /// \brief	Constructor
   //--------------------------------------------------------------
   CWakeOnLan();

   //--------------------------------------------------------------
   /// \brief	Destructor
   //--------------------------------------------------------------
   virtual ~CWakeOnLan();

   // IPlugin implementation
   void doWork(boost::shared_ptr<yApi::IYPluginApi> api) override;
   // [END] IPlugin implementation

   void done(const boost::system::error_code& error);

private:
   //--------------------------------------------------------------
   /// \param [in]   api            The plugin context
   /// \param [in]   recipientId    The recipient id to send sms to
   /// \param [in]   smsContent     The SMS content
   //--------------------------------------------------------------

   std::string m_deviceName;
   std::string m_keywordName;
   //--------------------------------------------------------------
   /// \brief	Event that wake the pc wich belows to the device mac address
   //--------------------------------------------------------------
   boost::shared_ptr<yApi::historization::CEvent> m_wakeOnLan;

   //--------------------------------------------------------------
   /// \brief	The keywords list to historize in one step for better performances
   //--------------------------------------------------------------
  // const yApi::historization::IHistorizable > m_keywords;
   int static const beginMagicPacketLenght = 6;
   char static const beginMagicPacket = 0xFF;
   int static const macAdressLenght = 6;
   int static const numberMacAddress = 16;
   int static const port = 9;
   void sendSocket(boost::array<unsigned char, beginMagicPacketLenght + macAdressLenght * numberMacAddress> magic_packet);
   boost::array<unsigned char, beginMagicPacketLenght + macAdressLenght * numberMacAddress> CWakeOnLan::buildMagicPacket(boost::array<unsigned char, 6> aAddress);
};