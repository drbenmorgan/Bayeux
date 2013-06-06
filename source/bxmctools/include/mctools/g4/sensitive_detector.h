// -*- mode: c++ ; -*-
/* sensitive_detector.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   GEANT4 sensitive detector implementation
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_SENSITIVE_DETECTOR_H_
#define MCTOOLS_G4_SENSITIVE_DETECTOR_H_ 1

#include <string>
#include <map>
#include <vector>
#include <list>

#include <boost/cstdint.hpp>

#include <datatools/properties.h>

#include <mctools/base_step_hit_processor.h>
#include <mctools/g4/sensitive_hit.h>
#include <mctools/g4/sensitive_hit_collection.h>
#include <mctools/g4/track_history.h>

// G4 stuff:
#include <G4VSensitiveDetector.hh>

#include <mctools/g4/loggable_support.h>

class G4Step;

namespace mctools {

  namespace g4 {

    class manager;

    class sensitive_detector : public G4VSensitiveDetector,
                               public loggable_support
    {
    public:

      typedef std::map<std::string, mctools::base_step_hit_processor *> hit_processor_dict_type;

      static const size_t DEFAULT_HIT_BUFFER_CAPACITY;

    public:

      bool has_manager () const;
      void set_manager (manager & mgr_);

      void set_flag_delta_ray_from_alpha (bool);
      void set_record_primary_particle   (bool);
      void set_record_track_id           (bool);
      void set_record_major_track        (bool);
      void set_record_creator_process    (bool);
      void set_record_creator_category   (bool);
      void set_record_momentum           (bool);
      void set_record_kinetic_energy     (bool);
      void set_record_material           (bool);
      void set_record_sensitive_category (bool);

      void set_major_track_minimum_energy (double);

      size_t get_hits_buffer_capacity () const;
      void set_hits_buffer_capacity (size_t);

      void set_track_gamma   (bool);
      void set_track_neutron (bool);

      const datatools::properties & get_auxiliaries () const;
      datatools::properties & grab_auxiliaries ();

      int get_HCID () const;
      void set_HCID (int);

      bool is_drop_zero_energy_deposit_steps () const;
      void set_drop_zero_energy_deposit_steps (bool);

      bool is_store_g4_volume_properties () const;
      void set_store_g4_volume_properties (bool);

      void attach_logical_volume (const std::string & log_volume_name_);

      const std::list<std::string> & get_attached_logical_volumes () const;
      std::list<std::string> & grab_attached_logical_volumes ();
      const std::string & get_sensitive_category () const;

      hit_processor_dict_type & grab_hit_processors ();
      const hit_processor_dict_type & get_hit_processors () const;
      bool has_hit_processor (const std::string & name_) const;
      void add_hit_processor (const std::string & name_,
                              mctools::base_step_hit_processor & shp_);
      mctools::base_step_hit_processor & grab_hit_processor (const std::string & name_);

      static std::string make_hit_collection_name (const std::string & sensitive_category_);

      /// Constructor
      sensitive_detector (const std::string & name_);

      /// Desctructor
      virtual ~sensitive_detector ();

      void configure (const datatools::properties & config_);

      virtual void clear ();

      virtual void tree_dump (std::ostream      & out_    = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;

      // Geant4 interface :
      virtual void Initialize (G4HCofThisEvent *);

      virtual void EndOfEvent (G4HCofThisEvent *);

      virtual G4bool ProcessHits (G4Step *, G4TouchableHistory *);

    protected:

      void _set_defaults ();

    private:

      std::string            _sensitive_category_;
      std::list<std::string> _attached_logical_volumes_;
      bool                   _store_g4_volume_properties_;
      bool                   _drop_zero_energy_deposit_steps_;
      bool                   _record_momentum_;
      bool                   _record_kinetic_energy_;

      bool                   _record_primary_particle_;
      bool                   _record_track_id_;
      bool                   _record_major_track_;
      bool                   _record_creator_process_;
      bool                   _record_creator_category_;
      bool                   _record_material_;
      bool                   _record_sensitive_category_;
      double                 _major_track_minimum_energy_;
      bool                   _track_gamma_;
      bool                   _track_neutron_;
      bool                   _flag_delta_ray_from_alpha_;
      size_t                 _hits_buffer_capacity_;

      const track_history::track_info * _track_info_ptr_;
      const track_history::track_info * _parent_track_info_ptr_;

      manager * _manager_;

      datatools::properties _aux_;

      int                         _HCID_;
      std::vector<sensitive_hit>  _hits_buffer_;
      sensitive_hit_collection  * _hits_collection_;
      int                         _used_hits_count_;

      // Dictionnary to attach some processors for step hits:
      hit_processor_dict_type _hit_processors_;

      // Tools :
      uint32_t _number_of_sensitive_steps_;

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_SENSITIVE_DETECTOR_H_

// end of sensitive_detector.h
