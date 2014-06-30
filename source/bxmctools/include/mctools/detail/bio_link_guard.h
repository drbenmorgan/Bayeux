/// \file mctools/detail/bio_link_guard.h

#ifndef MCTOOLS_DETAIL_BIO_LINK_GUARD_H
#define MCTOOLS_DETAIL_BIO_LINK_GUARD_H

namespace mctools {
/// Nested private namespace of the Bayeux/mctools module library (detail)
namespace detail {
/// Nested private namespace of the Bayeux/mctools module library (serialization)
namespace serialization {

/** \brief Data structure to ensure that a given executable is linked
 *        against the mctools_bio DLL with new gcc 4.6 linking strategy
 *        even if no explicit mctools serialization code is explicitely
 *        invoked from the executable.
 */
class dynamic_link_guard {
 public:
  dynamic_link_guard();
  ~dynamic_link_guard();

  static dynamic_link_guard& instance();
 private:
  static bool _g_devel_;
};

} // end namespace serialization
} // end namespace detail
} // end namespace mctools

#endif // MCTOOLS_DETAIL_BIO_LINK_GUARD_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
