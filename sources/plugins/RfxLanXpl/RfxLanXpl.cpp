#include "stdafx.h"
#include "RfxLanXpl.h"
#include <shared/plugin/ImplementationHelper.h>
#include <shared/Log.h>
#include <shared/plugin/yadomsApi/StandardCapacities.h>
#include <shared/exception/Exception.hpp>
#include <shared/serialization/PTreeToJsonSerializer.h>
#include "xplcore/XplService.h"
#include "xplcore/XplHub.h"
#include "xplcore/XplConstants.h"
#include "xplrules/IReadRule.h"
#include "xplrules/ICommandRule.h"
#include "xplrules/rfxLanXpl/DeviceManager.h"

// Use this macro to define all necessary to make your DLL a Yadoms valid plugin.
// Note that you have to provide some extra files, like package.json, and icon.png
// This macro also defines the static PluginInformations value that can be used by plugin to get information values
IMPLEMENT_PLUGIN(CRfxLanXpl)

std::string CRfxLanXpl::m_xpl_gateway_id = "xplplug";

CRfxLanXpl::CRfxLanXpl()
   :m_deviceManager(new xplrules::rfxLanXpl::CDeviceManager())
{
}

CRfxLanXpl::~CRfxLanXpl()
{
}

// Event IDs
enum
{
   kXplMessageReceived = yApi::IYadomsApi::kPluginFirstEventId,   // Always start from yApi::IYadomsApi::kPluginFirstEventId
};

void CRfxLanXpl::doWork(boost::shared_ptr<yApi::IYadomsApi> context)
{
   boost::shared_ptr<xplcore::CXplHub> hub;
   try
   {
      // Load configuration values (provided by database)
      m_configuration.set(context->getConfiguration());

      //start ioservice
      xplcore::CXplService xplService(m_xpl_gateway_id, "1", NULL);
      xplService.subscribeForAllMessages(&context->getEventHandler(), kXplMessageReceived);

      //manage xpl hub
      if (m_configuration.getStartXplhub())
      {
         hub.reset(new xplcore::CXplHub(m_configuration.getHubLocalIp()));
         hub->start();
      }

      // the main loop
      YADOMS_LOG(debug) << "RfxLanXpl plugin is running...";
      while (1)
      {
         // Wait for an event
         switch (context->getEventHandler().waitForEvents())
         {
         case yApi::IYadomsApi::kEventDeviceCommand:
         {
            // Command was received from Yadoms
            boost::shared_ptr<yApi::IDeviceCommand> command = context->getEventHandler().getEventData<boost::shared_ptr<yApi::IDeviceCommand> >();
            OnSendDeviceCommand(command, context, xplService);
            break;
         }
         case yApi::IYadomsApi::kEventUpdateConfiguration:
         {
            // Configuration was updated
            std::string newConfiguration = context->getEventHandler().getEventData<std::string>();
            YADOMS_LOG(debug) << "configuration was updated...";
            BOOST_ASSERT(!newConfiguration.empty());  // newConfigurationValues shouldn't be empty, or kEventUpdateConfiguration shouldn't be generated

            // Take into account the new configuration
            // - Restart the plugin if necessary,
            // - Update some resources,
            // - etc...
            m_configuration.set(newConfiguration);
            break;
         }

         case kXplMessageReceived:
         {
            // Xpl message was received
            xplcore::CXplMessage xplMessage = context->getEventHandler().getEventData<xplcore::CXplMessage>();
            OnXplMessageReceived(xplMessage, context);
            break;
         }
         default:
         {
            YADOMS_LOG(error) << "Unknown message id";
            break;
         }
         }
      };
   }
   // Plugin must catch this end-of-thread exception to make its cleanup.
   // If no cleanup is necessary, still catch it, or Yadoms will consider
   // as a plugin failure.
   catch (boost::thread_interrupted&)
   {
   }
   catch (shared::exception::CException & ex)
   {
      YADOMS_LOG(fatal) << "The XPL plugin fails. Unknown expcetion : " << ex.what();
   }

   if (hub)
      hub->stop();
}




//----------------------------------------------
///\brief Function handler when receiving XplMessage
///\param [in] The xpl message received
//----------------------------------------------
void CRfxLanXpl::OnXplMessageReceived(xplcore::CXplMessage & xplMessage, boost::shared_ptr<yApi::IYadomsApi> context)
{
   try
   {
      YADOMS_LOG(trace) << "Xpl Message received : " << xplMessage.toString();

      std::string realSource = xplMessage.getSource().toString();
      //if incomming message has been sent from me, use target has real source
      if (xplMessage.getSource().getVendorId() == xplcore::CXplConstants::getYadomsVendorId() &&
         xplMessage.getSource().getDeviceId() == m_xpl_gateway_id)
         realSource = xplMessage.getTarget().toString();

      if (m_deviceManager->isHandled(realSource))
      {

         boost::shared_ptr<xplrules::IRule> rule = m_deviceManager->identifyRule(xplMessage.getMessageSchemaIdentifier().toString(), m_instanceManager);
         if (rule.get() != NULL)
         {
            //retreeive device identifier
            xplrules::CDeviceIdentifier deviceAddress = rule->getDeviceAddressFromMessage(xplMessage);

            if (!context->deviceExists(deviceAddress.getId()))
            {
               boost::property_tree::ptree details;
               details.put("readingProtocol", deviceAddress.getReadingXplProtocol().toString());
               details.put("writingProtocol", deviceAddress.getWritingXplProtocol().toString());
               details.put("source", realSource);
               shared::serialization::CPtreeToJsonSerializer serialiser;
               context->declareDevice(deviceAddress.getId(), deviceAddress.getCommercialName(), serialiser.serialize(details));
            }

            //create message keywords in database
            std::vector< boost::shared_ptr<xplrules::CDeviceKeyword> > allKeywords = rule->identifyKeywords(xplMessage);
            for (std::vector< boost::shared_ptr<xplrules::CDeviceKeyword> >::iterator keyword = allKeywords.begin(); keyword != allKeywords.end(); ++keyword)
            {
               context->declareKeyword(deviceAddress.getId(), (*keyword)->getName(), (*keyword)->getCapacity(), (*keyword)->getAccessMode(), (*keyword)->getType(), (*keyword)->getUnits(), (*keyword)->getDetails());
            }


            //check if the rule handle reading
            boost::shared_ptr<xplrules::IReadRule> readRule = boost::dynamic_pointer_cast<xplrules::IReadRule>(rule);

            if (readRule)
            {

               //create message to insert in database
               xplrules::MessageContent data = readRule->extractMessageData(xplMessage);

               xplrules::MessageContent::iterator i;
               for (i = data.begin(); i != data.end(); ++i)
               {
                  context->historizeData(deviceAddress.getId(), i->first, i->second);
               }
            }


         }
         else
         {
            std::string errorMessage = (boost::format("Unsupported protocol = %1%") % xplMessage.getMessageSchemaIdentifier().toString()).str();
            YADOMS_LOG(error) << errorMessage;
         }
      }
      else
      {
         std::string errorMessage = (boost::format("Unknown xpl source = %1%") % realSource).str();
         YADOMS_LOG(error) << errorMessage;
      }

   }
   catch (std::exception &ex)
   {
      YADOMS_LOG(error) << "xpl plugin fail to treat message : " << ex.what();
   }
}



//----------------------------------------------
///\brief Function handler used to send a command to a device
///\param [in] The command to send
//----------------------------------------------
void CRfxLanXpl::OnSendDeviceCommand(boost::shared_ptr<yApi::IDeviceCommand> command, boost::shared_ptr<yApi::IYadomsApi> context, xplcore::CXplService & xplService)
{
   try
   {
      YADOMS_LOG(trace) << "Sending message : " << command->toString();
      if (context->deviceExists(command->getTargetDevice()))
      {
         //get device details
         std::string deviceDetails = context->getDeviceDetails(command->getTargetDevice());
         boost::property_tree::ptree details;
         shared::serialization::CPtreeToJsonSerializer serialiser;
         serialiser.deserialize(deviceDetails, details);

         std::string protocol = details.get<std::string>("writingProtocol");
         std::string source = details.get<std::string>("source");

         if (m_deviceManager->isHandled(source))
         {
            boost::shared_ptr<xplrules::IRule> rule = m_deviceManager->identifyRule(protocol, m_instanceManager);

            if (rule)
            {
               //check if the rule handle reading
               boost::shared_ptr<xplrules::ICommandRule> commandRule = boost::dynamic_pointer_cast<xplrules::ICommandRule>(rule);

               if (commandRule)
               {
                  boost::shared_ptr< xplcore::CXplMessage > messageToSend = commandRule->createXplCommand(command, source);
                  if (messageToSend)
                  {
                     messageToSend->setSource(xplService.getActor());
                     xplService.sendMessage(*messageToSend.get());
                  }
                  else
                  {
                     //send result
                     std::string errorMessage = "Fail to create the Xpl message to send to the device";
                     YADOMS_LOG(error) << errorMessage;
                  }

               }
               else
               {
                  std::string errorMessage = (boost::format("The protocol %1% do not support commands") % protocol).str();
                  YADOMS_LOG(error) << errorMessage;
               }
            }
            else
            {
               std::string errorMessage = (boost::format("Unsupported protocol = %1%") % protocol).str();
               YADOMS_LOG(error) << errorMessage;
            }
         }
         else
         {
            std::string errorMessage = (boost::format("Unknown xpl source = %1%") % source).str();
            YADOMS_LOG(error) << errorMessage;
         }

      }

   }
   catch (std::exception &ex)
   {
      std::string errorMessage = (boost::format("xpl plugin fail to send message : %1%") % ex.what()).str();
      YADOMS_LOG(error) << errorMessage;
   }
}