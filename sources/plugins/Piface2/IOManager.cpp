#include "stdafx.h"
#include "IOManager.h"
#include "InitializationException.hpp"
#include "pifacedigital.h"
#include <mcp23s17.h>
#include "eventDefinitions.h"

CIOManager::CIOManager(const std::string& device)
   : m_InterruptEventHandler(nullptr),
     m_deviceName (device),
     m_inputValue(0)
{
   // Open the connection
   if (pifacedigital_open(0) == -1)
      throw CInitializationException("Initialization error - Configuration of the SPI in raspi-config ?");

   //TODO : clean-up when another Piface2
   if (pifacedigital_enable_interrupts()<0)
   //  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
   
   // if the first initialization doesn't work
   //int ret = pifacedigital_enable_interrupts();

   //if (ret != 0)
      throw CInitializationException("interrupt initialization error");
}

void CIOManager::Initialize(boost::shared_ptr<yApi::IYPluginApi> api, 
                            std::map<std::string, boost::shared_ptr<CIO> > IOlist)
{
   m_InterruptEventHandler = &api->getEventHandler();
   m_mapKeywordsName = IOlist;
   m_keywordsToDeclare.clear();

   // Initial reading of DI
   for (int counter = 0; counter<NB_INPUTS; ++counter)
   {
      std::string name = "DI" + boost::lexical_cast<std::string>(counter);
      m_inputValue |= (m_mapKeywordsName[name]->get() << counter);
      m_keywordsToDeclare.push_back(m_mapKeywordsName[name]->historizable());
   }

   //Historize initial values
   api->historizeData(m_deviceName, m_keywordsToDeclare);

   // Creation of the reception thread
   m_interruptReceiverThread = boost::thread(&CIOManager::interruptReceiverThreaded, this, m_deviceName);
}

void CIOManager::onCommand(boost::shared_ptr<yApi::IYPluginApi> api,
                           boost::shared_ptr<const yApi::IDeviceCommand> command)
{
   std::cout << "Command received :" << yApi::IDeviceCommand::toString(command) << std::endl;

   auto search = m_mapKeywordsName.find(command->getKeyword());

   if (search != m_mapKeywordsName.end())
   {
      // Set the value of the corresponding IO and historize 
      search->second->set(boost::lexical_cast<bool>(command->getBody()));
      api->historizeData(m_deviceName, search->second->historizable());
   }
   else
      std::cerr << "Cannot find keyword : " << command->getKeyword();
}

void CIOManager::onCommand(boost::shared_ptr<yApi::IYPluginApi> api,
                           int receivedValue)
{
   m_keywordsToDeclare.clear();

   std::cout << "Value received :" << std::hex << receivedValue << std::endl;

   for (int counter = 0; counter<NB_INPUTS; ++counter)
   {
      std::string name = "DI" + boost::lexical_cast<std::string>(counter);

      if ((m_inputValue & (1 << counter)) != (receivedValue & (1 << counter)))
      {
         m_mapKeywordsName[name]->set(boost::lexical_cast<bool>((receivedValue & (1 << counter)) >> counter));
         m_keywordsToDeclare.push_back(m_mapKeywordsName[name]->historizable());
      }
   }

   m_inputValue = receivedValue;
   api->historizeData(m_deviceName, m_keywordsToDeclare);
}

void CIOManager::interruptReceiverThreaded(const std::string& keywordName) const
{
   try
   {
      std::cout << "### Start interruptReceiverThread ###" << std::endl;

      while (true)
      {
         unsigned char inputs=0;

         int ret = pifacedigital_wait_for_input(&inputs, 5000, 0);
         switch (ret)
         {
            // Change of variables
            case 1:
               m_InterruptEventHandler->postEvent<const int>(kEvtIOStateReceived, inputs );
               break;
            // Time out
            case 0:
               break;
            // Error
            case -1:
               throw CInitializationException("pifacedigital_wait_for_input error");
               break;
            // Unknow return
            default:
               throw CInitializationException("pifacedigital_wait_for_input unknow return");
               break;
         }

         if (boost::this_thread::interruption_requested())
            throw boost::thread_interrupted();
      }
   }
   catch (boost::thread_interrupted&)
   {
      std::cout << "### Stop interruptReceiverThread ###" << std::endl;
   }
   catch (...)
   {
      std::cout << "### Unknown error ###" << std::endl;
   }
}

CIOManager::~CIOManager()
{
   m_interruptReceiverThread.interrupt();

   if (!m_interruptReceiverThread.timed_join(boost::posix_time::seconds(20)))
      std::cerr << "Thread interruptReceiverThread join time out" << std::endl;

   // Close de connection
   pifacedigital_close(0);
}