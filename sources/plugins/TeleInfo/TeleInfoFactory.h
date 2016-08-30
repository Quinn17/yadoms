#pragma once

#include "ITeleInfoConfiguration.h"
#include <shared/communication/IAsyncPort.h>
#include "TeleInfoReceiveBufferHandler.h"
#include "ITransceiver.h"

//--------------------------------------------------------------
/// \brief	General factory
//--------------------------------------------------------------
class CTeleInfoFactory
{
public:
   //--------------------------------------------------------------
   /// \brief	    Destructor
   //--------------------------------------------------------------
   virtual ~CTeleInfoFactory();

   //--------------------------------------------------------------
   /// \brief	                           Create a port instance and connect to the TeleInfo Receiver
   /// \param[in] configuration           Plugin instance configuration (contains the serial port)
   /// \param[in] eventHandler            The event handler to receive notifications from port
   /// \param[in] evtPortConnectionId     The event id raised on connection events
   /// \param[in] evtPortDataReceived     The event id raised on data receive events
   /// \return                            The created port
   //--------------------------------------------------------------
   static boost::shared_ptr<shared::communication::IAsyncPort> constructPort(const ITeleInfoConfiguration& configuration,
                                                                             shared::event::CEventHandler& eventHandler,
                                                                             boost::shared_ptr<CTeleInfoReceiveBufferHandler> receiveBufferHandler,
                                                                             int evtPortConnectionId);

   //--------------------------------------------------------------
   /// \brief	                           Create a transceiver instance
   /// \return                            The created transceiver
   //--------------------------------------------------------------
   static boost::shared_ptr<ITransceiver> constructTransceiver();

   //--------------------------------------------------------------
   /// \brief	                          Return the buffer handler of the teleInfo
   /// \return                            The Buffer handler
   //--------------------------------------------------------------
   static boost::shared_ptr<CTeleInfoReceiveBufferHandler> GetBufferHandler(shared::event::CEventHandler& eventHandler,
                                                                            int evtPortDataReceived,
                                                                            size_t messageSize);
};

