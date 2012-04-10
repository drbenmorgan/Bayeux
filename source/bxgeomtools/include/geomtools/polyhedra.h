// -*- mode: c++ ; -*- 
/* polyhedra.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-08
 * Last modified: 2010-10-14
 * 
 * License: 
 * 
 * Description: 
 *   Polyhedra 3D shape with regular polygon 
 *   sections (fr: équilatéral, inscribed, circonscriptible)
 *   See also: http://en.wikipedia.org/wiki/Frustum
 *
 * History: 
 * 
 */

#ifndef __geomtools__polyhedra_h
#define __geomtools__polyhedra_h 1

#include <iostream>
#include <string>
#include <map>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  using namespace std;

  class polyhedra : public i_shape_3d, public i_stackable
  {
  public:

    static const string POLYHEDRA_LABEL;
    static const size_t MIN_NUMBER_OF_SIDES = 3;

    enum faces_mask_t
      {
        FACE_NONE       = FACE_NONE_BIT,
        FACE_INNER_SIDE = 0x1,
        FACE_OUTER_SIDE = 0x2,
        FACE_BOTTOM     = 0x4,
        FACE_TOP        = 0x8,
        FACE_ALL        = (FACE_INNER_SIDE
                           | FACE_OUTER_SIDE
                           | FACE_BOTTOM 
                           | FACE_TOP)
      };  

    /**
     * The polyhedra is defined by a list of (z, rmin, rmax) triplets
     * the radius are the tangential radius (radius of the inscribed
     * circles) and not the distance from Z-axis to the corners.
     *
     * Example with a 'n_sides==6' polyhedra:
     *        
     *         C           B
     *          +_________+
     *         /           \ 
     *        /            .+ T
     *       /          .    \
     *    D +        +        + A
     *       \      O        /
     *        \             /
     *         \___________/
     *         +           +
     *        E             F
     *
     */
    struct r_min_max
    {
      double rmin, rmax;
    };

    typedef map<double, r_min_max> rz_col_t;

  private:

    size_t   _n_sides_;
    rz_col_t _points_;
    double  _top_surface_;
    double  _bottom_surface_;
    //double  _side_surface_;
    double  _outer_side_surface_;
    double  _inner_side_surface_;
    double  _outer_volume_;
    double  _inner_volume_;
    double  _volume_;
    double  _z_min_;
    double  _z_max_;
    double  _r_max_;
    double  _xy_max_;
    bool    _extruded_;
    
  public: 

    bool is_extruded () const;
    
    //>>> stackable interface:
    double get_xmin () const
    {
      return -_xy_max_;
    }
    
    double get_xmax () const
    {
      return +_xy_max_;
    }
    
    double get_ymin () const
    {
      return -_xy_max_;
    }
    
    double get_ymax () const
    {
      return +_xy_max_;
    }
    
    double get_zmin () const
    {
      return _z_min_;
    }
    
    double get_zmax () const
    {
      return _z_max_;
    }
    //<<<

    double get_r_max () const;

    vector_3d get_corner (int zplane_index_, 
                          int corner_index_, 
                          bool inner_ = false) const;

  private:

    void _compute_surfaces_ ();

    void _compute_volume_ ();

    void _compute_limits_ ();

    void _compute_all_ ();

  public:

    void set_n_sides (size_t n_sides_);

    size_t get_n_sides () const;

    const rz_col_t & points () const;

    // ctor:
    polyhedra ();

    // dtor:
    virtual ~polyhedra ();
  
    // methods:
      
    virtual string get_shape_name () const;

    bool is_valid () const;

    void reset ();

    void add (double z_, double rmax_, bool compute_ = true);

    void add (double z_, double rmin_, double rmax_, bool compute_ = true);

    void initialize (const datatools::utils::properties & setup_); 

    void initialize (const string & filename_);

    void compute_inner_polyhedra (polyhedra & ip_);

    void compute_outer_polyhedra (polyhedra & op_);

    double get_volume () const;

    double get_surface (int mask_ = FACE_ALL) const;

    double get_z_min () const;

    double get_z_max () const;

    double get_xy_max () const;

    double get_parameter ( const string & flag_ ) const;

    virtual bool is_inside (const vector_3d &, 
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool 
    is_on_surface (const vector_3d & , 
                   int mask_    = FACE_ALL , 
                   double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    friend ostream & operator<< (ostream &, const polyhedra &);

    friend istream & operator>> (istream &, polyhedra &);
      
    virtual bool find_intercept (const vector_3d & from_, 
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;

  };

} // end of namespace geomtools

#endif // __geomtools__polyhedra_h

// end of polyhedra.h
