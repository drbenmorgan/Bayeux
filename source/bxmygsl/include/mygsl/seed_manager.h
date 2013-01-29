/* seed_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-02-26
 * Last modified : 2012-06-08
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301, USA.
 * 
 * Description: 
 *
 *   A manager for PRNG's seeds
 * 
 * History: 
 * 
 */

#ifndef __mygsl__seed_manager_h
#define __mygsl__seed_manager_h 1

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

#include <datatools/bit_mask.h>

namespace mygsl {

  class rng;

  /** A seed manager instance is responsible to save
   *  a collection of seeds used to initialize a set
   *  of pseudo-random number generators (PRNG). As 
   *  the PRNGs are all labelled with a name. The 
   *  seed manager uses an internal dictionnary to store 
   *  the seeds. It is possible to force a seed value
   *  for a given PRNG as well as to use a seed determined
   *  by some specific PRNG initialized with the current time.
   *  I/O operations on streams enable to store and load the
   *  content of the seed manager.
   */
  class seed_manager
  {
  public: 
      
    /// An alias for the embedded seed dictionnary container class
    typedef std::map<std::string, int32_t> dict_type;

    static const std::string INIT_SEED_FROM_ENV_NAME;
      
    enum init_seed_from_type
      {
        INIT_SEED_FROM_UNDEFINED    = 0,
        INIT_SEED_FROM_CURRENT_TIME = datatools::bit_mask::bit00,
        INIT_SEED_FROM_CURRENT_PID  = datatools::bit_mask::bit01,
        INIT_SEED_FROM_CURRENT_TIME_AND_PID = INIT_SEED_FROM_CURRENT_TIME | INIT_SEED_FROM_CURRENT_PID ,
        INIT_SEED_FROM_URANDOM = datatools::bit_mask::bit02,
        //INIT_SEED_FROM_RANDOM_DEVICE = datatools::bit_mask::bit03
        INIT_SEED_FROM_DEFAULT = INIT_SEED_FROM_URANDOM,
      };

  public: 
      
    /// Return true if the seed has a valid value
    static bool seed_is_valid (int);

    /// Check if the debug mode is activated
    bool is_debug () const;
      
    /// Set/unset the debug mode
    void set_debug (bool);
 
    /// Build an array of all the PRNGs' labels stored in the manager
    void get_labels (std::vector<std::string> & labels_) const;

    /// Check is a PRNG with a given label has an associated seed
    bool has_seed (const std::string & label_) const;
      
    /// Install the seed value associated to a PRNG with a given label
    void add_seed (const std::string & label_, int32_t seed_);

    /// Get the seed value associated to a PRNG with a given label
    int32_t get_seed (const std::string & label_) const;

    /// Change the seed value associated to a PRNG with a given label
    void update_seed (const std::string & label_, int32_t seed_);

    /// Clear the list of stored seeds
    void clear ();

    /// Make all seeds associated to all PRNGs invalid 
    void invalidate ();

    /// Make all seeds associated to all PRNGs set by the current time
    void all_time_seeds ();

    /// Compute the effective seeds' values from the current time
    void transform_time_seeds (bool allow_duplication_ = false);

    /// Compute the effective seeds' values to be all different from each others
    void ensure_different_seeds ();

    /// Check if all valid seeds are differents :
    bool all_different_seeds () const;

    /// Check if some valid seeds are 'time' :
    bool has_time_seeds () const;

    /// Check if some seeds are 'invalid' :
    bool has_invalid_seeds () const;

  protected:

    void _ensure_different_seeds (mygsl::rng * random_ = 0);
 
    int32_t _set_seed_for_seeds ();

  private:
    
    void _set_init_seed_flags_ ();

  public:

    /// Returns the number of PRNG's seed values storedin the manager
    size_t size () const;
      
    /// Check if the manager is empty 
    bool empty () const;

  public: 
      
    /// Default constructor:
    seed_manager ();
      
    /// Destructor:
    virtual ~seed_manager ();

    void set_init_seed_flags (uint32_t);

    uint32_t get_init_seed_flags () const;
      
  public:

    /// Basic print
    void dump (std::ostream &) const;

    /// Store the seeds informations in a stream
    friend std::ostream & operator<< (std::ostream &, const seed_manager &);

    /// Load the seeds informations froma stream
    friend std::istream & operator>> (std::istream &, seed_manager &);
      
  private: 
      
    bool      _debug_; /// Debug flag   
    uint32_t  _init_seed_flags_; /// Seed initialization flags
    dict_type _dict_;  /// Dictionnary of seeds associated to PRNGs' labels

  };

  /// An alias
  typedef seed_manager prng_seed_manager;

}  // end of namespace mygsl

#endif // __mygsl__seed_manager_h

// end of seed_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
