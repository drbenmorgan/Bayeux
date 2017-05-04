//! \file    lahague/resource.h
//! \brief   Utilities for accessing LaHague resource files
//! \details The LaHague library makes use of several resource files
//!          containing data about dosimetry and radioprotection.
//!
//!          As these files are not compiled into the LaHague library,
//!          lahague provides a simple API to get a path to known
//!          resource files.
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

#ifndef LAHAGUE_RESOURCE_H
#define LAHAGUE_RESOURCE_H

// Standard Library:
#include <string>

namespace lahague {

  //! Return URL, i.e. a path, to the  base directory where resource files are installed
  //! \param overriden_env_ If set this flag trigger the search for the
  //!        environment variable LAHAGUE_RESOURCE_DIR as a directory path on the filesystem
  //!        as an alternative base directory for resources in place of the
  //!        standard installation path
  std::string get_resource_dir(bool overriden_env_ = false);

  //! Return URL, i.e. a path, to named resource
  //! By default the encoded resource root, determined at compile time
  //! is used to search for the resource. This can be overriden by setting
  //! the LAHAGUE_RESOURCE_DIR environment variable to a path holding custom
  //! resources.
  //! \param rname_ name of resource, given as a path relative to
  //!        resource root.
  //! \param overriden_env_ flag to allow path overriding by the LAHAGUE_RESOURCE_DIR environment variable.
  std::string get_resource(const std::string& rname_, bool overriden_env_ = false);

} // namespace lahague

#endif // LAHAGUE_RESOURCE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
