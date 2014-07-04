#pragma once

#include <shared/Export.h>
#include <shared/exception/OutOfRange.hpp>
#include <shared/exception/InvalidParameter.hpp>
#include "serialization/IDataSerializable.h"
#include "serialization/IDataFileSerializable.h"
#include "IDataContainable.h"

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_enum.hpp>

namespace shared
{
   //--------------------------------------------------------------
   /// \brief	This class should be used as a data container
   ///			The aim is to handle any data; like configuration
   ///			
   ///			The container should contains data by a get/set method
   ///			Each data is associated to a key name
   ///			The data could be a simple type, a list of data, or a DataContainer child
   ///
   ///			The container has the serialize feature
   ///			This is done be serializing/deserializing a string,
   ///			or a file
   ///			
   ///			***********************
   ///			** USAGE
   ///			***********************
   ///			A/ Simple type USAGE
   ///			
   ///			   1. simple data 
   ///			   	shared::CDataContainer test;
   ///			   	test.set("intValue", 5);
   ///			   	test.set("doubleValue", 4.0);
   ///			   	test.set("stringValue", "plouf");
   ///			   
   ///			   	int i = test.get<int>("intValue");
   ///			   	double d = test.get<double>("doubleValue");
   ///			   	std::string s = test.get<std::string>("stringValue");
   ///			   
   ///			   
   ///			   2. vector of data
   ///			   	std::vector<int> vi;
   ///			   	...
   ///			   	test.set("vectorint", vi);
   ///			   
   ///			   	std::vector<double> vd;
   ///			   	...
   ///			   	test.set("vectordouble", vd);
   ///			   
   ///			   
   ///			   	std::vector<int> vi2 = test.get< std::vector<int> >("vectorint");
   ///			   	std::vector<double> vd2 = test.get< std::vector<double> >("vectordouble");
   ///			   
   ///			   
   ///			   
   ///			   3. sub container (act as a tree)
   ///               
   ///               //create another container
   ///               shared::CDataContainer subContainer;
   ///               subContainer.set("int4", 6);
   ///               subContainer.set("double1", 8.0);
   ///               subContainer.set("string2", "plouf2");
   ///               
   ///               //add it to main container
   ///               test.set("config1", subContainer);
   ///               //retreive container
   ///               shared::CDataContainer subt = test.get<shared::CDataContainer>("config1");
   ///               //direct access to a sub container data
   ///               double d = test.get<double>("config1.double1");
   ///
   ///            4. boost::shared_ptr<>
   ///               The use of shared_ptr is completly transparent. It works like if you get/set directly the encapsulated type
   ///               
   ///               //works with boost::shared_ptr of any simple type
   ///               //you can put a shared_ptr and get the data direclty without using shared_ptr for reading
   ///
   ///               boost::shared_ptr<std::string> shs(new std::string("un shared ptr"));
   ///               test.set("StringSharedPtr", shs);
   ///               
   ///               boost::shared_ptr<std::string> shs2 = test.get< boost::shared_ptr<std::string> >("StringSharedPtr");
   ///               std::string direct = test.get< std::string >("StringSharedPtr");
   ///               
   ///             5. std::vector< boost::shared_ptr<> >
   ///               
   ///               //check vector of shared_ptr<int>
   ///               std::vector< boost::shared_ptr<int> > vish;
   ///               ....
   ///               test.set("vectorintsh", vish);
   ///               std::vector< boost::shared_ptr<int> > vish2 = test.get< std::vector< boost::shared_ptr<int> > >("vectorintsh");
   ///               std::vector< int > vi2 = test.get< std::vector< int > >("vectorintsh");
   ///            
   ///
   ///         B. Enhanced usage (IDataContainable)
   ///            This is an interface which allow any class to be get/set in this container.
   ///            This interface have only two methods to override, one for filling the class instance from a container, 
   ///            and another to fill the container from the class instance
   ///
   ///            Example:
   ///               
   ///               class CTestClass : public shared::IDataContainable
   ///               {
   ///               public:
   ///                  CTestClass(int i, double d, std::string s)
   ///                     :m_aIntValue(i), m_dValue(d), m_sValue(s)
   ///                  {
   ///               
   ///                  }
   ///               
   ///                  virtual void extractContent(shared::CDataContainer & cont) const
   ///                  {
   ///                     cont.set("Value1", m_aIntValue);
   ///                     cont.set("Value2", m_dValue);
   ///                     cont.set("Value3", m_sValue);
   ///                  }
   ///               
   ///                  virtual void fillFromContent(const shared::CDataContainer & initialData)
   ///                  {
   ///                     m_aIntValue = initialData.get<int>("Value1");
   ///                     m_dValue = initialData.get<double>("Value2");
   ///                     m_sValue = initialData.get<std::string>("Value3");
   ///                  }
   ///               
   ///               private:
   ///                  int m_aIntValue;
   ///                  double m_dValue;
   ///                  std::string m_sValue;
   ///               };
   ///
   ///            1. Simple use
   ///               shared::CDataContainer cont;
   ///
   ///               CTestClass obj(1, 42.0, "test de datacontainble");
   ///               cont.set("myobject", obj);
   ///               CTestClass result = cont.get<CTestClass>("myobject");
   ///               
   ///            2. boost::shared_ptr<IDataContainable>
   ///
   ///               boost::shared_ptr<CTestClass> sp(new CTestClass(2, 43.0, "chaine1"));
   ///               cont.set("myobjectsh", sp);
   ///               boost::shared_ptr<CTestClass> result2 = cont.get< boost::shared_ptr<CTestClass> >("myobjectsh");
   ///               CTestClass result3 = cont.get< CTestClass >("myobjectsh");
   ///
   ///            3. std::vector<shared::IDataContainable>
   ///               
   ///               std::vector<CTestClass> vc;
   ///               for (int i = 0; i < 10; ++i)
   ///                  vc.push_back(CTestClass(i, 42.0 * i, "test de std::vector<IDataContainable>"));
   ///               shared::CDataContainer contvec;
   ///               contvec.set("mycollection", vc);
   ///               std::vector<CTestClass> vc2 = contvec.get< std::vector<CTestClass> >("mycollection");
   ///               
   ///             4. std::vector< boost::shared_ptr<IDataContainable> >
   ///
   ///               std::vector< boost::shared_ptr<CTestClass> > vcsh;
   ///               for (int i = 0; i < 10; ++i)
   ///                  vcsh.push_back(boost::shared_ptr<CTestClass>(new CTestClass(i, 42.0 * i, "test de std::vector<IDataContainable>")));
   ///               shared::CDataContainer contvecsh;
   ///               contvecsh.set("mycollectionofshared", vcsh);
   ///               std::vector< boost::shared_ptr<CTestClass> >  vcsh2 = contvecsh.get< std::vector< boost::shared_ptr<CTestClass> > >("mycollectionofshared");
   ///               std::vector< CTestClass >  vc2 = contvecsh.get< std::vector< CTestClass > >("mycollectionofshared");
   ///
   ///
   //--------------------------------------------------------------
   class YADOMS_SHARED_EXPORT CDataContainer : public serialization::IDataSerializable, public serialization::IDataFileSerializable /*, public IDataContainable*/
   {
   public:
      //--------------------------------------------------------------
      //
      //
      //
      // Constructors & destructor
      //
      //
      //
      //--------------------------------------------------------------

      //--------------------------------------------------------------
      /// \brief		Constructor. Empty data
      //--------------------------------------------------------------
      CDataContainer();

      //--------------------------------------------------------------
      /// \brief		Constructor. 
      /// \param [in] initialData    Initial data for this container (will be deserialized)
      //--------------------------------------------------------------
      CDataContainer(const std::string & initialData);

      //--------------------------------------------------------------
      /// \brief		Constructor. 
      /// \param [in] initialData    Initial data for this container (will be deserialized)
      //--------------------------------------------------------------
      CDataContainer(const CDataContainer & initialData);

      //--------------------------------------------------------------
      /// \brief			Destructor
      /// \return    	void
      //--------------------------------------------------------------
      virtual ~CDataContainer(void);


      //--------------------------------------------------------------
      //
      //
      //
      // Public get/set methods
      //
      //
      //
      //--------------------------------------------------------------

      //--------------------------------------------------------------
      /// \brief	    Get parameter value
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter value
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline T get(const std::string& parameterName) const;

      //--------------------------------------------------------------
      /// \brief	    Get parameter value. If the name is not found the default value is returned
      /// \param [in] parameterName    Name of the parameter
      /// \param [in] defaultValue     Default value
      /// \return     The parameter value
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline T get(const std::string& parameterName, const T & defaultValue) const;

      //--------------------------------------------------------------
      /// \brief	    Set parameter value
      /// \param [in] parameterName    Name of the parameter
      /// \param [in] value            Value of the parameter
      //--------------------------------------------------------------
      template<class T>
      inline void set(const std::string& parameterName, const T & value);

      //--------------------------------------------------------------
      //
      //
      //
      // Public get/set methods for ENUMS treated as string
      //
      //
      //
      //--------------------------------------------------------------

      //--------------------------------------------------------------
      /// \brief	    Type representing authorized enum values list
      //--------------------------------------------------------------
      typedef std::map<std::string, unsigned int> EnumValuesNames;

      //--------------------------------------------------------------
      /// \brief	    Get enum parameter value
      /// \param [in] parameterName    Name of the enum parameter
      /// \param[in]  valueNames  List of authorized values for the enum
      /// \return     The parameter value
      /// \throw      shared::exception::COutOfRange if parameter value is not in the provided list
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<typename EnumType>
      EnumType getEnumValue(const std::string& parameterName, const EnumValuesNames& valuesNames) const;


      //--------------------------------------------------------------
      //
      //
      //
      // Public initialization methods
      //
      //
      //
      //--------------------------------------------------------------


      //--------------------------------------------------------------
      /// \brief		Copy another container to this instance
      /// \param [in] rhs  The container to copy
      //--------------------------------------------------------------
      void initializeWith(const CDataContainer &rhs);

      //--------------------------------------------------------------
      //
      //
      //
      // Public utility methods
      //
      //
      //
      //--------------------------------------------------------------

      //--------------------------------------------------------------
      /// \brief	    Check if parameter value is present
      /// \param [in] parameterName    Name of the parameter
      /// \return     true if parameter found
      //--------------------------------------------------------------
      bool hasValue(const std::string& parameterName) const;


      //--------------------------------------------------------------
      /// \brief		Equality operator
      /// \param [in] rhs  The container to compare with
      /// \return   true if the container equals to this instance
      //--------------------------------------------------------------
      bool operator ==(const CDataContainer &rhs) const;

      //--------------------------------------------------------------
      /// \brief		Difference operator
      /// \param [in] rhs  The container to compare with
      /// \return   true if the container is different to this instance
      //--------------------------------------------------------------
      bool operator !=(const CDataContainer &rhs) const;

      //--------------------------------------------------------------
      /// \brief		Affectation operator
      /// \param [in] rhs  The container to copy
      /// \return   Reference to this instance
      //--------------------------------------------------------------
      CDataContainer & operator=(const CDataContainer &rhs);

      //--------------------------------------------------------------
      /// \brief		Affectation operator
      /// \param [in] rhs  A serialized string
      /// \return   Reference to this instance
      //--------------------------------------------------------------
      CDataContainer & operator=(const std::string &rhs);

      //--------------------------------------------------------------
      /// \brief		Check if the container is empty
      /// \return    true if contanier is empty
      //--------------------------------------------------------------
      bool empty() const;


      //--------------------------------------------------------------
      //
      //
      //
      // Interface implementations
      //
      //
      //
      //--------------------------------------------------------------

      // IDataSerializable implementation
      virtual std::string serialize() const;
      virtual void deserialize(const std::string & data);
      // [END] IDataSerializable implementation

      // IDataFileSerializable implementation
      virtual void serializeToFile(const std::string & filename) const;
      virtual void deserializeFromFile(const std::string & filename);
      // [END] IDataFileSerializable implementation

      // IDataContainable implementation
      virtual void extractContent(CDataContainer &) const;
      virtual void fillFromContent(const CDataContainer & initialData);
      // [END] IDataContainable implementation


      //--------------------------------------------------------------
      //
      //
      //
      // Static helpers
      //
      //
      //
      //--------------------------------------------------------------


      //--------------------------------------------------------------
      /// \brief		An empty container which could be used as default method parameter
      //--------------------------------------------------------------
      const static CDataContainer EmptyContainer;


   protected:
      //--------------------------------------------------------------
      /// \brief		Constructor. 
      /// \param [in] initialData    Initial tree for this container
      //--------------------------------------------------------------
      CDataContainer(const boost::property_tree::ptree & initialTree);


      //--------------------------------------------------------------
      //
      //
      //
      // Internal get/set methods
      //
      //
      //
      //--------------------------------------------------------------

      //--------------------------------------------------------------
      /// \brief	    Get parameter value
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter value
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline T getInternal(const std::string& parameterName) const;

      //--------------------------------------------------------------
      /// \brief	    Get parameter value (T is IDataContainable)
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter value (T is IDataContainable)
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline T getInternalIDataContainable(const std::string& parameterName) const;

      //--------------------------------------------------------------
      /// \brief	    Get parameter values
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter values
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline std::vector<T> getValuesInternal(const std::string& parameterName) const;

      //--------------------------------------------------------------
      /// \brief	    Get parameter values (T is IDataContainable)
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter values (T is IDataContainable)
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline std::vector<T> getValuesInternalIDataContainable(const std::string& parameterName) const;


      //--------------------------------------------------------------
      /// \brief	    Get parameter values (boost::shared_ptr<T>)
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter values (boost::shared_ptr<T>)
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline std::vector< boost::shared_ptr<T> > getValuesSPInternal(const std::string& parameterName) const;

      //--------------------------------------------------------------
      /// \brief	    Get parameter values (boost::shared_ptr<T> and T is IDataContainable)
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter values (boost::shared_ptr<T> and T is IDataContainable)
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline std::vector< boost::shared_ptr<T> > getValuesSPIDataContainableInternal(const std::string& parameterName) const;


      //--------------------------------------------------------------
      /// \brief	    Set parameter value
      /// \param [in] parameterName    Name of the parameter
      /// \param [in] value            Value of the parameter
      //--------------------------------------------------------------
      template<class T>
      inline void setInternal(const std::string& parameterName, const T & value);

      //--------------------------------------------------------------
      /// \brief	    Set parameter value (T is IDataContainable)
      /// \param [in] parameterName    Name of the parameter
      /// \param [in] value            Value of the parameter (T is IDataContainable)
      //--------------------------------------------------------------
      template<class T>
      inline void setInternalIDataContainable(const std::string& parameterName, const T & value);
      
      //--------------------------------------------------------------
      /// \brief	    Set parameter values
      /// \param [in] parameterName    Name of the parameter
      /// \param [in] values           Valuse of the parameter
      //--------------------------------------------------------------
      template<class T>
      inline void setValuesInternal(const std::string& parameterName, const std::vector<T> & values);

      //--------------------------------------------------------------
      /// \brief	    Get parameter values (IDataContainable)
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter values (IDataContainable)
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline void setValuesInternalIDataContainable(const std::string& parameterName, const std::vector<T> & values);

		//--------------------------------------------------------------
		/// \brief	    Get parameter values (Enum)
		/// \param [in] parameterName    Name of the parameter
		/// \return     The parameter values (Enum)
		/// \throw      shared::exception::COutOfRange if parameter can not be converted
		/// \throw      shared::exception::CInvalidParameter if parameter is not found
		//--------------------------------------------------------------
		template<class T>
		inline std::vector<T> getValuesInternalEnum(const std::string& parameterName) const;


		//--------------------------------------------------------------
		/// \brief	    Set parameter values (Enum)
		/// \param [in] parameterName    Name of the parameter
		/// \param [in] values           Valuse of the parameter (Enum)
		//--------------------------------------------------------------
		template<class T>
		inline void setValuesInternalEnum(const std::string& parameterName, const std::vector<T> & values);

      //--------------------------------------------------------------
      /// \brief	    Get parameter values (std::vector< boost::shared_ptr<T> >)
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter values (std::vector< boost::shared_ptr<T> >)
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      template<class T>
      inline void setValuesSPInternal(const std::string& parameterName, const std::vector< boost::shared_ptr<T> > & values);

      //--------------------------------------------------------------
      /// \brief	    Get parameter values (std::vector< boost::shared_ptr<IDataContainable> >)
      /// \param [in] parameterName    Name of the parameter
      /// \return     The parameter values (std::vector< boost::shared_ptr<IDataContainable> >)
      /// \throw      shared::exception::COutOfRange if parameter can not be converted
      /// \throw      shared::exception::CInvalidParameter if parameter is not found
      //--------------------------------------------------------------
      inline void setValuesSPIDataContainableInternal(const std::string& parameterName, const std::vector< boost::shared_ptr<IDataContainable> > & values);

      //--------------------------------------------------------------
      //
      //
      //
      // Internal helpers (allow redirecting to the good get/set internal method)
      //
      //
      //
      //--------------------------------------------------------------

      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with single value type (int, double, std::string,...)
      //--------------------------------------------------------------
      template <typename T, class Enable = void>
      struct helper
      {

         //--------------------------------------------------------------
         /// \brief	    GET Method for all standard types (int, double, std::string,...)
         //--------------------------------------------------------------
         static T getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return tree->getInternal<T>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for all standard types (int, double, std::string,...)
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const T & value)
         {
            tree->setInternal<T>(parameterName, value);
         }

      };



      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with boost::shared_ptr
      //--------------------------------------------------------------
      template <typename T>
      struct helper < boost::shared_ptr< T > >
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for boost::shared_ptr<T>
         //--------------------------------------------------------------
         static boost::shared_ptr< T > getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return boost::make_shared<T>(helper<T>::getInternal(tree, parameterName));
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for boost::shared_ptr<T>
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const boost::shared_ptr< T > & value)
         {
            helper<T>::setInternal(tree, parameterName, *value.get());
         }
      };



      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with IDataContainable object
      //--------------------------------------------------------------
      template <typename T>
      struct helper < T, typename boost::enable_if< boost::is_base_of< IDataContainable, T > >::type >
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for IDataContainable object
         //--------------------------------------------------------------
         static T getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return tree->getInternalIDataContainable<T>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for IDataContainable object
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const T & value)
         {
            tree->setInternalIDataContainable<T>(parameterName, value);
         }

      };


      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with enum
      //--------------------------------------------------------------
      template <typename T>
      struct helper < T, typename boost::enable_if< boost::is_enum< T > >::type >
      {

         //--------------------------------------------------------------
         /// \brief	    GET Method for enumeration
         //--------------------------------------------------------------
         static T getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return (T)tree->getInternal<int>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for enumeration
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const T & value)
         {
            tree->setInternal<int>(parameterName, (int)value);
         }

      };










      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with vector of value type (vector<int>, vector<double>, vector<class>,...)
      //--------------------------------------------------------------
      template <typename T, class Enable = void >
      struct vectorhelper
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for std::vector< T >
         //--------------------------------------------------------------
         static std::vector< T > getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return tree->getValuesInternal<T>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for std::vector< T >
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const std::vector< T > & value)
         {
            tree->setValuesInternal(parameterName, value);
         }
      };

      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with vector of IDataContainable objects
      //--------------------------------------------------------------
      template <typename T>
      struct vectorhelper < T, typename boost::enable_if< boost::is_base_of< IDataContainable, T > >::type >
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for std::vector< IDataContainable >
         //--------------------------------------------------------------
         static std::vector< T > getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return tree->getValuesInternalIDataContainable<T>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for std::vector< IDataContainable >
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const std::vector< T > & value)
         {
            tree->setValuesInternalIDataContainable(parameterName, value);
         }
      };

      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with vector of Enum
      //--------------------------------------------------------------
      template <typename T>
		struct vectorhelper < T, typename boost::enable_if< boost::is_enum< T > >::type >
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for std::vector< Enum >
         //--------------------------------------------------------------
         static std::vector< T > getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return tree->getValuesInternalEnum<T>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for std::vector< Enum >
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const std::vector< T > & value)
         {
            tree->setValuesInternalEnum(parameterName, value);
         }
      };


      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with vector of value type (vector<int>, vector<double>, vector<class>,...)
      //--------------------------------------------------------------
      template <typename T>
      struct helper < std::vector< T > >
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for std::vector< T >
         //--------------------------------------------------------------
         static std::vector<T> getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return vectorhelper<T>::getInternal(tree, parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for std::vector< T >
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const std::vector<T> & value)
         {
            vectorhelper<T>::setInternal(tree, parameterName, value);
         }
      };






      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with vector of shared_ptr of simple type ( std::vector< boost::shared_ptr<int> >,... )
      //--------------------------------------------------------------
      template <typename T, class Enable = void >
      struct vhelper
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for std::vector< boost::shared_ptr<T> >
         //--------------------------------------------------------------
         static std::vector< boost::shared_ptr<T> > getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return tree->getValuesSPInternal<T>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for std::vector< boost::shared_ptr<T> >
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const std::vector<  boost::shared_ptr<T> > & value)
         {
            tree->setValuesSPInternal(parameterName, value);
         }
      };

      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with vector of shared_ptr of IDataContainable ( std::vector< boost::shared_ptr<IDataContainable> >)
      //--------------------------------------------------------------
      template <typename T>
      struct vhelper < T, typename boost::enable_if< boost::is_base_of< IDataContainable, T > >::type >
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for std::vector< boost::shared_ptr<IDataContainable> >
         //--------------------------------------------------------------
         static std::vector< boost::shared_ptr<T> > getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return tree->getValuesSPIDataContainableInternal<T>(parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for std::vector< boost::shared_ptr<IDataContainable> >
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const std::vector<  boost::shared_ptr<T> > & value)
         {
            std::vector<  boost::shared_ptr<IDataContainable> > compatibleVector;
            typename std::vector<  boost::shared_ptr<T> >::const_iterator i;
            for (i = value.begin(); i != value.end(); ++i)
               compatibleVector.push_back(boost::dynamic_pointer_cast<IDataContainable>(*i));
            tree->setValuesSPIDataContainableInternal(parameterName, compatibleVector);
         }
      };

      //--------------------------------------------------------------
      /// \brief	    Helper structure for get/set with vector of value type ( std::vector< boost::shared_ptr<T> > )
      //--------------------------------------------------------------
      template <typename T>
      struct helper < std::vector< boost::shared_ptr<T> > >
      {
         //--------------------------------------------------------------
         /// \brief	    GET Method for std::vector< boost::shared_ptr<T> >
         //--------------------------------------------------------------
         static std::vector< boost::shared_ptr<T> > getInternal(const CDataContainer * tree, const std::string& parameterName)
         {
            return vhelper<T>::getInternal(tree, parameterName);
         }

         //--------------------------------------------------------------
         /// \brief	    SET Method for std::vector< boost::shared_ptr<T> >
         //--------------------------------------------------------------
         static void setInternal(CDataContainer * tree, const std::string& parameterName, const std::vector< boost::shared_ptr<T> > & value)
         {
            vhelper<T>::setInternal(tree, parameterName, value);
         }

      };



   private:
      //--------------------------------------------------------------
      /// \brief	   The configuration content
      //--------------------------------------------------------------
      boost::property_tree::ptree m_tree;

      //--------------------------------------------------------------
      /// \brief	   Mutex protecting the configuration content
      //--------------------------------------------------------------
      mutable boost::mutex m_treeMutex;
   };





   //--------------------------------------------------------------
   /// \brief	    The below part of header contains all the function
   ///				definitions (out of class definition for better 
   ///				cross compiler compatibilities).
   ///				-> for template methods
   ///				-> for template specialization
   //--------------------------------------------------------------
   template<typename T>
   inline T CDataContainer::get(const std::string& parameterName) const
   {
      return helper<T>::getInternal(this, parameterName);
   }


   template<typename T>
   inline T CDataContainer::get(const std::string& parameterName, const T & defaultValue) const
   {
      if (hasValue(parameterName))
         return helper<T>::getInternal(this, parameterName);
      return defaultValue;
   }

   template<typename T>
   inline void CDataContainer::set(const std::string& parameterName, const T & value)
   {
      helper<T>::setInternal(this, parameterName, value);
   }



   template<class T>
   inline T CDataContainer::getInternal(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         return m_tree.get<T>(parameterName);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<>
   inline CDataContainer CDataContainer::getInternal(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         return CDataContainer(m_tree.get_child(parameterName));
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }


   template<>
   inline boost::posix_time::ptime CDataContainer::getInternal(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         return boost::posix_time::from_iso_string(m_tree.get<std::string>(parameterName));
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<class T>
   inline T CDataContainer::getInternalIDataContainable(const std::string& parameterName) const
   {
      try
      {
         CDataContainer t2 = getInternal<CDataContainer>(parameterName);
         T obj;
         obj.fillFromContent(t2);
         return obj;
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }






   template<class T>
   inline std::vector<T> CDataContainer::getValuesInternal(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      std::vector<T> result;
      try
      {
         boost::property_tree::ptree child = m_tree.get_child(parameterName);

         boost::property_tree::ptree::const_iterator end = child.end();
         for (boost::property_tree::ptree::const_iterator it = child.begin(); it != end; ++it) {
            result.push_back(it->second.get_value<T>());
         }
         return result;
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<class T>
   inline std::vector<T> CDataContainer::getValuesInternalEnum(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      std::vector<T> result;
      try
      {
         boost::property_tree::ptree child = m_tree.get_child(parameterName);

         boost::property_tree::ptree::const_iterator end = child.end();
         for (boost::property_tree::ptree::const_iterator it = child.begin(); it != end; ++it) {
            result.push_back((T)it->second.get_value<int>());
         }
         return result;
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<class T>
   inline std::vector< boost::shared_ptr<T> > CDataContainer::getValuesSPInternal(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      std::vector< boost::shared_ptr<T> > result;
      try
      {
         boost::property_tree::ptree child = m_tree.get_child(parameterName);

         boost::property_tree::ptree::const_iterator end = child.end();
         for (boost::property_tree::ptree::const_iterator it = child.begin(); it != end; ++it)
         {
            boost::shared_ptr<T> sp = boost::make_shared<T>(it->second.get_value<T>());
            result.push_back(sp);
         }
         return result;
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<class T>
   inline std::vector< boost::shared_ptr<T> > CDataContainer::getValuesSPIDataContainableInternal(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      std::vector< boost::shared_ptr<T> > result;
      try
      {
         boost::property_tree::ptree child = m_tree.get_child(parameterName);

         boost::property_tree::ptree::const_iterator end = child.end();
         for (boost::property_tree::ptree::const_iterator it = child.begin(); it != end; ++it)
         {
            boost::shared_ptr<T> sp(new T);
            sp->fillFromContent(CDataContainer(it->second));
            result.push_back(sp);
         }
         return result;
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }


   template<>
   inline std::vector< boost::shared_ptr<IDataContainable> > CDataContainer::getValuesSPInternal(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      std::vector<boost::shared_ptr<IDataContainable> > result;
      try
      {
         boost::property_tree::ptree child = m_tree.get_child(parameterName);

         boost::property_tree::ptree::const_iterator end = child.end();
         for (boost::property_tree::ptree::const_iterator it = child.begin(); it != end; ++it)
         {
            boost::shared_ptr<IDataContainable> sp;
            result.push_back(sp);
         }
         return result;
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }



   template<class T>
   inline std::vector<T> CDataContainer::getValuesInternalIDataContainable(const std::string& parameterName) const
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      std::vector<T> result;
      try
      {
         boost::property_tree::ptree child = m_tree.get_child(parameterName);

         boost::property_tree::ptree::const_iterator end = child.end();
         for (boost::property_tree::ptree::const_iterator it = child.begin(); it != end; ++it)
         {
            T a;
            a.fillFromContent(CDataContainer(it->second));
            result.push_back(a);
         }
         return result;
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }










   template<class T>
   inline void CDataContainer::setInternal(const std::string& parameterName, const T & value)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         m_tree.put(parameterName, value);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<class T>
   inline void CDataContainer::setInternalIDataContainable(const std::string& parameterName, const T & value)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         CDataContainer subTree;
         value.extractContent(subTree);
         m_tree.add_child(parameterName, subTree.m_tree);

      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }



   template<>
   inline void CDataContainer::setInternal(const std::string& parameterName, const CDataContainer & value)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         m_tree.add_child(parameterName, value.m_tree);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }


   template<>
   inline void CDataContainer::setInternal(const std::string& parameterName, const boost::posix_time::ptime & value)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         m_tree.put(parameterName, boost::posix_time::to_iso_string(value));
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }




   template<class T>
   inline void CDataContainer::setValuesInternal(const std::string& parameterName, const std::vector<T> & values)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         boost::property_tree::ptree innerData;

         typename std::vector<T>::const_iterator i;
         for (i = values.begin(); i != values.end(); ++i)
         {
            boost::property_tree::ptree t;
            t.put("", *i);
            innerData.push_back(std::make_pair("", t));
         }

         m_tree.add_child(parameterName, innerData);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<class T>
   inline void CDataContainer::setValuesInternalEnum(const std::string& parameterName, const std::vector<T> & values)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         boost::property_tree::ptree innerData;

         typename std::vector<T>::const_iterator i;
         for (i = values.begin(); i != values.end(); ++i)
         {
            boost::property_tree::ptree t;
            t.put("", (int)(*i));
            innerData.push_back(std::make_pair("", t));
         }

         m_tree.add_child(parameterName, innerData);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   template<class T>
   inline void CDataContainer::setValuesInternalIDataContainable(const std::string& parameterName, const std::vector<T> & values)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         boost::property_tree::ptree innerData;

         typename std::vector<T>::const_iterator i;
         for (i = values.begin(); i != values.end(); ++i)
         {
            CDataContainer t;
            i->extractContent(t);
            innerData.push_back(std::make_pair("", t.m_tree));
         }

         m_tree.add_child(parameterName, innerData);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }



   template<class T>
   inline void CDataContainer::setValuesSPInternal(const std::string& parameterName, const std::vector< boost::shared_ptr<T> > & values)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         boost::property_tree::ptree innerData;

         typename std::vector< boost::shared_ptr<T> >::const_iterator i;
         for (i = values.begin(); i != values.end(); ++i)
         {
            boost::property_tree::ptree t;
            t.put("", *i->get());
            innerData.push_back(std::make_pair("", t));
         }

         m_tree.add_child(parameterName, innerData);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }

   inline void CDataContainer::setValuesSPIDataContainableInternal(const std::string& parameterName, const std::vector< boost::shared_ptr<IDataContainable> > & values)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         boost::property_tree::ptree innerData;

         std::vector< boost::shared_ptr<IDataContainable> >::const_iterator i;
         for (i = values.begin(); i != values.end(); ++i)
         {
            CDataContainer t;
            (*i)->extractContent(t);
            innerData.push_back(std::make_pair("", t.m_tree));
         }

         m_tree.add_child(parameterName, innerData);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }


   template<>
   inline void CDataContainer::setValuesInternal(const std::string& parameterName, const std::vector<CDataContainer> & values)
   {
      boost::lock_guard<boost::mutex> lock(m_treeMutex);

      try
      {
         boost::property_tree::ptree innerData;

         std::vector<CDataContainer>::const_iterator i;
         for (i = values.begin(); i != values.end(); ++i)
         {
            innerData.push_back(std::make_pair("", i->m_tree));
         }

         m_tree.add_child(parameterName, innerData);
      }
      catch (boost::property_tree::ptree_bad_path& e)
      {
         throw exception::CInvalidParameter(parameterName + " : " + e.what());
      }
      catch (boost::property_tree::ptree_bad_data& e)
      {
         throw exception::COutOfRange(parameterName + " can not be converted to expected type, " + e.what());
      }
   }



   template<typename EnumType>
   EnumType CDataContainer::getEnumValue(const std::string& parameterName, const EnumValuesNames& valuesNames) const
   {
      std::string stringValue = get<std::string>(parameterName);
      EnumValuesNames::const_iterator it = valuesNames.find(stringValue);
      if (it != valuesNames.end())
         return (EnumType)(it->second);

      throw exception::COutOfRange(std::string("Value ") + stringValue + " was not found for " + parameterName + " parameter");
   }

} // namespace shared