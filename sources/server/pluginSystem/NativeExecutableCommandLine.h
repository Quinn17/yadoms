#pragma once
#include <shared/process/ICommandLine.h>

namespace pluginSystem
{
   //--------------------------------------------------------------
   /// \brief	Command line generator
   //--------------------------------------------------------------
   class CNativeExecutableCommandLine : public shared::process::ICommandLine
   {
   public:
      //--------------------------------------------------------------
      /// \brief	Constructor
      /// \param[in] workingDirectory  Working directory
      /// \param[in] executableName    Executable name
      /// \param[in] parameters        Command line arguments
      //--------------------------------------------------------------
      CNativeExecutableCommandLine(const boost::filesystem::path& workingDirectory,
                                   const std::string& executableName,
                                   const std::vector<std::string> parameters);

      //--------------------------------------------------------------
      /// \brief	Destructor
      //--------------------------------------------------------------
      virtual ~CNativeExecutableCommandLine();

      // ICommandLine Implementation
      std::string executable() const override;
      const boost::filesystem::path& workingDirectory() const override;
      const std::vector<std::string>& args() const override;
      // [END] ICommandLine Implementation

   private:
      const boost::filesystem::path m_workingDirectory;
      const std::string m_pluginName;
      const std::vector<std::string> m_args;
   };

} // namespace pluginSystem