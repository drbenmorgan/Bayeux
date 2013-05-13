// -*- mode: c++ ; -*-
/* nand_cut.cc
 */

#include <cuts/nand_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>

namespace cuts {

  using namespace std;

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(nand_cut, "cuts::nand_cut");

  // ctor:
  nand_cut::nand_cut (int a_debug_level)
    : i_binary_cut ("cuts::nand_cut",
                    "Nand cut",
                    "1.0",
                    a_debug_level)
  {
    return;
  }

  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (nand_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(nand_cut)
  {
    int status_1 = _handle_1.grab ().process ();
    int status_2 = _handle_2.grab ().process ();
    if ((status_1 < 0) || (status_2 < 0))
      {
        return INAPPLICABLE;
      }
    if ((status_1 + status_2) == 2)
      {
        return (REJECTED);
      }
    return (ACCEPTED);
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(nand_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::nand_cut::initialize: "
                << "Cut '" << get_name () << "' is already initialized ! ";
        throw logic_error (message.str ());
      }

    this->i_binary_cut::_install_cuts (a_configuration,a_cut_dict);

    _set_initialized (true);
    return;
  }

} // end of namespace cuts

// end of nand_cut.cc
