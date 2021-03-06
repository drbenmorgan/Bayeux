//! \file datatools/event_id.ipp
#ifndef DATATOOLS_EVENT_ID_IPP
#define DATATOOLS_EVENT_ID_IPP

// Ourselves:
#include <datatools/event_id.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>

// This project:
#include <datatools/utils.h>

namespace datatools {

  /// Boost serialization template method
  template<class Archive>
  void event_id::serialize(Archive & archive, const unsigned int version)
  {
    if (version > 0) {
      archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    }
    archive & boost::serialization::make_nvp("run_number", _run_number_);
    archive & boost::serialization::make_nvp("event_number", _event_number_);
    return;
  }

} // end of namespace datatools

#endif // DATATOOLS_EVENT_ID_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
