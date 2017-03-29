#pragma once
#include <plugin_cpp_api/IPlugin.h>

// Shortcut to yPluginApi namespace
namespace yApi = shared::plugin::yPluginApi;

//--------------------------------------------------------------
/// \brief	FreeMobile plugin. Allow to send notification through FreeMobileApi
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

   void sendSocket( int port, boost::array<unsigned char, 6 + 6 * 16> magic_packet);
   boost::array<unsigned char, 6 + 6 * 16> CWakeOnLan::buildMagicPacket(boost::array<unsigned char, 6> aAddress);
};