// -*- mode: c++ ; -*- 
/* random_utils.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-16
 * Last modified: 2010-02-16
 * 
 * License: 
 *
 * Copyright 2010-2012 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *  A test class tat does nothing and is not used.
 *
 * History: 
 * 
 */

#ifndef __mygsl__random_utils_h
#define __mygsl__random_utils_h 1

#include <boost/cstdint.hpp>

namespace mygsl {
  
  /// A class that hosts useful constants related to the management
  /// of pseudo-random number generators (PRNG)
  class random_utils
  {

  public:
      
    static const int32_t SEED_INVALID = -1; //!< Constant that represents an invalid seed value
    static const int32_t SEED_TIME    =  0; //!< Constant that represents a seed that must be initialized from some external 'random' source like the current time, the process ID or the /dev/urandom source (or combinaison of them)
     
  };

} // end of namespace mygsl

#endif // __mygsl__random_utils_h

// end of random_utils.h
