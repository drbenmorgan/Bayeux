// resource.cc - Implementation of genbb_help resource
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of genbb_help.
//
// genbb_help is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// genbb_help is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with genbb_help.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <genbb_help/resource.h>

// Standard Library
#include <iostream>
#include <fstream>
#include <cstdlib>

// Third Party
// Boost
#include <boost/filesystem.hpp>
// - datatools
#include <datatools/exception.h>
#include <datatools/logger.h>

// This Project
#include <genbb_help/genbb_help_config.h>
// - Bayeux
#include <bayeux/reloc.h>

namespace {
  //! Return the path to the root resource directory
  std::string get_resource_root(bool overriden_env) {
    if (overriden_env) {
      const char * env_key = GENBB_HELP_ENV_RESOURCE_DIR;
      if (std::getenv(env_key)) {
        return std::string(std::getenv(env_key));
      }
    }
    static boost::filesystem::path install_resource_root;
    if (install_resource_root.empty()) {
      install_resource_root = bayeux::get_resource_dir();
      install_resource_root /= "genbb_help";
    }
    return install_resource_root.string();
  }
} // namespace

namespace genbb {

std::string get_resource_dir(bool overriden_env) {
  return get_resource_root(overriden_env);
}

std::string get_resource(const std::string& rname, bool overriden_env) {
  DT_THROW_IF(rname[0] == '/', std::logic_error, "invalid resource name");

  std::string fullpath(get_resource_root(overriden_env) + "/" + rname);
  std::ifstream check(fullpath.c_str());
  DT_THROW_IF(!check.good(),
              std::runtime_error,
              "Unreadable resource '" << fullpath << "'")
  return fullpath;
}
} // namespace genbb
