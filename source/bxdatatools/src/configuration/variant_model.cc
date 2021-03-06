// datatools/configuration/variant_model.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <datatools/configuration/variant_model.h>

// Standard library:
#include <sstream>
#include <set>
#include <iomanip>

// This project:
#include <datatools/exception.h>
#include <datatools/multi_properties.h>
#include <datatools/ioutils.h>
#include <datatools/configuration/parameter_model.h>
// #include <datatools/configuration/parameter_physical.h>

namespace datatools {

  namespace configuration {

    // virtual
    bool variant_model::is_name_valid(const std::string & name_) const
    {
      // std::cerr << "DEVEL: variant_model::is_name_valid: name = '" << name_ << "'" << std::endl;
      return ::datatools::configuration::validate_model_name(name_);
    }

    variant_model::variant_model()
    {
      _initialized_ = false;
      _set_default();
      return;
    }

    variant_model::~variant_model()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void variant_model::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Parameter model '" << get_name() << "' is not initialized!");
      _initialized_ = false;
      // _ranked_parameters_.clear();
      _parameters_.clear();
      this->enriched_base::clear();
      _set_default();
      return;
    }

    void variant_model::_set_default()
    {
      return;
    }

    bool variant_model::has_documentation() const
    {
      return ! _documentation_.empty();
    }

    void variant_model::set_documentation(const std::string & documentation_)
    {
      _documentation_ = documentation_;
      return;
    }

    const std::string & variant_model::get_documentation() const
    {
      return _documentation_;
    }

    bool variant_model::is_rank_used(int rank_) const
    {
      if (rank_ >= 0) {
        for (const auto & parrec : _parameters_) {
          if (parrec.second.rank == rank_) return true;
        }
      }
      return false;
    }

    bool variant_model::is_ranked_parameter(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found = _parameters_.find(parameter_name_);
      DT_THROW_IF(found == _parameters_.end(), std::logic_error,
                  "No parameter with name '" << parameter_name_ << "'!");
      return found->second.rank >= 0;
    }

    void variant_model::build_list_of_ranked_parameters(std::vector<std::string> & ranked_params_) const
    {
      ranked_params_.clear();
      std::map<int, std::string> ranked;
      std::set<std::string> unranked;
      for (const auto & parrec : _parameters_) {
        int rk = parrec.second.rank;
        if (rk >= 0) {
          ranked[rk] = parrec.first;
        } else {
          unranked.insert(parrec.first);
        }
      }
      ranked_params_.reserve(ranked.size() + unranked.size());
      for (const auto & rk : ranked) {
        ranked_params_.push_back(rk.second);
      }
      for (const auto & urk : unranked) {
        ranked_params_.push_back(urk);
      }
      return;
    }

    void variant_model::set_parameter_rank(const std::string & parameter_name_, int rank_)
    {
      parameter_dict_type::iterator found = _parameters_.find(parameter_name_);
      DT_THROW_IF(found == _parameters_.end(), std::logic_error,
                  "No parameter with name '" << parameter_name_ << "'!");
      DT_THROW_IF(rank_ < 0, std::domain_error,
                  "Invalid rank for parameter named '" << parameter_name_ << "'!");
      found->second.rank = rank_;
      return;
    }

    int variant_model::get_parameter_rank(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found = _parameters_.find(parameter_name_);
      DT_THROW_IF(found == _parameters_.end(), std::logic_error,
                  "No parameter with name '" << parameter_name_ << "'!");
      return found->second.rank;
    }

    bool variant_model::has_parameter(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      return found_parameter != _parameters_.end();
    }

    void variant_model::remove_parameter(const std::string & parameter_name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Configuration variant model '" << get_name() << "' is locked !");
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      DT_THROW_IF(found_parameter != _parameters_.end(), std::logic_error,
                  "Configuration variant model '" << get_name() << "' "
                  << "does not have a parameter named '" << parameter_name_ << "' !");
      _parameters_.erase(parameter_name_);
      return;
    }

    void variant_model::add_parameter(const std::string & parameter_name_,
                                      const pm_handle_type & parameter_model_handle_,
                                      const std::string & description_,
                                      const std::string & occurrence_def_,
                                      int rank_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Configuration variant model '" << get_name() << "' is locked !");
      DT_THROW_IF(!validate_instance_name(parameter_name_), std::logic_error,
                  "Parameter name '" << parameter_name_ << "' is not allowed "
                  << "in configuration variant model '" << get_name() << "'!");
      DT_THROW_IF(!parameter_model_handle_, std::logic_error,
                  "Not a valid parameter model for parameter named '" << parameter_name_ << "' "
                  << "in configuration variant model '" << get_name() << "' !");
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      DT_THROW_IF(found_parameter != _parameters_.end(), std::logic_error,
                  "Configuration variant model '" << get_name() << "' "
                  << "already has a parameter named '" << parameter_name_ << "' !");
      DT_THROW_IF(is_rank_used(rank_), std::logic_error,
                  "Configuration variant model '" << get_name() << "' "
                  << "already has a parameter with rank [" << rank_ << "]!");
      {
        parameter_record dummy;
        _parameters_.insert(std::pair<std::string, parameter_record>(parameter_name_, dummy));
      }
      parameter_record & pp = _parameters_.find(parameter_name_)->second;
      pp.description = description_;
      pp.physical.set(parameter_name_, pp.description, parameter_model_handle_);
      pp.physical.install_occurrence(occurrence_def_);
      if (rank_ >= 0) {
        pp.rank = rank_;
      }
      return;
    }

    const std::string &
    variant_model::get_parameter_description(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      DT_THROW_IF(found_parameter == _parameters_.end(),
                  std::logic_error,
                  "Variant model '" << get_name() << "' has no parameter named '"
                  << parameter_name_ << "' !");
      return found_parameter->second.description;
    }

    const parameter_model &
    variant_model::get_parameter_model(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      DT_THROW_IF(found_parameter == _parameters_.end(),
                  std::logic_error,
                  "Variant model '" << get_name() << "' has no parameter named '"
                  << parameter_name_ << "' !");
      return found_parameter->second.physical.get_model();
    }

    pm_handle_type
    variant_model::get_parameter_model_handle(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      DT_THROW_IF(found_parameter == _parameters_.end(),
                  std::logic_error, "Variant model '" << get_name() << "' has no parameter named '"
                  << parameter_name_ << "' !");
      return found_parameter->second.physical.get_model_handle();
    }

    bool
    variant_model::has_parameter_occurrence(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      DT_THROW_IF(found_parameter == _parameters_.end(),
                  std::logic_error, "Variant model '" << get_name() << "' has no parameter named '"
                  << parameter_name_ << "' !");
      DT_THROW_IF(!found_parameter->second.physical.has_occurrence(),
                  std::logic_error, "Variant model '" << get_name() << "' with parameter named '"
                  << parameter_name_ << "' without explicit occurrence!");
      return found_parameter->second.physical.has_occurrence();
    }

    const i_occurrence &
    variant_model::get_parameter_occurrence(const std::string & parameter_name_) const
    {
      parameter_dict_type::const_iterator found_parameter = _parameters_.find(parameter_name_);
      DT_THROW_IF(found_parameter == _parameters_.end(),
                  std::logic_error, "Variant model '" << get_name() << "' has no parameter named '"
                  << parameter_name_ << "' !");
      return found_parameter->second.physical.get_occurrence();
    }

    bool variant_model::is_initialized() const
    {
      return _initialized_;
    }

    void variant_model::initialize_simple()
    {
      properties dummy_setup;
      initialize_standalone(dummy_setup);
      return;
    }

    void variant_model::initialize_standalone(const properties & setup_)
    {
      model_item_dict_type dummy_dict;
      initialize(setup_, dummy_dict);
      return;
    }

    void variant_model::initialize(const properties & config_, model_item_dict_type & items_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Variant model '" << get_name() << "' is already locked !");

      enriched_base::initialize(config_);

      // Daughter parameters:
      /* Example with 3 embedded parameters using the same parameter model:
       *
       *  parameters : string[3] = "length" "width" "boxes"
       *
       *  parameters.length.model       : string = "distance.PM"
       *  parameters.length.description : string = "Length of the plate"
       *  parameters.length.occurence   : string = "single"
       *
       *  parameters.width.model        : string = "distance.PM"
       *  parameters.width.description  : string = "Width of the plate"
       *
       *  parameters.boxes.model        : string = "distance.PM"
       *  parameters.boxes.description  : string = "Width of the plate"
       *  parameters.boxes.occurence    : string = "array size=4 start_id=0"
       *
       */
      if (config_.has_key("parameters")) {
        std::vector<std::string> par_names;
        std::set<int> ranks;
        config_.fetch("parameters", par_names);
        for (int i = 0; i < (int) par_names.size(); i++) {
          int par_rank = i;
          const std::string & par_name = par_names[i];
          DT_THROW_IF(has_parameter(par_name), std::logic_error,
                      "Parameter '" << par_name << "' is already used!");
          std::ostringstream par_model_ss;
          par_model_ss << "parameters." << par_name << ".model";
          DT_THROW_IF(!config_.has_key(par_model_ss.str()), std::logic_error,
                      "Property '" << par_model_ss.str() << "' is missing!");
          const std::string & par_model = config_.fetch_string(par_model_ss.str());
          model_item_dict_type::const_iterator found = items_.find(par_model);
          DT_THROW_IF(found == items_.end(), std::logic_error,
                      "Item '" << par_model << "' does not exist!");
          const model_item & par_item = found->second;
          DT_THROW_IF(! par_item.is_parameter(), std::logic_error,
                      "Item '" << par_model << "' is not a parameter!");
          std::ostringstream par_desc_ss;
          par_desc_ss << "parameters." << par_name << ".description";
          std::string par_desc;
          if (config_.has_key(par_desc_ss.str())) {
            par_desc = config_.fetch_string(par_desc_ss.str());
          }
          std::ostringstream par_occ_ss;
          par_occ_ss << "parameters." << par_name << ".occurrence";
          std::string par_occ_def;
          if (config_.has_key(par_occ_ss.str())) {
            par_occ_def = config_.fetch_string(par_occ_ss.str());
          }
          if (par_occ_def.empty()) {
            par_occ_def = "single";
          }
          std::ostringstream par_rank_ss;
          par_rank_ss << "parameters." << par_name << ".rank";
          if (config_.has_key(par_rank_ss.str())) {
            par_rank = config_.fetch_integer(par_rank_ss.str());
          }
          DT_THROW_IF (par_rank < 0, std::logic_error, "Invalid rank for parameter '" << par_name << "'!");
          DT_THROW_IF (ranks.count(par_rank) > 0, std::logic_error, "Rank '" << par_rank << "' for parameter '" << par_name << "' is already used!");

          add_parameter(par_name,
                        par_item.get_parameter_handle(),
                        par_desc,
                        par_occ_def);
          if (par_rank >= 0) {
            set_parameter_rank(par_name, par_rank);
          }
          ranks.insert(par_rank);
        }
      }

      _initialized_ = true;
      return;
    }

    const variant_model::parameter_dict_type & variant_model::get_parameters() const
    {
      return _parameters_;
    }

    void variant_model::tree_dump(std::ostream& out_,
                                  const std::string& title_,
                                  const std::string& indent_,
                                  bool inherit_) const
    {
      this-> enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::tag
           << "Documentation  : ";
      if (has_documentation()) {
        out_ << "yes";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Parameters     : ";
      if (_parameters_.size()) {
        out_ << _parameters_.size();
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      for (parameter_dict_type::const_iterator i = _parameters_.begin();
           i != _parameters_.end();
           i++) {
        parameter_dict_type::const_iterator j = i;
        j++;
        std::ostringstream indent_2;
        out_ << indent_ << i_tree_dumpable::skip_tag;
        indent_2 << indent_ << i_tree_dumpable::skip_tag ;
        if (j == _parameters_.end()) {
          out_ << i_tree_dumpable::last_tag;
          indent_2 << i_tree_dumpable::last_skip_tag;
        } else {
          out_ << i_tree_dumpable::tag;
          indent_2 << i_tree_dumpable::skip_tag;
        }
        out_ << "Parameter '" << i->first << "' (model '" << i->second.physical.get_model().get_name() << "')";
        if (!i->second.description.empty()) {
          out_ << " : " << i->second.description;
        }
        if (i->second.rank >= 0) {
          out_ << " [rank=" << i->second.rank << "]";
        }
        out_ << std::endl;
        // const std::string & param_name = i->first;
        // out_ << "Parameter '" << param_name << "' with model: ";
        // out_ << std::endl;
        // const parameter_model & param = i->second.get_model();
        // param.tree_dump(out_, "", indent_2.str(), false);
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized    : "
           << this->is_initialized() << std::endl;

      return;
    }

    /// Print in ReST format
    void variant_model::print_rst(std::ostream & out_,
                                  const std::string & indent_,
                                  uint32_t flags_) const
    {
      bool with_title = true;
      if (flags_ & PRINT_RST_NO_TITLE) {
        with_title = false;
      }

      if (with_title) {
        std::ostringstream titleoss;
        titleoss << "Variant model ``" << get_name() << "``";
        out_ << std::setw(titleoss.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << titleoss.str() << std::endl;
        out_ << std::setw(titleoss.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << std::endl;
      }
      std::string indent = indent_;

      if (has_terse_description()) {
        print_multi_lines(out_, get_terse_description(), indent);
        out_ << std::endl;
      }

      if (has_documentation()) {
        out_ << indent << std::endl;
        //out_ << indent << "* Documentation: " << std::endl;
        print_multi_lines(out_, get_documentation(), indent);
        out_ << indent << std::endl;
      }

      out_ << indent << std::endl;
      out_ << indent << "* Display name: ";
      if (! has_display_name()) {
        out_ << "<none provided>";
      } else {
        out_ << "``\"" << get_display_name() << "\"``";
      }
      out_ << std::endl;

      /*
      {
        std::size_t count = 0;

        std::vector<std::string> param_paths;
        build_list_of_ranked_parameter_records(param_paths);

        out_ << "* Parameters: ``" << param_paths.size() << "``" << std::endl;
        count++;

        out_ << indent << "* Parameters : ";
        if (_parameters_.size() == 0) {
          out_ << "<none>";
        }
        out_ << std::endl;
        for (std::size_t iparam = 0; iparam < param_paths.size(); iparam++) {
          const std::string & varParamName = param_paths[iparam];
          if (iparam == 0) {
            out_ << std::endl;
          }
          const variant_record & varParRec = vreg.get_parameter_record(varParamName);
          const parameter_model & varParModel = varParRec.get_parameter_model();

          out_ << "  * Parameter ``" << '"' << varParamName << '"'
               << "`` (model: ``"
               << '"' << varParModel.get_name() << '"'
               << "``) :"
               << std::endl;
          out_ << std::endl;

          const std::string & varParLeafName = varParRec.get_leaf_name();
          const variant_record & varParentRec = varParRec.get_parent();
          const variant_model & varParentModel = varParentRec.get_variant_model();
          const std::string & param_desc = varParentModel.get_parameter_description(varParLeafName);
          if (!param_desc.empty()) {
            out_ << "    * Description: ``";
            out_ << param_desc;
            out_ << "``" << std::endl;
          }
          out_ << "    * Full path: ``" << '"' << vreg_key << ':' << varParamName << '"' << "``" << std::endl;
          uint32_t varParModelRstOpt = 0;
          varParModelRstOpt |= parameter_model::PRINT_RST_NO_TITLE;
          varParModelRstOpt |= parameter_model::PRINT_RST_NO_DESC;
          varParModel.print_rst(out_, "    ", varParModelRstOpt);
          out_ << std::endl;
        }
        if (count) {
          out_ << std::endl;
        }
      } // for
      */

      for (parameter_dict_type::const_iterator i = _parameters_.begin();
           i != _parameters_.end();
           i++) {
        parameter_dict_type::const_iterator j = i;
        j++;
        if (i == _parameters_.begin()) {
          out_ << std::endl;
        }
        out_ << indent << "  * Parameter '" << i->first
             << "' : model '" << i->second.physical.get_model().get_name() << "'";
        if (!i->second.description.empty()) {
          out_ << " : " << i->second.description;
        }
        out_ << std::endl;
        if (j == _parameters_.end()) {
          out_ << std::endl;
        }
      }
      out_ << indent << std::endl;

      return;
    }

    // static
    void variant_model::init_ocd(datatools::object_configuration_description & ocd_)
    {

      enriched_base::init_ocd(ocd_);

      return;
    }


  }  // end of namespace configuration

}  // end of namespace datatools

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(datatools::configuration::variant_model, ocd_)
{
  // The class name :
  ocd_.set_class_name ("datatools::configuration::variant_model");

  // The origin of the object:
  // ocd_.set_from ("datatools::configuration::variant_model");

  // The class terse description :
  ocd_.set_class_description ("The description of a configuration variant_model");

  // The library the class belongs to :
  ocd_.set_class_library ("datatools");

  // The class detailed documentation :
  ocd_.set_class_documentation ("A example class of configurable object with OCD support");

  // Document the various configuration parameters :
  datatools::configuration::variant_model::init_ocd(ocd_);

  // Set validation support
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for the class :
DOCD_CLASS_SYSTEM_REGISTRATION(datatools::configuration::variant_model, "datatools::configuration::variant_model")
