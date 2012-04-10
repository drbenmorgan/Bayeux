// -*- mode: c++; -*-
/* sensitive.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-13
 * Last modified: 2010-03-13
 *
 * License:
 *
 * Description:
 *
 *  Utilities for sensitive reference.
 *
 * History:
 *
 */

#ifndef __geomtools__sensitive_h
#define __geomtools__sensitive_h 1

#include <string>

#include <datatools/utils/properties.h>

namespace geomtools {

  class sensitive
  {
  public:
  
    struct constants
    {
      std::string SENSITIVE_PREFIX;
      std::string SENSITIVE_CATEGORY_PROPERTY;
      std::string SENSITIVE_RECORD_ALPHA_QUENCHING_FLAG;
      std::string SENSITIVE_RECORD_TRACK_ID_FLAG;
      std::string SENSITIVE_RECORD_PRIMARY_PARTICLE_FLAG;
      
      constants ();
      
      static const constants & instance ();

    };

    static std::string make_key (const std::string & flag_);

    static void extract (const datatools::utils::properties & source_,
                         datatools::utils::properties & target_);

    static bool has_flag (const datatools::utils::properties & config_,
                          const std::string & flag_);

    static bool has_key (const datatools::utils::properties & config_,
                         const std::string & key_);

    static bool is_sensitive (const datatools::utils::properties & config_);

    static std::string get_sensitive_category (const datatools::utils::properties & config_);

    static void set_sensitive_category (datatools::utils::properties & config_,
                                        const std::string & cat_name_);

    static bool recording_alpha_quenching (const datatools::utils::properties & config_);

    static bool recording_track_id (const datatools::utils::properties & config_);

    static bool recording_primary_particle (const datatools::utils::properties & config_);

  };


}  // end of namespace geomtools

#endif // __geomtools__sensitive_h

// end of sensitive.h
