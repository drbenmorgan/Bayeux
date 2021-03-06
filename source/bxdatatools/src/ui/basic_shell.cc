// datatools/ui/basic_shell.cc - A generic basic shell
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <datatools/ui/basic_shell.h>

// Standard libraries:
#include <fstream>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
// - Readline:
#if DATATOOLS_WITH_READLINE == 1
#include <readline/readline.h>
#include <readline/history.h>
#endif // DATATOOLS_WITH_READLINE

// This project:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/multi_properties.h>
#include <datatools/detail/command_macros.h>
#include <datatools/ui/base_command.h>
#include <datatools/ui/base_command_interface.h>
#include <datatools/ui/shell_command_interface.h>
#include <datatools/ui/ihs.h>
#include <datatools/ui/utils.h>
#include <datatools/ui/traits.h>

namespace datatools {

  namespace ui {

    //! \brief Dynamic informations about the current parsing source
    struct parser_context
    {
      //! Default constructor
      parser_context();

      std::string macro_name;   //!< The name of the current macro (empty in parent shell)
      std::size_t line_counter; //!< The line counter in the current parser context
    };

    parser_context::parser_context()
    {
      line_counter = 0;
      return;
    }

    // static
    const unsigned int basic_shell::DEFAULT_HISTORY_TRUNCATE;

    //! \brief Private implementation working data
    struct basic_shell::pimpl_type
    {
      //! Default constructor
      pimpl_type();

      //! Reset
      void reset();

      bool        stop_requested;            //!< Terminate request
      std::string expanded_history_filename; //!< Expanded history filename
      std::list<parser_context> pcontexts;   //!< The list of current nested parser context
    };

    basic_shell::pimpl_type::pimpl_type()
    {
      stop_requested = false;
      return;
    }

    void basic_shell::pimpl_type::reset()
    {
      expanded_history_filename = "";
      stop_requested = false;
      return;
    }

    basic_shell::pimpl_type & basic_shell::_grab_pimpl()
    {
      if (_pimpl_ == nullptr) {
        _pimpl_.reset(new pimpl_type);
      }
      return *_pimpl_;
    }

    const basic_shell::pimpl_type & basic_shell::_get_pimpl() const
    {
      basic_shell * mutable_this = const_cast<basic_shell *>(this);
      return const_cast<basic_shell::pimpl_type &>(mutable_this->_grab_pimpl());
    }

    // **********************************************
    // *                                            *
    // *         B A S I C     S H E L L            *
    // *                                            *
    // **********************************************

    // static
    const std::string & basic_shell::default_prompt()
    {
      static const std::string _p("%n:%W> ");
      return _p;
    }

    // static
    const std::string & basic_shell::default_continuation_prompt()
    {
      static const std::string _p("> ");
      return _p;
    }

    // static
    const std::string & basic_shell::system_interface_name()
    {
      static const std::string _name("builtins");
      return _name;
    }

    void basic_shell::_set_defaults()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _name_.clear();
      _version_.reset();
      _prompt_ = default_prompt();
      _continuation_prompt_ = default_continuation_prompt();
      _default_path_   = ""; // ::datatools::ui::path::root_path();
      _user_ = "";
      _host_ = "";
      _exit_on_error_  = false;
      _using_splash_   = true;
#if DATATOOLS_WITH_READLINE == 1
      _using_readline_ = true;
      _using_history_  = true;
#else
      _using_readline_ = false;
      _using_history_  = false;
#endif // DATATOOLS_WITH_READLINE
      _history_add_only_on_success_ = true;
      _history_filename_.clear();
      _history_truncate_ = 0;
      _services_ = nullptr;
      _ihs_ = nullptr;
      _current_working_path_ = ""; //::datatools::ui::path::root_path();
      _external_system_interface_ = nullptr;
      return;
    }

    bool basic_shell::has_current_path() const
    {
      return !_current_working_path_.empty();
    }

    void basic_shell::set_current_path(const std::string & p_)
    {
      DT_THROW_IF(! has_ihs(),
                  std::logic_error,
                  "Cannot change current path because no IHS is set!");
      DT_THROW_IF(! _ihs_->is_interface(p_),
                  std::logic_error,
                  "Path '" << p_ << "' is not an interface path!");
      _current_working_path_ = p_;
      return;
    }

    const std::string & basic_shell::get_current_path() const
    {
      return _current_working_path_;
    }

    basic_shell::basic_shell(const std::string & name_)
    {
      _initialized_ = false;
      _logging_ = datatools::logger::PRIO_FATAL;
      _ihs_ = nullptr;
      _set_defaults();
      if (! name_.empty()) {
        set_name(name_);
      }
      return;
    }

    basic_shell::~basic_shell()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    datatools::logger::priority basic_shell::get_logging() const
    {
      return _logging_;
    }

    void basic_shell::set_logging(datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    void basic_shell::set_name(const std::string & name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      DT_THROW_IF(!datatools::ui::path::is_valid_name(name_),
                  std::logic_error,
                  "Invalid shell name '" << name_ << "'!");
      _name_ = name_;
      return;
    }

    bool basic_shell::has_name() const
    {
      return !_name_.empty();
    }

    const std::string & basic_shell::get_name() const
    {
      return _name_;
    }

    void basic_shell::set_title(const std::string & title_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _title_ = title_;
      return;
    }

    bool basic_shell::has_title() const
    {
      return !_title_.empty();
    }

    const std::string & basic_shell::get_title() const
    {
      return _title_;
    }

    bool basic_shell::has_version() const
    {
      return _version_ != boost::none;
    }

    void basic_shell::set_version(const ::datatools::version_id & version_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _version_ = version_;
      return;
    }

    const ::datatools::version_id & basic_shell::get_version() const
    {
      return _version_.get();
    }

    void basic_shell::set_prompt(const std::string & prompt_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _prompt_ = prompt_;
      if (_prompt_.empty()) {
        _prompt_ = default_prompt();
      } else if (_prompt_[_prompt_.length() - 1] != ' ') {
        _prompt_ += " ";
      }
      return;
    }

    const std::string & basic_shell::get_prompt() const
    {
      return _prompt_;
    }

    bool basic_shell::has_user() const
    {
      return !_user_.empty();
    }

    void basic_shell::set_user(const std::string & user_)
    {
      _user_ = user_;
      return;
    }

    const std::string & basic_shell::get_user() const
    {
      return _user_;
    }

    bool basic_shell::has_host() const
    {
      return !_host_.empty();
    }

    void basic_shell::set_host(const std::string & host_)
    {
      _host_ = host_;
      return;
    }

    const std::string & basic_shell::get_host() const
    {
      return _host_;
    }

    void basic_shell::set_continuation_prompt(const std::string & p_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _continuation_prompt_ = p_;
      if (_continuation_prompt_.empty()) {
        _continuation_prompt_ = default_prompt();
      } else if (_continuation_prompt_[_continuation_prompt_.length() - 1] != ' ') {
        _continuation_prompt_ += " ";
      }

      return;
    }

    const std::string & basic_shell::get_continuation_prompt() const
    {
      return _continuation_prompt_;
    }

    bool basic_shell::has_default_path() const
    {
      return !_default_path_.empty();
    }

    void basic_shell::set_default_path(const std::string & default_path_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      if (_ihs_ == nullptr) {
        DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                       "No IHS is defined! Default path maybe not valid!");
        _default_path_ = default_path_;
      } else {
        if (!_ihs_->is_interface(default_path_)) {
          DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                       "IHS has no interface with path '" << default_path_ << "'! Fall back to root path!");
          _default_path_ = ::datatools::ui::path::root_path();
        } else {
          _default_path_ = default_path_;
        }
      }
      return;
    }

    const std::string & basic_shell::get_default_path() const
    {
      return _default_path_;
    }

    std::string basic_shell::get_effective_prompt() const
    {
      std::string expanded;
      _expand_prompt(_prompt_, expanded);
      return expanded;
    }

    std::string basic_shell::get_effective_continuation_prompt() const
    {
      std::string expanded;
      _expand_prompt(_continuation_prompt_, expanded);
      return expanded;
    }

    void basic_shell::_expand_prompt(const std::string & prompt_rule_, std::string & expanded_prompt_) const
    {
      std::string tmp_prompt = boost::algorithm::replace_all_copy(prompt_rule_, "%n", get_name());
      tmp_prompt = boost::algorithm::replace_all_copy(tmp_prompt, "%w", _current_working_path_);
      std::string cwd_basename = datatools::ui::path::basename(_current_working_path_);
      tmp_prompt = boost::algorithm::replace_all_copy(tmp_prompt, "%W", cwd_basename);
      tmp_prompt = boost::algorithm::replace_all_copy(tmp_prompt, "%u", _user_);
      tmp_prompt = boost::algorithm::replace_all_copy(tmp_prompt, "%H", _host_);
      expanded_prompt_ = tmp_prompt;
      return;
    }

    void basic_shell::set_exit_on_error(bool aoe_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _exit_on_error_ = aoe_;
      return;
    }

    bool basic_shell::is_exit_on_error() const
    {
      return _exit_on_error_;
    }

    void basic_shell::set_using_splash(bool s_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _using_splash_ = s_;
      return;
    }

    bool basic_shell::is_using_splash() const
    {
      return _using_splash_;
    }

    void basic_shell::set_using_readline(bool s_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _using_readline_ = s_;
      return;
    }

    bool basic_shell::is_using_readline() const
    {
      return _using_readline_;
    }

    void basic_shell::set_using_history(bool s_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _using_history_ = s_;
      return;
    }

    bool basic_shell::is_using_history() const
    {
      return _using_history_;
    }

    void basic_shell::set_history_filename(const std::string & history_filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _history_filename_ = history_filename_;
      return;
    }

    const std::string & basic_shell::get_history_filename() const
    {
      return _history_filename_;
    }

    void basic_shell::set_history_truncate(unsigned int t_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _history_truncate_ = t_;
      return;
    }

    unsigned int basic_shell::get_history_truncate()
    {
      return _history_truncate_;
    }

    void basic_shell::set_history_add_only_on_success(bool f_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _history_add_only_on_success_ = f_;
      return;
    }

    bool basic_shell::is_history_add_only_on_success() const
    {
      return _history_add_only_on_success_;
    }

    bool basic_shell::has_services() const
    {
      return _services_ != nullptr;
    }

    void basic_shell::set_services(datatools::service_manager & services_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _services_ = &services_;
      return;
    }

    void basic_shell::reset_services()
    {
      _services_ = 0;
      return;
    }

    ::datatools::service_manager & basic_shell::grab_services()
    {
      DT_THROW_IF(!has_services(), std::logic_error, "No service manager is set!");
      return *_services_;
    }

    const ::datatools::service_manager & basic_shell::get_services() const
    {
      DT_THROW_IF(!has_services(), std::logic_error, "No service manager is set!");
      return *_services_;
    }

    bool basic_shell::has_system_interface() const
    {
      if (_external_system_interface_ != nullptr) return true;
      return _system_interface_.get() != nullptr;
    }

    void basic_shell::set_system_interface(shell_command_interface_type & si_)
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      DT_THROW_IF(_system_interface_.get() != nullptr, std::logic_error,
                  "Shell already has an embedded system interface!");
      _external_system_interface_ = &si_;
      if (! _external_system_interface_->has_target()) {
        DT_LOG_TRACE(get_logging(), "Install external system interface target!");
        _external_system_interface_->set_target(*this);
        _external_system_interface_->initialize_simple();
      }
      DT_LOG_TRACE_EXITING(get_logging());
      return;
    }

    basic_shell::shell_command_interface_type & basic_shell::_grab_system_interface()
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error, "Shell has no system interface!");
      if (_external_system_interface_ != nullptr) {
        return *_external_system_interface_;
      }
      return *_system_interface_.get();
    }

    const basic_shell::shell_command_interface_type & basic_shell::_get_system_interface() const
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error, "Shell has no system interface!");
      if (_external_system_interface_ != nullptr) {
        return *_external_system_interface_;
      }
      return *_system_interface_.get();
    }

    bool basic_shell::has_ihs() const
    {
      return _ihs_ != nullptr;
    }

    void basic_shell::set_ihs(ihs & ihs_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");
      _ihs_ = &ihs_;
      if (! _default_path_.empty() && !_ihs_->is_interface(_default_path_)) {
        DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                     "IHS has no interface with path '" << _default_path_ << "'! Fall back to root path!");
        _default_path_ = ::datatools::ui::path::root_path();
      }
      return;
    }

    ihs & basic_shell::grab_ihs()
    {
      DT_THROW_IF(!has_ihs(), std::logic_error, "No IHS is set!");
      return *_ihs_;
    }

    const ihs & basic_shell::get_ihs() const
    {
      DT_THROW_IF(!has_ihs(), std::logic_error, "No IHS is set!");
      return *_ihs_;
    }

    void basic_shell::builtin_command_names(std::vector<std::string> & cmd_names_) const
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error, "Shell '" << get_name() << "' has no system interface!");
      const shell_command_interface_type & sci = _get_system_interface();
      sci.build_command_names(cmd_names_);
      return;
    }

    bool basic_shell::has_builtin_command(const std::string & cmd_name_) const
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error,
                  "Shell '" << get_name() << "' has no system interface!");
      const shell_command_interface_type & sci = _get_system_interface();
      return sci.has_command(cmd_name_);
    }

    const base_command & basic_shell::get_builtin_command(const std::string & cmd_name_) const
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error,
                  "Shell '" << get_name() << "' has no system interface!");
      const shell_command_interface_type & sci = _get_system_interface();
      return sci.get_command(cmd_name_);
    }

    bool basic_shell::is_initialized() const
    {
      return _initialized_;
    }

    void basic_shell::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void basic_shell::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");

      if (_services_ == nullptr) {
        DT_LOG_WARNING(_logging_, "Shell has no access to any services!");
      }

      // Fetch configuration parameters:
      datatools::logger::priority p
        = datatools::logger::extract_logging_configuration(config_, _logging_, true);
      if (p != datatools::logger::PRIO_UNDEFINED) {
        set_logging(p);
      }

      // Shell name is mandatory:
      if (!has_name()) {
        if (config_.has_key("name")) {
          const std::string & name = config_.fetch_string("name");
          set_name(name);
        }
      }
      DT_THROW_IF(!has_name(), std::logic_error, "Shell has no name!");

      // Prompt:
      if (_prompt_.empty()) {
        if (config_.has_key("prompt")) {
          const std::string & prompt = config_.fetch_string("prompt");
          set_prompt(prompt);
        }
      }
      if (_prompt_.empty()) {
        _prompt_ = default_prompt();
      }

      // Continuation prompt:
      if (_continuation_prompt_.empty()) {
        if (config_.has_key("continuation_prompt")) {
          const std::string & continuation_prompt = config_.fetch_string("continuation_prompt");
          set_continuation_prompt(continuation_prompt);
        }
      }
      if (_continuation_prompt_.empty()) {
        _continuation_prompt_ = default_continuation_prompt();
      }

      if (config_.has_key("default_path")) {
        const std::string & defpath = config_.fetch_string("default_path");
        set_default_path(defpath);
      }

      if (config_.has_key("exit_on_error")) {
        set_exit_on_error(config_.fetch_boolean("exit_on_error"));
      }

      if (config_.has_key("using_readline")) {
#if DATATOOLS_WITH_READLINE == 1
        set_using_readline(config_.fetch_boolean("using_readline"));
#endif // DATATOOLS_WITH_READLINE
      }

      if (config_.has_key("using_splash")) {
        set_using_splash(config_.fetch_boolean("using_splash"));
      }

      if (config_.has_key("using_history")) {
#if DATATOOLS_WITH_READLINE == 1
        set_using_history(config_.fetch_boolean("using_history"));
#endif // DATATOOLS_WITH_READLINE
      }

#if DATATOOLS_WITH_READLINE == 1
      if (_using_history_) {

        if (_history_filename_.empty()) {
          if (config_.has_key("history_filename")) {
            const std::string & hf = config_.fetch_string("history_filename");
            set_history_filename(hf);
          }
        }

        if (config_.has_key("history_add_only_on_success")) {
          set_history_add_only_on_success(config_.fetch_boolean("history_add_only_on_success"));
        }

        if (_history_filename_.empty()) {
          std::ostringstream hist_oss;
          hist_oss << "~/." << _name_ << ".history";
          _history_filename_ = hist_oss.str();
        }

        if (_history_truncate_ == 0) {
          if (config_.has_key("history_truncate")) {
            int hft = config_.fetch_integer("history_truncate");
            DT_THROW_IF(hft < 0, std::domain_error, "Invalid history truncation size!");
            set_history_truncate((unsigned int) hft);
          }
        }

        if (_history_truncate_ == 0) {
          _history_truncate_ = DEFAULT_HISTORY_TRUNCATE;
        }

      }
#endif // DATATOOLS_WITH_READLINE

      // Specific initialization:
      _at_init(config_);

      if (has_system_interface()) {
        if (_logging_ >= datatools::logger::PRIO_NOTICE) {
          _get_system_interface().tree_dump(std::clog, "System builtin command interface: ", "[notice] ");
        }
      }

      // Set the current path:
      set_current_path(get_default_path());

      _initialized_ = true;
      return;
    }

    bool basic_shell::is_stop_requested() const
    {
      return _get_pimpl().stop_requested;
    }

    void basic_shell::set_stop_requested()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Shell is not initialized!");
      _grab_pimpl().stop_requested = true;
      return;
    }

    void basic_shell::_at_init(const datatools::properties & config_)
    {
      DT_THROW_IF(_ihs_ == nullptr, std::logic_error,
                  "Shell has no IHS!");
      if (_default_path_.empty()) {
        _default_path_ = datatools::ui::path::root_path();
      }
      if (_current_working_path_.empty()) {
        _current_working_path_ = _default_path_;
      }

      if (! has_system_interface()) {

        std::string system_interface_id;
        if (config_.has_key("system_interface_id")) {
          system_interface_id = config_.fetch_string("system_interface_id");
        } else {
          // Default to basic shell interface:
          system_interface_id = "datatools::ui::shell_command_interface";
        }

        const base_command_interface::factory_register_type & the_factory_register =
          DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(base_command_interface);
        DT_THROW_IF(!the_factory_register.has(system_interface_id),
                    std::logic_error,
                    "No command interface is registered with type identifier '" << system_interface_id << "'!");
        const base_command_interface::factory_register_type::factory_type & the_factory
          = the_factory_register.get(system_interface_id);
        DT_LOG_NOTICE(_logging_, "Instantiating shell command interface of type '" << system_interface_id << "'...");
        base_command_interface * bci = the_factory();
        shell_command_interface_type * sci = dynamic_cast<shell_command_interface_type *>(bci);
        DT_THROW_IF(sci == nullptr,
                    std::logic_error,
                    "Command interface with type identifier '" << system_interface_id << "' has no shell target!");

        _system_interface_.reset(sci);
        _system_interface_->set_target(*this);
        _system_interface_->set_name(system_interface_name());
        _system_interface_->set_terse_description(std::string("System builtin commands for the '") + get_name() + "' shell");

        datatools::properties sci_config;
        config_.export_and_rename_starting_with(sci_config, "system_interface.", "");
        DT_LOG_NOTICE(_logging_, "Initializing the shell command interface...");
        if (has_services()) {
          _system_interface_->initialize(sci_config, get_services());
        } else {
          _system_interface_->initialize_standalone(sci_config);
        }
      }
      return;
    }

    void basic_shell::_at_reset()
    {
      if (_external_system_interface_ != nullptr) {
        _external_system_interface_ = nullptr;
      }
      if (_system_interface_.get() != nullptr) {
        if (_system_interface_->is_initialized()) {
          _system_interface_->reset();
        }
        _system_interface_.reset();
      }
      return;
    }

    void basic_shell::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Shell is not initialized!");
      _initialized_ = false;
      _at_reset();
      reset_services();
      _set_defaults();
      return;
    }

    bool basic_shell::is_absolute_path(const std::string & path_) const
    {
      if (!_ihs_->exists(path_)) return false;
      return true;
    }

    std::string basic_shell::canonical_path(const std::string & path_) const
    {
      bool devel = false;
      std::string full_path;

      if (path_.empty()) {

        // "" -> "/path/to/default/interface"
        full_path = _default_path_;
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 1" << std::endl;

      } else if (path_ == datatools::ui::path::root_path()) {
        // "/" -> ""
        full_path = path_;
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 2" << std::endl;

      } else if (path_ == datatools::ui::path::home_interface_path()
                 || path_ == (datatools::ui::path::home_interface_path() + datatools::ui::path::sep())) {

        // "~" | "~/" -> "/path/to/default/interface"
        full_path = _default_path_;
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 3" << std::endl;

      } else if (boost::starts_with(path_,
                                    datatools::ui::path::home_interface_path() + datatools::ui::path::sep())) {

        // "~/foo/bar"  -> "/path/to/default/interface/foo/bar"
        // "~/foo/bar/" -> "/path/to/default/interface/foo/bar/"
        full_path = boost::algorithm::replace_first_copy(path_,
                                                         datatools::ui::path::home_interface_path(),
                                                         _default_path_);

        if (devel) std::cerr << "DEVEL: canonical_path: CASE 4" << std::endl;

      } else if (path_ == datatools::ui::path::current_interface_path()
                 || path_ == (datatools::ui::path::current_interface_path() + datatools::ui::path::sep())) {

        // "." | "./" -> "/path/to/cwi"
        full_path = _current_working_path_;
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 5" << std::endl;

      } else if (boost::starts_with(path_,
                                    datatools::ui::path::current_interface_path() + datatools::ui::path::sep())) {

        // "./foo/bar"  -> "/path/to/cwi/foo/bar"
        // "./foo/bar/" -> "/path/to/cwi/foo/bar/"
        full_path = boost::algorithm::replace_first_copy(path_,
                                                         datatools::ui::path::current_interface_path(),
                                                         _current_working_path_);
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 6" << std::endl;

      } else if (path_ == datatools::ui::path::current_parent_interface_path()
                 || path_ == (datatools::ui::path::current_parent_interface_path() + datatools::ui::path::sep())) {

        // std::cerr << "DEVEL: canonical_path: path_      = '" << path_ << "'" << std::endl;

        // ".." | "../" -> "/path/to"
        full_path = datatools::ui::path::parent_path(_current_working_path_);
        // std::cerr << "DEVEL: canonical_path:  full_path = '" << full_path << "'" << std::endl;
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 7" << std::endl;

      } else if (boost::starts_with(path_,
                                    datatools::ui::path::current_parent_interface_path() + datatools::ui::path::sep())) {

        // "../foo/bar"  -> "/path/to/foo/bar"
        // "../foo/bar/" -> "/path/to/foo/bar/"
        full_path = boost::algorithm::replace_first_copy(path_,
                                                         datatools::ui::path::current_parent_interface_path(),
                                                         datatools::ui::path::parent_path(_current_working_path_));
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 8" << std::endl;

      } else if (datatools::ui::path::is_absolute_path(path_)) {

        // "/foo/bar" -> "/foo/bar"
        full_path = path_;
        if (devel) std::cerr << "DEVEL: canonical_path: CASE 9" << std::endl;

      } else {

        if (_current_working_path_ != datatools::ui::path::root_path()) {

          // "foo/bar" -> "/path/to/cwi/foo/bar"
          full_path = _current_working_path_ + datatools::ui::path::sep() + path_;
          if (devel) std::cerr << "DEVEL: canonical_path: CASE 10" << std::endl;

        } else {

          // "foo/bar" -> "/foo/bar"
          full_path = _current_working_path_ + path_;
          if (devel) std::cerr << "DEVEL: canonical_path: CASE 11" << std::endl;

        }
      }

      if (devel) std::cerr << "DEVEL: canonical_path: pre full_path      = '" << full_path << "'" << std::endl;
      if (full_path != datatools::ui::path::root_path()) {
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, full_path, boost::is_any_of("/"));
        std::list<std::string> tokens2;
        for (int itok = 0; itok < (int) tokens.size(); itok++) {
          if (tokens[itok] == datatools::ui::path::current_interface_path()) {
            // ignore token:
          } else if (tokens[itok] == datatools::ui::path::current_parent_interface_path()) {
            if (tokens2.size() > 0) {
              tokens2.pop_back();
            } else {
              // ignore token:
            }
          } else {
            if (!tokens[itok].empty()) {
              tokens2.push_back(tokens[itok]);
            }
          }
          // We test for each iteration of the partial path exists in this shell interface:
          std::string testpath = datatools::ui::path::build_path(tokens2);
          if (devel) std::cerr << "DEVEL: canonical_path: testpath = '" << testpath << "'" << std::endl;
          if (testpath.empty() || !is_absolute_path(testpath)) {
            // DT_THROW(std::logic_error, "Invalid path '" << path_ << "!");
            full_path.clear();
            return full_path;
          }
        }
        std::string expanded_path;
        for (std::list<std::string>::const_iterator itok = tokens2.begin();
             itok != tokens2.end();
             itok++) {
          if (!itok->empty()) {
            expanded_path += '/';
            expanded_path += *itok;
          }
        }
        if (devel) std::cerr << "DEVEL: canonical_path: expanded_path    = '" << expanded_path << "'" << std::endl;
        full_path = expanded_path;
        if (full_path.empty()) {
          full_path = datatools::ui::path::root_path();
        }
      }
      full_path = datatools::ui::path::remove_trailing_sep(full_path);
      if (devel) std::cerr << "DEVEL: canonical_path: final full_path    = '" << full_path << "'" << std::endl;
      return full_path;
    }

    // static
    void basic_shell::make_title_upper(const std::string & title_, std::string & upper_)
    {
      std::ostringstream oss;
      for (std::size_t i = 0; i < title_.length(); i++) {
        oss << (char) toupper(title_[i]);
        if (i != title_.length() - 1) {
          oss << ' ';
        }
      }
      upper_ = oss.str();
      return;
    }

    void basic_shell::_print_splash(std::ostream & out_)
    {
      out_ << "\n";
      std::string upper;
      std::string title = _title_;
      if (title.empty()) {
        title = _name_;
      }
      make_title_upper(title, upper);
      out_ << "\t" << upper << "\n";
      if (has_version()) {
        out_ << "\tVersion " << get_version() << "\n";
      }
      out_ << "\n";
      return;
    }

    void basic_shell::_print_help(std::ostream & out_)
    {
      out_ << "\n";
      out_ << "Sorry, no immediate help is available.\n";
      out_ << "\n";
      return;
    }

    int basic_shell::run(std::istream * in_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(!is_initialized(), std::logic_error, "Shell is not initialized!");
      _at_run_start();
      std::istream * in = in_;
      uint32_t rc_flags = RC_NONE;
      if (in != nullptr) {
        // Force readline/history inhibition:
        rc_flags |= RC_INHIBIT_READLINE;
        rc_flags |= RC_INHIBIT_HISTORY;
      }
      if (is_exit_on_error()) {
        DT_LOG_DEBUG(get_logging(), "Flag RC_EXIT_ON_ERROR");
        rc_flags |= RC_EXIT_ON_ERROR;
      }
      if (!is_using_readline()) {
        rc_flags |= RC_INHIBIT_READLINE;
      }
      if (!is_using_history()) {
        rc_flags |= RC_INHIBIT_HISTORY;
      }
#if DATATOOLS_WITH_READLINE == 0
      rc_flags |= RC_INHIBIT_READLINE;
      rc_flags |= RC_INHIBIT_HISTORY;
#endif // DATATOOLS_WITH_READLINE
      {
        parser_context top_context;
        _grab_pimpl().pcontexts.push_back(top_context);
      }
      datatools::command::returned_info cri = _run_core(in, rc_flags);
      {
        _grab_pimpl().pcontexts.pop_back();
      }
      _at_run_stop();
      DT_LOG_TRACE_EXITING(_logging_);
      return cri.get_error_code();
    }

    void basic_shell::_at_run_start()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_LOG_TRACE(_logging_, "Starting run...");
      if (_using_splash_) {
        _print_splash(std::cout);
      }
#if DATATOOLS_WITH_READLINE == 1
      if (_using_history_) {
        _grab_pimpl().expanded_history_filename = _history_filename_;
        datatools::fetch_path_with_env(_grab_pimpl().expanded_history_filename);
        using_history();
        if (boost::filesystem::exists(_grab_pimpl().expanded_history_filename)) {
          int error = read_history(_grab_pimpl().expanded_history_filename.c_str());
          if (error) {
            _grab_pimpl().expanded_history_filename.clear();
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Shell " << _name_ << ": "
                         << "Cannot read history file '"
                         << _grab_pimpl().expanded_history_filename << "'!");
          }
        }
      }
#endif // DATATOOLS_WITH_READLINE
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void basic_shell::_at_run_stop()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_LOG_TRACE(_logging_, "Stopping run...");
#if DATATOOLS_WITH_READLINE == 1
      if (_using_history_) {
        if (! _grab_pimpl().expanded_history_filename.empty()) {
          int error = write_history(_grab_pimpl().expanded_history_filename.c_str());
          if (error) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Shell " << _name_ << ": "
                         << "Cannot write history file '"
                         << _grab_pimpl().expanded_history_filename << "' !");
          }
          history_truncate_file(_grab_pimpl().expanded_history_filename.c_str(),
                                _history_truncate_);
        }
        clear_history();
      }
#endif // DATATOOLS_WITH_READLINE
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void basic_shell::load_macro(const std::string & macro_, uint32_t flags_)
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      std::string macro = macro_;
      datatools::fetch_path_with_env(macro);
      DT_THROW_IF(!boost::filesystem::exists(macro),
                  std::runtime_error,
                  "File '" << macro << "' does not exist!");
      boost::filesystem::path p(macro);
      DT_THROW_IF(!boost::filesystem::is_regular_file(p),
                  std::runtime_error,
                  "File '" << macro << "' is not a regular file!");
      DT_LOG_TRACE(get_logging(), "Executing macro '" << macro << "...");
      std::ifstream ifmacro;
      ifmacro.open(macro.c_str());
      DT_THROW_IF(!ifmacro,
                  std::runtime_error,
                  "Cannot open macro file '" << macro << "'!");
      uint32_t flags = flags_;
      flags |= RC_INHIBIT_READLINE;
      flags |= RC_INHIBIT_HISTORY;
      {
        parser_context macro_context;
        macro_context.macro_name = macro;
        _grab_pimpl().pcontexts.push_back(macro_context);
      }
      datatools::command::returned_info cri = _run_core(&ifmacro, flags);
      {
        _grab_pimpl().pcontexts.pop_back();
      }
      ifmacro.close();
      DT_THROW_IF(cri.is_failure(),
                  std::runtime_error,
                  "Macro '" << macro << "' failed! " << cri.get_error_message());
      DT_LOG_TRACE_EXITING(get_logging());
      return;
    }

    datatools::command::returned_info basic_shell::_run_core(std::istream * in_, uint32_t flags_)
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      datatools::command::returned_info cri;
      // Run the command line interface loop:
      bool go_on = true;
      std::vector<std::string> to_be_historized;
      bool using_prompt = true;
#if DATATOOLS_WITH_READLINE == 1
      bool using_history = _using_history_;
#endif // DATATOOLS_WITH_READLINE
      bool exit_on_error = false;
      if (flags_ & RC_EXIT_ON_ERROR) {
        DT_LOG_TRACE(_logging_, "exit_on_error = " << exit_on_error);
        exit_on_error = true;
      }
#if DATATOOLS_WITH_READLINE == 1
      if (flags_ & RC_INHIBIT_HISTORY) {
        using_history = false;
      }
#endif // DATATOOLS_WITH_READLINE
      bool using_readline = _using_readline_;
      if (flags_ & RC_INHIBIT_READLINE) {
        using_readline = false;
      }
      // Default input stream:
      std::istream * input = &std::cin;
      // Using an external input stream:
      if (in_ != nullptr && in_ != &std::cin) {
        // Special input stream: inhibit readline and prompt
        using_readline = false;
        using_prompt = false;
        input = in_;
      }
#if DATATOOLS_WITH_READLINE == 1
      if (!using_readline) {
        using_history = false;
      }
#endif // DATATOOLS_WITH_READLINE

      // std::size_t line_counter = 0;
      // Main loop :
      while (go_on) {
        // DT_LOG_TRACE(get_logging(), "Main loop...");
        if (! using_readline) {
          if (! *input || input->eof()) {
            std::cerr << std::flush;
            std::cout << std::endl;
            break;
          }
        }
        std::string line;
        // Read a line:
#if DATATOOLS_WITH_READLINE == 1
        // DT_LOG_TRACE(get_logging(),
        //              "Using readline with effective prompt : '" << get_effective_prompt() << "'");
        if (using_readline) {
          char * readline_line = nullptr;
          go_on = false;
          readline_line = readline(get_effective_prompt().c_str()); // use readline library
          _grab_pimpl().pcontexts.back().line_counter++;
          if (readline_line != nullptr) {
            go_on = true;
            line = readline_line; // use readline library
            if (! line.empty() && using_history) {
              to_be_historized.push_back(readline_line);
            }
            free(readline_line);
            readline_line = nullptr;
          }
        } else
#endif // DATATOOLS_WITH_READLINE
          {
            // Prompt:
            if (using_prompt) {
              std::cerr << get_effective_prompt() << std::flush;
            }
            std::getline(*input, line);
            _grab_pimpl().pcontexts.back().line_counter++;
            if (! *input || input->eof()) {
              go_on = false;
            }
          } // End of read line

        {
          // Skip blank line and lines starting with '#' :
          std::istringstream dummy(line);
          std::string word;
          dummy >> word;
          if (word.empty()) continue;
          if (word[0] == '#') continue;
        }

        // Manage continuation marks :
        if (go_on) {
          while (line[line.length()-1] == '\\') {
            line = line.substr(0, line.length()-1);
            std::string more;
#if DATATOOLS_WITH_READLINE == 1
            if (using_readline)
              {
                char * readline_line = nullptr;
                go_on = false;
                readline_line = readline(get_effective_continuation_prompt().c_str()); // use readline library
                _grab_pimpl().pcontexts.back().line_counter++;
                if (readline_line != nullptr) {
                  go_on = true;
                  more = readline_line; // use readline library
                  if (! more.empty() && using_history) {
                    to_be_historized.push_back(readline_line);
                  }
                  free(readline_line);
                  readline_line = nullptr;
                }
              }
            else
#endif // DATATOOLS_WITH_READLINE
              {
                if (using_prompt) {
                  std::cerr << get_effective_continuation_prompt() << std::flush;
                }
                std::getline(*input, more);
                _grab_pimpl().pcontexts.back().line_counter++;
                if (! *input || input->eof()) {
                  go_on = false;
                }
              }
            // Append to the current line:
            line += more;
          }
        } // End of continuation marks management.

        // Interpreter block:
#if DATATOOLS_WITH_READLINE == 1
        bool success = true;
#endif // DATATOOLS_WITH_READLINE
        {
          cri.set_success();
          cri = _run_command(line);
          DT_LOG_TRACE(get_logging(),
                       "Command returned with error code [" << cri.get_error_code() << "]");
          if (cri.is_stop()) {
            DT_LOG_TRACE(get_logging(), "Command requests a shell stop.");
            go_on = false;
          } else if (cri.is_failure()) {
#if DATATOOLS_WITH_READLINE == 1
            success = false;
#endif // DATATOOLS_WITH_READLINE
            DT_LOG_TRACE(get_logging(), "Command failed.");
            std::cerr << get_name() << ": error: ";
            const parser_context & pcontext = _grab_pimpl().pcontexts.back();
            if (!pcontext.macro_name.empty()) {
              std::cerr << "in macro '" << pcontext.macro_name << "'";
            }
            std::cerr << "at line [#" << pcontext.line_counter << "]: ";
            if (cri.has_error_message()) {
              std::cerr << cri.get_error_message();
            } else {
              std::cerr << "Command failed !";
            }
            // std::cerr << " CRI code = [" << cri.get_error_code() << "] ";
            // std::cerr << " Exit-on-error = [" << exit_on_error << "] ";
            if (exit_on_error || cri.get_error_code() == datatools::command::CEC_ABORT) {
              // Special kind of error occured: we abort the shell:
              go_on = false;
              std::cerr << " Abort!";
            }
            std::cerr << std::endl;
          } else {
            DT_LOG_TRACE(get_logging(), "Command was successful.");
          }
        } // End of interpreter block.

#if DATATOOLS_WITH_READLINE == 1
        // Use readline/history library:
        if (using_history) {
          bool historize_it = true;
          if (_history_add_only_on_success_ && ! success) {
            historize_it = false;
          }
          if (historize_it) {
            for (int iline = 0; iline < (int) to_be_historized.size(); iline++) {
              const std::string & local_line = to_be_historized[iline];
              DT_LOG_TRACE(get_logging(), "Adding history '" << local_line << "'...");
              add_history(local_line.c_str());
            }
          }
          to_be_historized.clear();
        }
#endif // DATATOOLS_WITH_READLINE

        if (! using_readline && input->eof()) {
          DT_LOG_TRACE(get_logging(), "EOF.");
          go_on = false;
          break;
        }
        if (_grab_pimpl().stop_requested) {
          DT_LOG_TRACE(get_logging(), "Stop is requested.");
          go_on = false;
        }

        if (go_on) {
          go_on = _compute_continue_condition();
        }

        DT_LOG_DEBUG(get_logging(), "go_on = " << go_on);
        if (! go_on) {
          break;
        }

      } // End of loop.

      DT_LOG_TRACE_EXITING(get_logging());
      return cri;
    }

    bool basic_shell::_compute_continue_condition()
    {
      return true;
    }

    datatools::command::returned_info basic_shell::_run_command(const std::string & command_line_)
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      datatools::command::returned_info cri;
      DT_LOG_TRACE(get_logging(), "Line is '" << command_line_ << "'");

      // Prepare the command line as an Unix-like array of options/arguments:
      std::vector<std::string> argv = boost::program_options::split_unix(command_line_);

      if (argv.size()) {
        DT_LOG_TRACE(get_logging(), "Line is not empty.");
        // Fetch the command name:
        std::string cmd_name = argv[0];

        bool call_done = false;

        // First try to call system/builtin commands (cd/ls/pwd...) from the system interface:
        if (! call_done) {
          if (has_system_interface()) {
            DT_LOG_TRACE(get_logging(), "Trying system interface...");
            if (_get_system_interface().has_command(cmd_name)) {
              call_done = true;
              _grab_system_interface().call(argv, cri);
            } else {
              DT_LOG_TRACE(get_logging(), "Not a system interface command!");
            }
          } else {
            DT_LOG_TRACE(get_logging(), "No system interface is available!");
          }
        }

        // Next try some interface from the IHS:
        if (! call_done) {
          if (has_ihs()) {
            DT_LOG_TRACE(get_logging(), "Trying IHS...");
            // Expand the full path of the command:
            std::string cmd_full_path = this->canonical_path(cmd_name);
            DT_LOG_TRACE(get_logging(), "cmd_full_path = '" << cmd_full_path << "'");
            // Check if the path correspond to an IHS command:
            if (_ihs_->is_command(cmd_full_path)) {
              if (_ihs_->is_trait(cmd_full_path,datatools::ui::traits::disabled_label())) {
                call_done = true;
                DT_COMMAND_RETURNED_ERROR(cri,
                                          datatools::command::CEC_COMMAND_INVALID_CONTEXT,
                                          "Disabled command '" << cmd_name << "'!");
              } else {
                if (_ihs_->is_trait(cmd_full_path,datatools::ui::traits::broken_label())) {
                  // Attempt to run a broken command...
                  std::cerr << get_name() << ": warning: "
                            << "Attempt to run the broken command '" << cmd_name << "'!" << std::endl;
                }
                DT_LOG_TRACE(get_logging(), "Found IHS command = '" << cmd_full_path << "'");
                // Then call the command with its vector of options/argument:
                std::vector<std::string> cmd_argv = argv;
                cmd_argv.erase(cmd_argv.begin());
                base_command & cmd = _ihs_->grab_command(cmd_full_path);
                call_done = true;
                cmd.call(cmd_argv, cri);
              }
            } else {
              DT_LOG_TRACE(get_logging(), "Not a IHS command!");
            }
          } else {
            DT_LOG_TRACE(get_logging(), "No IHS is available!");
          }
        }

        if (! call_done) {
          // Finaly, we found no matching command:
          DT_COMMAND_RETURNED_ERROR(cri,
                                    datatools::command::CEC_COMMAND_INVALID,
                                    "Invalid command '" << cmd_name << "'!");
          DT_LOG_TRACE(get_logging(), "Could not found command '" << cmd_name << "'!");
        }

      } else {
        // Blank line means the shell should continue:
        cri.set_continue();
        DT_LOG_TRACE(get_logging(), "Shell continues...");
      }

      DT_LOG_TRACE_EXITING(get_logging());
      return cri;
    }

    void basic_shell::print_tree(std::ostream & out_,
                                 const boost::property_tree::ptree & options_) const
    {
      datatools::i_tree_dumpable::base_print_options popts;
      popts.configure_from(options_);
      bool print_ihs = options_.get<bool>("ihs", false);

      if (!popts.title.empty()) out_ << popts.indent << popts.title << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Logging  : '" << datatools::logger::get_priority_label(_logging_) << "'" << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Name     : '" << _name_ << "'" << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Title    : '" << _title_ << "'" << std::endl;

      if (has_version()) {
        out_ << popts.indent << i_tree_dumpable::tag
             << "Version  : '" << get_version() << "'" << std::endl;
      }

      out_ << popts.indent << i_tree_dumpable::tag
           << "Prompt   : '" << _prompt_ << "'"  << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Continuation prompt : '" << _continuation_prompt_ << "'"  << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Default path  : '" << _default_path_ << "'"  << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "User           : '" << _user_ << "'"  << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Host           : '" << _host_ << "'"  << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Exit on error  : " << std::boolalpha << _exit_on_error_ << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Using splash   : " << std::boolalpha << _using_splash_ << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Using readline : " << std::boolalpha << _using_readline_ << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Using history  : " << std::boolalpha << _using_history_ << std::endl;

      if (_using_history_) {

        out_ << popts.indent << i_tree_dumpable::tag
             << "History add only on success : " << std::boolalpha << _history_add_only_on_success_ << std::endl;

        out_ << popts.indent << i_tree_dumpable::skip_tag << i_tree_dumpable::tag
             << "History filename : '" << _history_filename_  << "'" << std::endl;

        out_ << popts.indent << i_tree_dumpable::skip_tag << i_tree_dumpable::last_tag
             << "History truncate : [#" << _history_truncate_ << ']' << std::endl;

      }

      out_ << popts.indent << i_tree_dumpable::tag << "System interface : ";
      if (has_system_interface()) {
        out_ << "'" << _get_system_interface().get_name() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      if (has_system_interface()) {
        std::ostringstream indent2;
        indent2 << popts.indent << i_tree_dumpable::skip_tag;
        _get_system_interface().tree_dump(out_, "", indent2.str());
      }

      out_ << popts.indent << i_tree_dumpable::tag
           << "IHS : " << "[@" << _ihs_ << "]" << std::endl;
      if (print_ihs) {
        if (has_ihs()) {
          std::ostringstream indent2;
          indent2 << popts.indent << i_tree_dumpable::skip_tag;
          _ihs_->tree_dump(out_, "", indent2.str());
        }
      }

      out_ << popts.indent << i_tree_dumpable::tag
           << "Current working path : '" << _current_working_path_ << "'"  << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Services       : [@" << _services_ << ']' << std::endl;

      out_ << popts.indent << i_tree_dumpable::tag
           << "Stop requested : " << std::boolalpha << _get_pimpl().stop_requested << std::endl;

      out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
           << "Initialized    : " << std::boolalpha << _initialized_ << std::endl;

      return;
    }

  } // namespace ui

} // namespace datatools
