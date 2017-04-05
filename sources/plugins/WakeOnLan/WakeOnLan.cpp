#include "stdafx.h"
#include "WakeOnLan.h"
#include <plugin_cpp_api/ImplementationHelper.h>
#include <shared/plugin/yPluginApi/historization/MessageFormatter.h>
#include <shared/Log.h>


IMPLEMENT_PLUGIN(CWakeOnLan)

CWakeOnLan::CWakeOnLan() :
	m_deviceName("WakeOnLan"),
	m_wakeOnLan(boost::make_shared<yApi::historization::CEvent>("WakeOnLan"))
{
	//voir la librairie awake et faire la même chose
}

CWakeOnLan::~CWakeOnLan()
{
}

void CWakeOnLan::done(const boost::system::error_code& error)
{
	//std::cout << __FUNCTION__ << "(" << error << ")" << std::endl;
}

void CWakeOnLan::doWork(boost::shared_ptr<yApi::IYPluginApi> api)
{
	//create device/keyword
	//auto keyword(boost::make_shared<CWolKeyword>(m_keywordName));

	if (!api->deviceExists(m_deviceName))
		api->declareDevice(m_deviceName, "Wake On Lan");

	// Declare these sensors to Yadoms (devices and associated keywords) if not already declared
	if (!api->keywordExists(m_deviceName, m_wakeOnLan))
		api->declareKeyword(m_deviceName, m_wakeOnLan);

	//api->getConfiguration();
	
	//YADOMS_LOG(information) << "Connecting UPS on serial port " << configuration.getSerialPort() << "...";
	// the main loop
	YADOMS_LOG(information)  << "CWakeOnLan is running..." ;
	api->setPluginState(yApi::historization::EPluginState::kRunning);


	// TODO : load the mac address from Yadoms configuration 
	boost::array<unsigned char, 6> address;

	//unsigned char address [6];
	address[0] = 0x00;
	address[1] = 0x24;
	address[2] = 0x21;
	address[3] = 0x1E;
	address[4] = 0x08;
	address[5] = 0xE4;

	while (1)
	{
		// Wait for an event
		switch (api->getEventHandler().waitForEvents())
		{
		case yApi::IYPluginApi::kEventStopRequested:
		{
			YADOMS_LOG(information)  << "Stop requested" ;
			api->setPluginState(yApi::historization::EPluginState::kStopped);
			return;
		}
		case yApi::IYPluginApi::kEventDeviceCommand:
		{
			YADOMS_LOG(information)  << "Received event ..." ;
			// A command was received from Yadoms
			auto command = api->getEventHandler().getEventData<boost::shared_ptr<const yApi::IDeviceCommand>>();
			YADOMS_LOG(information)  << "Command received from Yadoms :" << yApi::IDeviceCommand::toString(command) ;
			sendSocket(buildMagicPacket(address));
			//parse the command data
			//yApi::historization::CMessageFormatter msgInfo(command->getBody());

			break;
		}
		default:
		{
			std::cerr << "Unknown ou unsupported message id " << api->getEventHandler().getEventId() << std::endl;
			break;
		}
		}
	}
}

void CWakeOnLan::sendSocket(boost::array<unsigned char, CWakeOnLan::beginMagicPacketLenght + CWakeOnLan::macAdressLenght * CWakeOnLan::numberMacAddress> magic_packet)
{
	YADOMS_LOG(debug)  << "Sending socket ..." ;
	boost::asio::io_service io;
	boost::asio::ip::udp::socket socket(io);
	boost::asio::ip::udp::socket::endpoint_type endpoint(boost::asio::ip::address_v4::broadcast(), CWakeOnLan::port);
	socket.open(endpoint.protocol());
	socket.set_option(boost::asio::socket_base::broadcast(true));
	socket.send_to(boost::asio::buffer(magic_packet), endpoint);
	YADOMS_LOG(debug) << "... socket sent.";
		

}

boost::array<unsigned char, CWakeOnLan::beginMagicPacketLenght + CWakeOnLan::macAdressLenght * CWakeOnLan::numberMacAddress> CWakeOnLan::buildMagicPacket(boost::array<unsigned char, 6> aAddress)
{
	boost::array<unsigned char, CWakeOnLan::beginMagicPacketLenght + CWakeOnLan::macAdressLenght * CWakeOnLan::numberMacAddress>  magic_packet;
	for (int i = 0; i < CWakeOnLan::beginMagicPacketLenght; i++)
	{
		magic_packet[i] = CWakeOnLan::beginMagicPacket;
	}
	for (int i = 1; i <= CWakeOnLan::numberMacAddress; i++)
	{
		for(int j = 0; j<CWakeOnLan::macAdressLenght;j++)
			magic_packet[i*CWakeOnLan::macAdressLenght+j] = aAddress[j];
	}
	
	return magic_packet;
}

