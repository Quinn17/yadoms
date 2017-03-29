#include "stdafx.h"
#include "WakeOnLan.h"
#include <plugin_cpp_api/ImplementationHelper.h>
#include <shared/plugin/yPluginApi/historization/MessageFormatter.h>


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
	

	// the main loop
	std::cout << "CWakeOnLan is running..." << std::endl;
	api->setPluginState(yApi::historization::EPluginState::kRunning);

	boost::array<unsigned char, 6> address;

	//unsigned char address [6];
	address[0] = 0x00;
	address[1] = 0x24;
	address[2] = 0x21;
	address[3] = 0x1E;
	address[4] = 0x08;
	address[5] = 0xE4;

	// modifer le port
	int port = 9;
	


	// faire l'appel de la foncion dans le while lors d'un event  et monter l'adresse mac (faire une fonction qui récupère l'adresse mac et envoie la bonne data



	while (1)
	{
		// Wait for an event
		switch (api->getEventHandler().waitForEvents())
		{
		case yApi::IYPluginApi::kEventStopRequested:
		{
			std::cout << "Stop requested" << std::endl;
			api->setPluginState(yApi::historization::EPluginState::kStopped);
			return;
		}
		case yApi::IYPluginApi::kEventDeviceCommand:
		{
			std::cout << "Received event ..." << std::endl;
			// A command was received from Yadoms
			auto command = api->getEventHandler().getEventData<boost::shared_ptr<const yApi::IDeviceCommand>>();
			std::cout << "Command received from Yadoms :" << yApi::IDeviceCommand::toString(command) << std::endl;
			sendSocket(port, buildMagicPacket(address));
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

void CWakeOnLan::sendSocket( int port, boost::array<unsigned char, 6 + 6 * 16> magic_packet)
{
	std::cout << "Sending socket ..." << std::endl;
	boost::asio::io_service io;
	boost::asio::ip::udp::socket socket(io);
	boost::asio::ip::udp::socket::endpoint_type endpoint(boost::asio::ip::address_v4::broadcast(), port);
	socket.open(endpoint.protocol());
	socket.set_option(boost::asio::socket_base::broadcast(true));
	socket.send_to(boost::asio::buffer(magic_packet), endpoint);
	
		

}

boost::array<unsigned char, 6 + 6 * 16> CWakeOnLan::buildMagicPacket(boost::array<unsigned char, 6> aAddress)
{
	// modifier la data
	boost::array<unsigned char, 6 + 6 * 16>  magic_packet;
	//The magic packet begin with FF FF FF FF FF FF FF
	for (int i = 0; i < 6; i++)
	{
		magic_packet[i] = 0xFF;
	}

	//Remettre l'ancien code !!!
	for (int i = 1; i <= 16; i++)
	{
		for(int j = 0; j<6;j++)
			magic_packet[i*6+j] = aAddress[j];
	}
	
	return magic_packet;
}

