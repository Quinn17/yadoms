#pragma once
#include <shared/Export.h>
#include "Capacity.h"

/*
Besoins des widgets

	1. Lister les devices qui fournissent une donn�e (temp�rature, hygro,...)
	2. Lister les devices qui supportent des commandes (potentiellement jamais �mises)
	3. Envoyer des commandes � un device
	4. Recevoir des donn�es d'un device
	
	

Fonctions des plugins

	1. Communiquent avec les hardwares (lien s�rie, r�seau, ...)
	2. Cr�er/rechercher un device
	3. Historiser des donn�es
	4. Piloter les hardwares
	5. Connaissent les fonctions des hardwares (savent d�crire leurs capacit�s : envoi de commande, sonde temperature)
	
	

	
	
Roadmap

	1. d�finir IYadomsApi
	2. d�finir une capacit� (mot cl�, read/write, ...)
	3. d�finir les interfaces de communications (conteneur de donn�es, de commandes,....)
	4. diagrammes de s�quences
	5. d�finir une liste de capacit�s avec leur d�tails implicites (unit�s par exemple, ....)
   
   
D�tails/remarques :
   
   1. Il existera une instance de l'impl�mentation de IYadomsApi par instance de plugin. Ca permet de ne pas diffuser l'instanceId au code du plugin (entre autre) et
   d'une mani�re g�n�rale que cet objet contienne des donn�es/m�thodes concernant l'instance du plugin (comme l'eventHandler) en plus des donn�es/m�thodes globales de Yadoms.
   2. L'instance de plugin re�oit tous les �v�nements (kEvtUpdateConfiguration, kDeviceCommand) sans besoin de s'abonner explicitement. Les �v�nements ne sont de toutes
   fa�ons lev�s que si actif (kEvtUpdateConfiguration lev� que si le plugin contient une configuration, kDeviceCommand que si le plugin a d�clar� un device canWrite).
	
   
Questions:

   1. gestion des exceptions ou retour par bool sur les m�thodes de l'api ? ou  alors m�me en retournant un int � la fa�on linux
   
   
Notes:
SG> Mon avis sur les questions :
   1. Je suis favorable aux exceptions, plus objet, � condition que �a ne g�n�re pas d'incompatibilit� (plugin compil� avec un autre compilo que Yadoms). A tester.
*/

namespace shared { namespace plugin { namespace yadomsApi
{
   class IYadomsApi
   {
   public:
      //-----------------------------------------------------
      ///\brief Destructor
      //-----------------------------------------------------
      virtual ~IYadomsApi() {}

      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //--
      //-- EVENTS
      //--
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      enum
      {
         //-----------------------------------------------------
         ///\brief The plugin receive an event from Yadoms
         //-----------------------------------------------------
         kEvtUpdateConfiguration = kUserFirstId,      // Yadoms notify the plugin that its configuration was changed
         kDeviceCommand                               // Yadoms send a command to a device managed by this plugin
      };

      //-----------------------------------------------------
      ///\brief Container class for device commands sent by server
      //TODO not yet finalized ;-)
      //-----------------------------------------------------
      class CDeviceCommand
      {
         CCapacity m_capacity;
         std::string m_value;
         std::string targetDeviceName;
      };   
      
      
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //--
      //-- DEVICES
      //--
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      
      //-----------------------------------------------------
      ///\brief Check if a device already exists for the server
      ///\param    [in]    deviceName         The device name (must be unique)
      ///\return true if the device exists, false if not
      //-----------------------------------------------------
      virtual bool deviceExists(const std::string & deviceName) = 0;

      //-----------------------------------------------------
      ///\brief Declare a new device
      ///\param    [in]    deviceName         The device name (must be unique)
      ///\param    [in]    capacities         The device capacity list
      ///\return true if the device has been successfully created, false if not
      ///\throw   CAlreadyExistException if the device already exists
      ///\throw   CBadParameterException if the plugin instance id is not known
      //-----------------------------------------------------   
      virtual bool declareNewDevice(const std::string & deviceName, std::vector< CCapacity > & capacities) = 0;
      
      
      
      //-----------------------------------------------------
      ///\brief Historize a new data values
      ///\param    [in]    deviceName         The device name (must be unique)
      ///\param    [in]    capacity           The device capacity (temperature, rssi,...)
      ///\param    [in]    value              The capacity value
      ///\return true if the data has been successfully stored, false if not
      ///\throw   CAlreadyExistException if the device already exists
      ///\throw   CBadParameterException if the plugin instance id is not known
      //-----------------------------------------------------     
      virtual bool historizeData(const std::string & deviceName, const CCapacity & capacity, const std::string & value) = 0;
      
      
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //--
      //-- Plugin event recorder
      //--
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      
      
      //-----------------------------------------------------
      ///\brief The plugin event record level
      //-----------------------------------------------------      
      typedef enum
      {
         kInfo,
         kError,
      }PluginEventSeverity;
      
      //-----------------------------------------------------
      ///\brief Record a plugin major event (recorded in Yadoms database)
      ///\param    [in]    severity           The message severity
      ///\param    [in]    message            The message
      ///\return true if the message has been successfully stored, false if not
      //-----------------------------------------------------      
      virtual bool recordPluginEvent(PluginEventSeverity severity, const std::string & message) = 0;
      
      
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //--
      //-- IO_SERVICE
      //--
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------------------------------------   
      //-----------------------------------------------------
      ///\brief Get the global io_service
      ///\return A reference (non copyable) to the io_service
      //-----------------------------------------------------   
      virtual boost::asio::io_service & getPluginsIoService() = 0;
   };
	
} } } // namespace shared::plugin::yadomsApi	
	
	