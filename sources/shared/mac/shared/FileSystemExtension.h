//
// FileSystemExtension.h
//
// File system extensions
//
#pragma once

#include <shared/exception/Exception.hpp>
#include <dlfcn.h>

namespace shared
{

   //--------------------------------------------------------------
   /// \brief	this class is used to manage platforms peripherals
   //--------------------------------------------------------------
   class CFileSystemExtension
   {
   public:
      //--------------------------------------------------------------
      /// \brief			Constructor
      //--------------------------------------------------------------
      CFileSystemExtension();

      //--------------------------------------------------------------
      /// \brief			Destructor
      //--------------------------------------------------------------
      virtual ~CFileSystemExtension();

      //--------------------------------------------------------------
      /// \brief			Get the current module path.
      ///               This code must be executed in the module which the path is required,
      ///               so let it in the header file.
      //--------------------------------------------------------------
      static const boost::filesystem::path getModulePath()
      { 
         Dl_info dl_info; 
         if (dladdr((void *)getModulePath, &dl_info) == 0) 
         { 
            const std::string error(strerror(errno));
            throw shared::exception::CException(std::string("CFileSystemExtension::getModulePath fails with error ") + error);
         } 
         
         /* We've got the path, construct a path from it */
         boost::filesystem::path fullPath(dl_info.dli_fname);
         return fullPath.parent_path();
      }

   };

} // namespace shared