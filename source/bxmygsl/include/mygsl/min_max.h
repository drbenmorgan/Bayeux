// -*- mode: c++; -*-
/* min_max.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2013-04-23
 *
 * License:
 *
 * Description:
 *
 *   A collection of algorithm to compute the min_max of
 *   a list of numbers
 *
 */

#ifndef MYGSL_MIN_MAX_H_
#define MYGSL_MIN_MAX_H_ 1

namespace mygsl {

  class min_max
  {
  public:
    bool is_valid() const;
    double get_min() const;
    double get_max() const;
    double get_median() const;
    min_max();
    void reset();
    void add(double value_);
  private:
    double _min_;
    double _max_;
  };

} // end of namespace mygsl

#endif // MYGSL_MIN_MAX_H_

// end of min_max.h
