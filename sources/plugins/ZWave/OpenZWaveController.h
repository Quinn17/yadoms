#pragma once

#include "IZWaveController.h"
#include <command_classes/CommandClasses.h>
#include <command_classes/CommandClass.h>
#include "OpenZWaveNode.h"

class COpenZWaveController : public IZWaveController
{
public:
   //--------------------------------------------------------------
   /// \brief	    Constructor
   //--------------------------------------------------------------
   COpenZWaveController();

   //--------------------------------------------------------------
   /// \brief	    Destructor
   //--------------------------------------------------------------
   virtual ~COpenZWaveController();

   // IZWaveController implementation
   void configure(CZWaveConfiguration* configuration,
                  shared::event::CEventHandler* handler) override;
   E_StartResult start() override;
   void stop() override;
   void sendCommand(const std::string& device,
                    const std::string& keyword,
                    const std::string& value) override;
   void startInclusionMode() override;
   void startExclusionMode() override;
   void hardResetController() override;
   void softResetController() override;
   void testNetwork(int count) override;
   void cancelCurrentCommand() override;
   void healNetwork() override;
   // [END] IZWaveController implementation


   //-----------------------------------------------------------------------------
   /// \brief	Callback that is triggered when a value, group or node changes
   /// \param [in]    _notification    The openzwave notification
   /// \param [in]    content          The context
   //-----------------------------------------------------------------------------
   void onNotification(OpenZWave::Notification const* _notification,
                       void* _context);

private:
   //-----------------------------------------------------------------------------
   /// \brief	Return the NodeInfo object associated with this notification
   //-----------------------------------------------------------------------------   
   boost::shared_ptr<COpenZWaveNode> getNode(OpenZWave::Notification const* _notification);

   //-----------------------------------------------------------------------------
   /// \brief	Return the NodeInfo object matching homeId and nodeId
   //-----------------------------------------------------------------------------   
   boost::shared_ptr<COpenZWaveNode> getNode(const uint32 homeId,
                                             const uint8 nodeId);

   //-----------------------------------------------------------------------------
   /// \brief	Ask configuration parameters to each found node
   //-----------------------------------------------------------------------------   
   void RequestConfigurationParameters();

   //--------------------------------------------------------------
   /// \brief	   Retreive openzwave ids from yadoms data
   /// \param [in]    device	            the yadoms device
   /// \param [in]    keyword	            the yadoms keyword
   /// \param [out]   homeId	            the openzwave homeid
   /// \param [out]   nodeId	            the openzwave nodeId
   /// \param [out]   instance            the openzwave instance
   /// \throw shared::exception::CException if any yadoms data do not match pattern
   /// \note:  patterns:   device = homeId.nodeId
   /// \note:  patterns:   keyword = keywordName.keywordClass
   //--------------------------------------------------------------
   void retreiveOpenZWaveIds(const std::string& device,
                             const std::string& keyword,
                             uint32& homeId,
                             uint8& nodeId,
                             uint8& instance);

   //--------------------------------------------------------------
   /// \brief	   Mutex protecting the configuration content
   //--------------------------------------------------------------
   mutable boost::mutex m_treeMutex;


   //--------------------------------------------------------------
   /// \brief	   The current HomeId
   //--------------------------------------------------------------
   uint32 m_homeId;

   //--------------------------------------------------------------
   /// \brief	   Tells if initialization failed
   //--------------------------------------------------------------
   bool m_initFailed;

   //--------------------------------------------------------------
   /// \brief	   Tells id all nodes have been queried
   //--------------------------------------------------------------
   bool m_nodesQueried;

   //--------------------------------------------------------------
   /// \brief	   Contains the last successfully sent command to controller
   //--------------------------------------------------------------
   OpenZWave::Driver::ControllerCommand m_lastSuccessfullySentCommand;

   //--------------------------------------------------------------
   /// \brief	   The zwave node list type
   //--------------------------------------------------------------
   typedef std::vector<boost::shared_ptr<COpenZWaveNode> > NodeListType;

   //--------------------------------------------------------------
   /// \brief	   The zwave node list
   //--------------------------------------------------------------
   NodeListType m_nodes;

   //--------------------------------------------------------------
   /// \brief	   The EventHandler of ZWave plugin
   //--------------------------------------------------------------
   shared::event::CEventHandler* m_handler;

   //--------------------------------------------------------------
   /// \brief	   The plugin configuration
   //--------------------------------------------------------------
   CZWaveConfiguration* m_configuration;
};

