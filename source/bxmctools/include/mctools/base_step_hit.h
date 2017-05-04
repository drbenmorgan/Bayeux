/// \file mctools/base_step_hit.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-06-02
 *
 * License:
 *
 * Description:
 *
 *   Base step hit within a sensitive detector
 *
 * History:
 *
 */

#ifndef MCTOOLS_BASE_STEP_HIT_H
#define MCTOOLS_BASE_STEP_HIT_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools :
#include <datatools/bit_mask.h>
// - Bayeux/geomtools :
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/base_hit.h>

namespace mctools {

  /// \brief The base class for all Monte-Carlo (MC) hit objects
  /**
   *   start
   *    o
   *     \
   *      \  particle track step
   *       \
   *        \
   *         \
   *          o
   *          stop
   *
   */
  class base_step_hit : public geomtools::base_hit
  {

  public:

    /// \brief Masks to automatically tag the attributes to be stored
    enum store_mask_type {
      STORE_POSITION_START = datatools::bit_mask::bit03, //!< Serialization mask for the position start attribute
      STORE_POSITION_STOP  = datatools::bit_mask::bit04, //!< Serialization mask for the position stop attribute
      STORE_TIME_START     = datatools::bit_mask::bit05, //!< Serialization mask for the time start attribute
      STORE_TIME_STOP      = datatools::bit_mask::bit06, //!< Serialization mask for the time stop attribute
      STORE_MOMENTUM_START = datatools::bit_mask::bit07, //!< Serialization mask for the momentum start attribute
      STORE_MOMENTUM_STOP  = datatools::bit_mask::bit08, //!< Serialization mask for the momentum stop attribute
      STORE_ENERGY_DEPOSIT = datatools::bit_mask::bit09, //!< Serialization mask for the energy deposit attribute
      STORE_PARTICLE_NAME  = datatools::bit_mask::bit10, //!< Serialization mask for the particle name attribute
      STORE_BIASING_WEIGHT = datatools::bit_mask::bit11  //!< Serialization mask for the biasing weight attribute (BUG 2015-02-26: was datatools::bit_mask::bit20)
    };

    // Extract specific properties from the 'auxiliaries' container:

    /// Check if the MC hit has been generated by a primary particle
    bool is_primary_particle () const;

    /// Check if the hit records the track ID
    bool has_track_id () const;

    /// Check if the hit records the parent track ID
    bool has_parent_track_id () const;

    /// Get the track ID
    int get_track_id () const;

    /// Get the parent track ID
    int get_parent_track_id () const;

    /// Get the start position of the hit (the coordinate system and units are arbitrary)
    const geomtools::vector_3d & get_position_start () const;

    /// Set the start position of the hit (the coordinate system and units are arbitrary)
    void set_position_start (const geomtools::vector_3d &);

    /// Reset/invalidate the start position of the hit
    void invalidate_position_start ();

    /// Get the stop position of the hit (the coordinate system and units are arbitrary)
    const geomtools::vector_3d & get_position_stop () const;

    /// Set the stop position of the hit (the coordinate system and units are arbitrary)
    void set_position_stop (const geomtools::vector_3d &);

    /// Reset/invalidate the stop position of the hit
    void invalidate_position_stop ();

    /// Get the stop momentum of the hit (units are arbitrary)
    const geomtools::vector_3d & get_momentum_stop () const;

    /// Set the stop momentum of the hit (units are arbitrary)
    void set_momentum_stop (const geomtools::vector_3d &);

    /// Reset/invalidate the stop momentum of the hit
    void invalidate_momentum_stop ();

    /// Get the start momentum of the hit (units are arbitrary)
    const geomtools::vector_3d & get_momentum_start () const;

    /// Set the start momentum of the hit (units are arbitrary)
    void set_momentum_start (const geomtools::vector_3d &);

    /// Reset/invalidate the start momentum of the hit
    void invalidate_momentum_start ();

    /// Get the start time of the hit (units are arbitrary)
    double get_time_start () const;

    /// Set the start time of the hit (units are arbitrary)
    void set_time_start (double);

    /// Reset/invalidate the start time of the hit
    void invalidate_time_start ();

    /// Get the stop time of the hit (units are arbitrary)
    double get_time_stop () const;

    /// Set the stop time of the hit (units are arbitrary)
    void set_time_stop (double);

    /// Reset/invalidate the stop time of the hit
    void invalidate_time_stop ();

    /// Get the energy deposit along the hit (units are arbitrary)
    double get_energy_deposit () const;

    /// Set the energy deposit along the hit (units are arbitrary)
    void set_energy_deposit (double);

    /// Reset/invalidate the energy deposit along the hit
    void invalidate_energy_deposit ();

    /// Get the particle name associated to the hit
    const std::string & get_particle_name () const;

    /// Set the particle name associated to the hit
    void set_particle_name (const std::string &);

    /// Reset/invalidate the particle name associated to the hit
    void invalidate_particle_name ();

    /// Check if a biasing weight is associated to the hit
    bool has_biasing_weight() const;

    /// Set the current biasing  weight of the particle track
    void set_biasing_weight(double);

    /// Return the current biasing  weight of the particle track
    double get_biasing_weight() const;

    /// Reset/invalidate the current biasing weight
    void invalidate_biasing_weight();

    /// Check if the hit has a valid internal structure
    /** We consider a base step hit valid if:
     * - a valid hit ID is set
     * - a valid geometry ID is set
     * - start time is set
     * - start position is set
     */
    virtual bool is_valid () const;

    /// Reset/invalidate the internal structure of the hit
    virtual void invalidate ();

    /// Default constructor
    base_step_hit ();

    // Destructor
    virtual ~base_step_hit ();

    /// Reset/invalidate the internal structure of the hit
    void reset ();

    /// Reset/invalidate the internal structure of the hit
    virtual void clear ();

    /// Smart print
    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    /// Smart print (default behaviour/shortcut)
    void dump() const;

  private:

    // Attributes :

    geomtools::vector_3d         _position_start_; //!< Start position : beginning of the tiny track segment (step)
    geomtools::vector_3d         _position_stop_;  //!< Stop position  : end of the tiny track segment (step)
    double                       _time_start_;     //!< Start time at start position
    double                       _time_stop_;      //!< Stop time at stop position
    geomtools::vector_3d         _momentum_start_; //!< Momentum at start position
    geomtools::vector_3d         _momentum_stop_;  //!< Momentum at stop position
    double                       _energy_deposit_; //!< Energy deposit along the track segment (step)
    std::string                  _particle_name_;  //!< Name of the particle associated to the hit
    double                       _biasing_weight_; //!< The biasing weight of the particle track

    DATATOOLS_SERIALIZATION_DECLARATION()

#if MCTOOLS_WITH_REFLECTION == 1
    //! Reflection interface
    DR_CLASS_RTTI()
#endif

  };

} // end of namespace mctools

#if MCTOOLS_WITH_REFLECTION == 1
// Activate reflection layer for the mctools::base_step_hit class :
DR_CLASS_INIT(::mctools::base_step_hit)
#endif // MCTOOLS_WITH_REFLECTION

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(mctools::base_step_hit, 1)

#endif // MCTOOLS_BASE_STEP_HIT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/