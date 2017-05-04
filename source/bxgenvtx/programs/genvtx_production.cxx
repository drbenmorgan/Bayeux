// genvtx_production.cxx

// Ourselves:
#include <genvtx/genvtx_config.h>

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third Party:
// - Boost:
#include <boost/program_options.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/configuration/io.h>
// - Bayeux/geomtools:
#include <geomtools/color.h>
// - Bayeux/genvtx:
#include <genvtx/genvtx_driver.h>

namespace genvtx {

  /// \brief Command line interface class
  struct cli : public genvtx_driver::ui_access {

    /// Constructor:
    cli(genvtx_driver &);

    static void print_help(boost::program_options::options_description &,
                           std::ostream & out_ = std::clog);

    static void build_general_opts(boost::program_options::options_description &,
                                   genvtx_driver_params &);

    static void build_initialization_opts(boost::program_options::options_description &,
                                          genvtx_driver_params &);

    static void build_action_opts(boost::program_options::options_description &,
                                  genvtx_driver_params &);

    // Command line interface:

    int command_initialize(const std::vector<std::string> & argv_);

    // int command_run(const std::vector<std::string> & argv_);

    // int command_reset(const std::vector<std::string> & argv_);

  };

} // namespace genvtx

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);

  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  namespace po = boost::program_options;
  boost::program_options::options_description opts("General options");
  try {
    bool run_session = true;
    genvtx::genvtx_driver_params params;

    // Preprocessor for command line arguments:
    unsigned int vpp_flags = 0;
    // vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_TRACE;
    vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_REMOVE_QUOTES;
    datatools::configuration::variant_preprocessor vpp(vpp_flags);
    std::vector<std::string> preprocessed_arguments;
    vpp.preprocess_args_options(argc_, argv_, preprocessed_arguments);

    genvtx::cli::build_general_opts(opts, params);
    po::positional_options_description args;
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(preprocessed_arguments)
      .options(opts)
      .allow_unregistered()
      .run();

    // Collect all other options & args for the driver session:
    std::vector<std::string> GD_argv;
    GD_argv = po::collect_unrecognized(parsed.options,
                                       po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    logging = datatools::logger::get_priority(params.logging_label);
    if (params.help) {
      genvtx::cli::print_help(opts, std::cout);
      run_session = false;
    }

    if (run_session) {
      // The genvtx driver :
      genvtx::genvtx_driver GD;

      // The command line interface:
      genvtx::cli CLI(GD);

      // Force logging options:
      GD_argv.push_back("--logging");
      GD_argv.push_back(params.logging_label);

      // Initialize the driver:
      int code = CLI.command_initialize(GD_argv);
      DT_THROW_IF(code != EXIT_SUCCESS,
                  std::logic_error,
                  "Driver initialization failed !");

      // Run a one-shot session:
      GD.run();

      // Terminate the driver:
      GD.reset();
    }

    // The end.
  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

// Implementation:
namespace genvtx {

  cli::cli(genvtx_driver & driver_) : genvtx_driver::ui_access(driver_)
  {
    return;
  }

  void cli::print_help(boost::program_options::options_description & opts_,
                       std::ostream & out_)
  {
    boost::program_options::options_description driver_init_opts;
    boost::program_options::options_description driver_action_opts;
    boost::program_options::options_description driver_all_opts;
    genvtx::genvtx_driver_params driver_params;
    build_initialization_opts(driver_init_opts, driver_params);
    build_action_opts(driver_action_opts, driver_params);
    driver_all_opts
      .add(opts_)
      .add(driver_init_opts)
      .add(driver_action_opts);

    const std::string APP_NAME = "bxgenvtx_production";

    out_ << "Usage : " << APP_NAME << " [OPTION]...                                      \n";
    out_ << "                                                                            \n";
    out_ << "  Inspect and generate vertices from a vertex generator                     \n";
    out_ << "  managed by a vertex generator manager associated to a                     \n";
    out_ << "  geometry manager.                                                         \n";
    out_ << "                                                                            \n";
    out_ << driver_all_opts << std::endl;
    out_ << "                                                                            \n";
    out_ << "Example:                                                                    \n";
    out_ << "                                                                            \n";
    out_ << "  List the available vertex generators:                                     \n";
    out_ << "                                                                            \n";
    out_ << "     " << APP_NAME << " \\                                                   \n";
    out_ << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
    out_ << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
    out_ << "       --list                                                               \n";
    out_ << "                                                                            \n";
    out_ << "  Show a given vertex generator:                                            \n";
    out_ << "                                                                            \n";
    out_ << "     " << APP_NAME << " \\                                                   \n";
    out_ << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
    out_ << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
    out_ << "       --show --vertex-generator \"scintillator_block.vg\"                  \n";
    out_ << "                                                                            \n";
    out_ << "  Generate some vertices, store them in a file, display them and            \n";
    out_ << "  serialize a display data object:                                          \n";
    out_ << "                                                                            \n";
    out_ << "     " << APP_NAME << " \\                                                   \n";
    out_ << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
    out_ << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
    out_ << "       --shoot \\                                                           \n";
    out_ << "       --prng-seed 314159 \\                                                \n";
    out_ << "       --number-of-vertices 10000 \\                                        \n";
    out_ << "       --vertex-generator \"scintillator_block.vg\" \\                      \n";
    out_ << "       --output-file \"vertices.data\"    \\                                \n";
    out_ << "       --visu \\                                                            \n";
    out_ << "       --visu-output-file \"vertices-visu-dd.data.gz\"                      \n";
    out_ << "                                                                            \n";
    out_ << "                                                                            \n";
    return;
  }


  void cli::build_general_opts(boost::program_options::options_description & opts_,
                               genvtx_driver_params & params_)
  {
    namespace po = boost::program_options;
    opts_.add_options()
      ("help,h",
       po::value<bool>(&params_.help)
       ->zero_tokens()
       //->default_value(false)
       ,
       "Print help then exit.   \n"
       "Example :               \n"
       "  --help                  "
       )
      ("logging",
       po::value<std::string>(&params_.logging_label)
       //->default_value("warning")
       ,
       "Set the logging priority threshold.\n"
       "Example :                          \n"
       "  --logging \"warning\"              "
       )
      ;
    return;
  }

  void cli::build_initialization_opts(boost::program_options::options_description & opts_,
                                      genvtx_driver_params & params_)
  {
    namespace po = boost::program_options;
    opts_.add_options()
      ("dll-config",
       po::value<std::string>(&params_.LL_config),
       "Set the dynamic library loader configuration file.           \n"
       "Example :                          \n"
       "  --dll-config \"dlls.lis\"          "
       )
      ("load-dll",
       po::value<std::vector<std::string> >(&params_.LL_dlls),
       "Load a given dynamic library (DLL).      \n"
       "Example :                                \n"
       "  --load-dll \"foo\" --load-dll \"bar\"    "
       )
      ("geometry-manager",
       po::value<std::string>(&params_.GeoMgrConfigFile),
       "Set the geometry manager configuration file.\n"
       "Example :                                   \n"
       "  --geometry-manager \"geo_mgr.conf\"         "
       )
      ("vertex-generator-manager",
       po::value<std::string>(&params_.VGMgrConfigFile),
       "Set the vertex generator manager configuration file.\n"
       "Example :                                           \n"
       "  --vertex-generator-manager \"vg_mgr.conf\"          "
       )
      ;
    return;
  }


  void cli::build_action_opts(boost::program_options::options_description & opts_,
                              genvtx_driver_params & params_)
  {
    namespace po = boost::program_options;
    opts_.add_options()
      ("list,l",
       po::value<bool>(&params_.action_list)
       ->zero_tokens()
       //->default_value(false)
       ,
       "List the available vertex generators defined\n"
       "in the vertex generator manager.   \n"
       "Example :                          \n"
       "  --list                             "
       )
      ("vertex-generator,g",
       po::value<std::string>(&params_.generator_name),
       "Set the name of the vertex generator to be     \n"
       "shown or shot.                                 \n"
       "Example :                                      \n"
       "  --vertex-generator \"calib_source_bulk.conf\"  "
       )
      ("show,w",
       po::value<bool>(&params_.action_show)
       ->zero_tokens()
       //->default_value(false)
       ,
       "Print a vertex generator defined\n"
       "in the vertex generator manager.   \n"
       "Example :                          \n"
       "  --show                           \n"
       "  --vertex-generator \"calib_source_bulk.conf\" \n"
       )
      ("shoot,s",
       po::value<bool>(&params_.action_shoot)
       ->zero_tokens()
       //->default_value(false)
       ,
       "Generate random vertices from a given \n"
       "vertex generator defined in the       \n"
       "vertex generator manager.             \n"
       "Example :                             \n"
       "  --shoot                             \n"
       "  --vertex-generator \"calib_source_bulk.conf\" "
       )
      ("prng-type",
       po::value<std::string>(&params_.prng_type)
       //->default_value("taus2")
       ,
       "Set the type of the random number generator. \n"
       "Example :                                    \n"
       "  --prng-type \"taus2\"                        "
       )
      ("prng-seed",
       po::value<int>(&params_.prng_seed),
       "Set the seed for the random number generator. \n"
       "Example :                                     \n"
       "  --prng-seed 314159                            "
       )
      ("number-of-vertices,n",
       po::value<int>(&params_.nshoots),
       "Set the number of vertices to be generated.\n"
       "Example :                                  \n"
       "  --number-of-vertices 1000                  "
       )
      ("vertex-modulo,m",
       po::value<int>(&params_.shoot_modulo),
       "Set the modulo on number of generated vertices.\n"
       "Example :                                      \n"
       "  --vertex-modulo 100                            "
       )
      ("output-file,o",
       po::value<std::string>(&params_.VtxOutputFile),
       "Set the output file name where to store  \n"
       "generated vertices.                      \n"
       "Example :                                \n"
       "  --output-file \"vertexes.data\"          "
       )
      ("visu",
       po::value<bool>(&params_.action_visu)
       ->zero_tokens()
       //->default_value(false)
       ,
       "Activate the visualization of randomly \n"
       "generated vertices.                    \n"
       "Example :                              \n"
       "  --visu                                 "
       )
      ("visu-max-counts",
       po::value<int>(&params_.visu_max_counts)
       //->default_value(10000)
       ,
       "Set the maximum number of randomly  \n"
       "generated vertices to be displayed. \n"
       "Example :                           \n"
       "  --visu-max-counts 100               "
       )
      ("visu-spot-zoom",
       po::value<double>(&params_.visu_spot_zoom)
       //->default_value(1.0)
       ,
       "Set the zoom factor of the randomly  \n"
       "generated vertices to be displayed.  \n"
       "Example :                            \n"
       "  --visu-spot-zoom 3.0                 "
       )
      ("visu-spot-size",
       po::value<std::string>()
       ,
       "Set the size (in mm) of the randomly \n"
       "generated vertices to be displayed.  \n"
       "Example :                            \n"
       "  --visu-spot-size \"3.0 mm\"          "
       )
      ("visu-spot-color",
       po::value<std::string>(&params_.visu_spot_color)
       ->default_value(geomtools::color::magenta())
       ,
       "Set the color of the randomly \n"
       "generated vertices to be displayed.  \n"
       "Example :                            \n"
       "  --visu-spot-color \"magenta\"        "
       )
      ("visu-output",
       po::value<bool>(&params_.action_visu_store_dd)
       ->zero_tokens()
       //->default_value(false)
       ,
       "Activate display data storage.       \n"
       "Example :                            \n"
       "  --visu-output                        "
       )
      ("visu-output-file",
       po::value<std::string>(&params_.visu_store_dd_out),
       "Set the output file name where to store  \n"
       "generated vertices converted in some     \n"
       "display data object.                     \n"
       "Example :                                \n"
       "  --visu-output-file \"vertexes-dd.data\" "
       )
      ;
    return;
  }

  int cli::command_initialize(const std::vector<std::string> & argv_)
  {
    namespace po = boost::program_options;
    po::options_description drv_general_opts("Driver general options");
    po::options_description drv_init_opts("Driver initialization options");
    po::options_description drv_action_opts("Driver action options");
    boost::program_options::options_description drv_all_opts;

    try {
      build_general_opts(drv_general_opts, this->params());
      build_initialization_opts(drv_init_opts, this->params());
      build_action_opts(drv_action_opts, this->params());
      drv_all_opts
        .add(drv_general_opts)
        .add(drv_init_opts)
        .add(drv_action_opts);
      po::positional_options_description args;
      po::variables_map vm;
      po::parsed_options parsed =
        po::command_line_parser(argv_)
        .options(drv_all_opts)
        .allow_unregistered()
        .run();
      std::vector<std::string> unrecognized
        = po::collect_unrecognized(parsed.options,
                                   po::include_positional);
      po::store(parsed, vm);
      po::notify(vm);

      if (vm.count("visu-spot-size")) {
        std::string spot_size_str = vm["visu-spot-size"].as<std::string>();
        std::istringstream iss(spot_size_str);
        double spot_size;
        std::string unit_symbol;
        std::string unit_label;
        DT_THROW_IF(!datatools::units::parse_value_with_unit(spot_size_str,
                                                             spot_size,
                                                             unit_symbol,
                                                             unit_label),
                    std::logic_error,
                    "Invalid format for vertex spot size!");
        DT_THROW_IF(unit_label != "length",  std::logic_error,
                    "Not a length unit for vertex spot size!");
        this->params().visu_spot_size = spot_size;
      }

      if (this->params().help) {
        std::clog << "Usage:" << std::endl;
        std::clog << "  initialize [OPTIONS]" << std::endl;
        std::clog << drv_all_opts << std::endl;
        return EXIT_SUCCESS;
      }

      driver().initialize();
    }
    catch (std::exception& error) {
      DT_LOG_ERROR(driver().get_logging(), error.what());
      return EXIT_FAILURE;
    }
    catch (...) {
      DT_LOG_ERROR(driver().get_logging(), "Unexpected error !");
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  /*
  int cli::command_reset(const std::vector<std::string> & argv_)
  {
    namespace po = boost::program_options;
    po::options_description drv_general_opts("Driver general options");
    try {
      build_general_opts(drv_general_opts, this->params());
      po::positional_options_description args;
      po::variables_map vm;
      po::parsed_options parsed =
        po::command_line_parser(argv_)
        .options(drv_general_opts)
        .allow_unregistered()
        .run();
      std::vector<std::string> unrecognized
        = po::collect_unrecognized(parsed.options,
                                   po::include_positional);
      po::store(parsed, vm);
      po::notify(vm);

      if (this->params().help) {
        std::clog << "Usage:" << std::endl;
        std::clog << "  reset [OPTIONS]" << std::endl;
        std::clog << drv_general_opts << std::endl;
        return EXIT_SUCCESS;
      }

      driver().reset();
    }
    catch (std::exception& error) {
      DT_LOG_ERROR(driver().get_logging(), error.what());
      return EXIT_FAILURE;
    }
    catch (...) {
      DT_LOG_ERROR(driver().get_logging(), "Unexpected error !");
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
  */

  /*
  int cli::command_run(const std::vector<std::string> & argv_)
  {
    namespace po = boost::program_options;
    po::options_description drv_general_opts("Driver general options");
    po::options_description drv_action_opts("Driver action options");
    po::options_description drv_all_opts;
    try {
      build_general_opts(drv_general_opts, this->params());
      build_action_opts(drv_action_opts, this->params());
      drv_all_opts
        .add(drv_general_opts)
        .add(drv_action_opts);
      po::positional_options_description args;
      po::variables_map vm;
      po::parsed_options parsed =
        po::command_line_parser(argv_)
        .options(drv_all_opts)
        .allow_unregistered()
        .run();
      std::vector<std::string> unrecognized
        = po::collect_unrecognized(parsed.options,
                                   po::include_positional);
      po::store(parsed, vm);
      po::notify(vm);

      if (this->params().help) {
        std::clog << "Usage:" << std::endl;
        std::clog << "  run [OPTIONS]" << std::endl;
        std::clog << drv_all_opts << std::endl;
        return EXIT_SUCCESS;
      }

      std::
      driver().run();
    }
    catch (std::exception & error) {
      DT_LOG_ERROR(driver().get_logging(), error.what());
      return EXIT_FAILURE;
    }
    catch (...) {
      DT_LOG_ERROR(driver().get_logging(), "Unexpected error !");
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
*/

} // namespace genvtx