// logger.cc - Implementation of datatools logger
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include "datatools/logger.h"

// Standard Library:

// Third Party:
// - Boost:
#include "boost/assign.hpp"
#include "boost/algorithm/string.hpp"
// Clang doesn't like bimap's use of BOOST_PP...
#if defined (__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wredeclared-class-member"
#endif

#include "boost/bimap.hpp"
#include "boost/bimap/set_of.hpp"
#include "boost/bimap/multiset_of.hpp"

#if defined (__clang__)
#pragma clang diagnostic pop
#endif

// This Project:
#include "datatools/exception.h"
#include "datatools/properties.h"
#include <datatools/object_configuration_description.h>

namespace {

  //! Hide boost bimap declaration in a typedef
  struct PriorityLookup {
    typedef boost::bimap<
      boost::bimaps::set_of<std::string>,
      boost::bimaps::multiset_of<datatools::logger::priority>
      > LookupTable;
  };

  //! Construct the lookup table.
  PriorityLookup::LookupTable ConstructLookupTable() {
    PriorityLookup::LookupTable a;
    boost::assign::insert(a)
      ("PRIO_FATAL", datatools::logger::PRIO_FATAL)
      ("FATAL", datatools::logger::PRIO_FATAL)
      ("fatal", datatools::logger::PRIO_FATAL)
      ("PRIO_CRITICAL", datatools::logger::PRIO_CRITICAL)
      ("CRITICAL", datatools::logger::PRIO_CRITICAL)
      ("critical", datatools::logger::PRIO_CRITICAL)
      ("PRIO_ERROR", datatools::logger::PRIO_ERROR)
      ("ERROR", datatools::logger::PRIO_ERROR)
      ("error", datatools::logger::PRIO_ERROR)
      ("PRIO_WARNING", datatools::logger::PRIO_WARNING)
      ("WARNING", datatools::logger::PRIO_WARNING)
      ("warning", datatools::logger::PRIO_WARNING)
      ("PRIO_NOTICE", datatools::logger::PRIO_NOTICE)
      ("NOTICE", datatools::logger::PRIO_NOTICE)
      ("notice", datatools::logger::PRIO_NOTICE)
      ("PRIO_INFORMATION", datatools::logger::PRIO_INFORMATION)
      ("INFORMATION", datatools::logger::PRIO_INFORMATION)
      ("information", datatools::logger::PRIO_INFORMATION)
      ("PRIO_DEBUG", datatools::logger::PRIO_DEBUG)
      ("DEBUG", datatools::logger::PRIO_DEBUG)
      ("debug", datatools::logger::PRIO_DEBUG)
      ("PRIO_TRACE", datatools::logger::PRIO_TRACE)
      ("TRACE", datatools::logger::PRIO_TRACE)
      ("trace", datatools::logger::PRIO_TRACE);

    return a;
  }

  //! Return priority label stripped of "PRIO_" prefix and lowercased
  std::string GetCanonicalLabel(const std::string & raw_)
  {
    return boost::to_lower_copy(boost::ireplace_first_copy(raw_, "PRIO_", ""));
  }

} // namespace

namespace datatools {

  logger::priority logger::get_priority(const std::string & name_)
  {
    static PriorityLookup::LookupTable a;
    if (a.empty()) a = ConstructLookupTable();

    PriorityLookup::LookupTable::left_const_iterator p = a.left.find(name_);
    return p != a.left.end() ? p->second : PRIO_UNDEFINED;
  }

  std::string logger::get_priority_label(logger::priority p_)
  {
    static PriorityLookup::LookupTable a;
    if (a.empty()) a = ConstructLookupTable();

    PriorityLookup::LookupTable::right_const_iterator n = a.right.find(p_);
    return n != a.right.end() ? GetCanonicalLabel(n->second) : "";
  }

  bool logger::is_undefined(priority p_)
  {
    return p_ == PRIO_UNDEFINED;
  }

  bool logger::is_fatal(priority p_)
  {
    return p_ >= PRIO_FATAL;
  }

  bool logger::is_critical(priority p_)
  {
    return p_ >= PRIO_CRITICAL;
  }

  bool logger::is_error(priority p_)
  {
    return p_ >= PRIO_ERROR;
  }

  bool logger::is_warning(priority p_)
  {
    return p_ >= PRIO_WARNING;
  }

  bool logger::is_notice(priority p_)
  {
    return p_ >= PRIO_NOTICE;
  }

  bool logger::is_information(priority p_)
  {
    return p_ >= PRIO_INFORMATION;
  }

  bool logger::is_debug(priority p_)
  {
    return p_ >= PRIO_DEBUG;
  }

  bool logger::is_trace(priority p)
  {
    return p >= PRIO_TRACE;
  }

  logger::priority logger::extract_logging_configuration(const datatools::properties & config_,
                                                         logger::priority default_prio_,
                                                         bool throw_on_error_)
  {
    datatools::logger::priority p = default_prio_;
    if (config_.has_key("logging.priority")) {
      std::string ps = config_.fetch_string("logging.priority");
      p = datatools::logger::get_priority(ps);
      if (p == datatools::logger::PRIO_UNDEFINED) {
        DT_THROW_IF(throw_on_error_,
                    std::logic_error,
                    "Invalid logging priority label '" << ps << "' !");
        p = default_prio_;
      }
    } else if (config_.has_flag("debug") || config_.has_flag("logging.debug")) {
      p = datatools::logger::PRIO_DEBUG;
    } else if (config_.has_flag("verbose") || config_.has_flag("logging.verbose")) {
      p = datatools::logger::PRIO_NOTICE;
    }
    return p;
  }

  void logger::declare_ocd_logging_configuration(datatools::object_configuration_description & ocd_,
                                                 const std::string & default_value_,
                                                 const std::string & prefix_,
                                                 const std::string & from_)
  {
    {
      std::ostringstream desc;
      desc <<  "Allowed values are:                      \n"
        "                                                \n"
        "  * ``\"trace\"`` : Heavy development messages  \n"
        "  * ``\"debug\"`` : Debug messages              \n"
        "  * ``\"information\"`` :                       \n"
        "  * ``\"notice\"`` :                            \n"
        "  * ``\"warning\"`` :                           \n"
        "  * ``\"error\"`` :                             \n"
        "  * ``\"critical\"`` :                          \n"
        "  * ``\"fatal\"`` : Only fatal error messages   \n"
        "                                                \n";
      std::ostringstream example;
      example <<
        "Set the logging priority to allow the printing  \n"
        "of informational messages: ::                   \n"
        "                                                \n";
      example << "  " << prefix_;
      example << "logging.priority : string = \"notice\" \n"
        "                                                \n";

      configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern(prefix_ + "logging.priority")
        .set_terse_description("Set the logging priority threshold")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string(default_value_.empty() ? "fatal": default_value_)
        .set_long_description(desc.str())
        .add_example(example.str())
        ;
      if (! from_.empty()) {
        cpd.set_from(from_);
      }
    }

    return;
  }

} // namespace datatools
