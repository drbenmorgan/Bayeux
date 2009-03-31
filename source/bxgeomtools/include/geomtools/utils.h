// -*- mode: c++; -*- 
/* utils.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__utils_h
#define __geomtools__utils_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <cmath>
#include <list>

#include <geomtools/clhep.h>
#include <datatools/serialization/serialization.h>
#include <boost/serialization/split_free.hpp>

namespace geomtools {

  typedef std::list<vector_3d> basic_polyline_t;

  enum orientation_type
    {
      VERTICAL   = 0,
      HORIZONTAL = 1
    };

  enum direction_type
    {
      BACK   = 0, // -x
      FRONT  = 1, // +x
      LEFT   = 2, // -y
      RIGHT  = 3, // +y
      BOTTOM = 4, // -z
      TOP    = 5  // +z
    };

  enum face_3d
    {
      FACE_NONE_BIT = 0x0,
      FACE_ALL_BITS = 0xFFFFFFFF
    };
  
  struct constants
  {
    static const int NO_INTERCEPT = -1;
    static const double DEFAULT_TOLERANCE;
  };

  // see also CLHEP/Vector/TwoVector.h and CLHEP/Vector/ThreeVector.h
  /*
    enum axis_type
    {
    X = 0,
    Y = 1,
    Z = 2
    };
  */
    
  /* Initialize a rotation matrix for 
   * "World coordinates system->Local coordinates system":
   */
  void
  create_rotation (rotation_3d & rot_,
		   double phi_,
		   double theta_,
		   double delta_);

  void
  reset_rotation (rotation_3d & rot_);
    
  void
  tree_dump (const rotation_3d & rot_,
	     std::ostream & out_, 
	     const std::string & title_ = "", 
	     const std::string & indent_ = "");
  
  void 
  invalidate (rotation_3d & rot_);

  bool 
  is_valid (const rotation_3d & rot_);
  
  void 
  invalidate (vector_3d & vec_);
  
  bool 
  is_valid (const vector_3d & vec_);
  
  void 
  invalidate (vector_2d & vec_);
  
  bool 
  is_valid (const vector_2d & vec_);

  bool 
  are_near (const vector_3d & vec1_, 
	    const vector_3d & vec2_,
	    double tolerance_ = constants::DEFAULT_TOLERANCE);

  /* the 'ran_func' (functor) must have the following operator
   * defined:
   *   double operator() (double);
   * it returns a uniform deviated random number in the [0,1) range
   * like 'drand48' does.
   */
  template <class ran_func>
  void
  randomize_direction (ran_func ran_, vector_3d & ran_dir_)
  {
    double phi = 2. * M_PI * ran_ ();
    double cos_theta = -1 + 2 * ran_ ();
    double sin_theta = std::sqrt (1. - cos_theta * cos_theta);
    double x = sin_theta * std::cos (phi);
    double y = sin_theta * std::sin (phi);
    double z = cos_theta;
    ran_dir_.set (x, y, z);
  }

  template <class ran_func>
  vector_3d
  randomize_direction (ran_func ran_)
  {
    vector_3d dir;
    randomize_direction (ran_, dir);
    return dir;
  }


  /* the 'ran_func' (functor) must have the following operator
   * defined:
   *   double operator() (double);
   * it returns a uniform deviated random number in the [0,1) range
   * like 'drand48' does.
   */
  template <class ran_func>
  void
  randomize_orthogonal_direction (ran_func ran_, 
				  const vector_3d & dir_, 
				  vector_3d & ran_dir_)
  {
    double theta = 2. * M_PI * ran_ ();
    double dx = std::cos (theta);
    double dy = std::sin (theta);
    double dz = 0.0;
    double dir_theta = dir_.theta ();
    double dir_phi = dir_.phi ();
    std::clog << "theta (dir) = " 
	      << 180.* dir_theta / M_PI << "°" << std::endl;
    std::clog << "phi (dir) = " 
	      << 180.* dir_phi / M_PI << "°"  << std::endl;
    rotation_3d dir_rot;
    create_rotation (dir_rot, dir_phi, dir_theta, 0.0);
    rotation_3d dir_inverse_rot;
    dir_inverse_rot = dir_rot.inverse ();
    vector_3d v (dx, dy, dz);
    ran_dir_ = v.transform (dir_inverse_rot); // XXX
  }
    
  template <class ran_func>
  vector_3d
  randomize_orthogonal_direction (ran_func ran_, const vector_3d & ref_dir_)
  {
    vector_3d dir;
    randomize_direction (ran_, ref_dir_, dir);
    return dir;
  }

  class rotation_wrapper_t : public rotation_3d
  {
  public:
    rotation_wrapper_t (double mxx_, double mxy_, double mxz_, 
			double myx_, double myy_, double myz_,
			double mzx_, double mzy_, double mzz_) 
     : rotation_3d (mxx_, mxy_, mxz_, 
		 myx_, myy_, myz_,
		 mzx_, mzy_, mzz_) {}
  };

  class intercept_t
  {
  private:

    int       __shape_index;
    int       __face;
    vector_3d __impact;

  public:

    int get_shape_index () const
    {
      return __shape_index;
    }

    void set_shape_index (int si_)
    {
      __shape_index = si_;
    }

    int get_face () const
    {
      return __face;
    }

    void set_face (int face_)
    {
      __face = face_;
    }

    void set_impact (const vector_3d & point_)
    {
      __impact = point_;
    }

    void set (int shape_index_, int face_, const vector_3d & point_)
    {
      set_shape_index (shape_index_);
      set_face (face_);
      set_impact (point_);
    }

    const vector_3d & get_impact () const
    {
      return __impact;
    }

    void reset ()
    {
      __shape_index = -1;
      __face = FACE_NONE_BIT;
      invalidate (__impact);
    }

    intercept_t ()
    {
      __shape_index = -1;
      __face = FACE_NONE_BIT;
      invalidate (__impact);
    }

    bool is_valid () const
    {
      return __shape_index >= 0 && __face != FACE_NONE_BIT;
    }

    bool is_ok () const
    {
      return is_valid ();
    }

  };
} // end of namespace geomtools


// serialization stuff for CLHEP 'vector_3d':
namespace boost {
namespace serialization {

  template<class Archive>
  void
  save (Archive & ar_ , 
	const geomtools::vector_3d & v_,
	const unsigned int version_)
  {
    double x = v_.getX ();
    double y = v_.getY ();
    double z = v_.getZ ();
    ar_ & boost::serialization::make_nvp ("x", x);
    ar_ & boost::serialization::make_nvp ("y", y);
    ar_ & boost::serialization::make_nvp ("z", z);
  }

  template<class Archive>
  void
  load (Archive & ar_ , 
	geomtools::vector_3d & v_,
	const unsigned int version_)
  {
    double x;
    double y;
    double z;
    ar_ & boost::serialization::make_nvp ("x", x);
    ar_ & boost::serialization::make_nvp ("y", y);
    ar_ & boost::serialization::make_nvp ("z", z);
    v_.set (x, y, z);
  }

  template<class Archive>
  void 
  serialize (Archive & ar_,
	     geomtools::vector_3d  & v_,
	     const unsigned int version_)
  {
    boost::serialization::split_free (ar_, v_, version_);
  } 

} // namespace serialization
} // namespace boost

// serialization stuff for CLHEP 'vector_2d':
namespace boost {
namespace serialization {

  template<class Archive>
  void
  save (Archive & ar_ , 
	const geomtools::vector_2d & v_,
	const unsigned int version_)
  {
    double x = v_.x ();
    double y = v_.y ();
    ar_ & boost::serialization::make_nvp ("x", x);
    ar_ & boost::serialization::make_nvp ("y", y);
  }

  template<class Archive>
  void
  load (Archive & ar_ , 
	geomtools::vector_2d & v_,
	const unsigned int version_)
  {
    double x;
    double y;
    ar_ & boost::serialization::make_nvp ("x", x);
    ar_ & boost::serialization::make_nvp ("y", y);
    v_.set (x, y);
  }

  template<class Archive>
  void 
  serialize (Archive & ar_,
	     geomtools::vector_2d  & v_,
	     const unsigned int version_)
  {
    boost::serialization::split_free (ar_, v_, version_);
  } 

} // namespace serialization
} // namespace boost

// serialization stuff for CLHEP 'rotation':
namespace boost {
namespace serialization {

  template<class Archive>
  void
  save (Archive & ar_ , 
	const geomtools::rotation_3d & r_,
	const unsigned int version_)
  {
    double rxx = r_.xx ();
    ar_ & boost::serialization::make_nvp ("xx", rxx);
    if (geomtools::is_valid (r_))
      {
	double rxy, rxz, ryx, ryy, ryz, rzx, rzy, rzz;
	rxy = r_.xy ();
	rxz = r_.xz ();
	ryx = r_.yx ();
	ryy = r_.yy ();
	ryz = r_.yz ();
	rzx = r_.zx ();
	rzy = r_.zy ();
	rzz = r_.zz ();
	ar_ & boost::serialization::make_nvp ("xy", rxy);
	ar_ & boost::serialization::make_nvp ("xz", rxz);
	ar_ & boost::serialization::make_nvp ("yx", ryx);
	ar_ & boost::serialization::make_nvp ("yy", ryy);
	ar_ & boost::serialization::make_nvp ("yz", ryz);
	ar_ & boost::serialization::make_nvp ("zx", rzx);
	ar_ & boost::serialization::make_nvp ("zy", rzy);
	ar_ & boost::serialization::make_nvp ("zz", rzz);
      }
  }

  template<class Archive>
  void
  load (Archive & ar_ , 
	geomtools::rotation_3d & r_,
	const unsigned int version_)
  {
    double rxx (0.0), rxy (0.0), rxz (0.0);
    double ryx (0.0), ryy (0.0), ryz (0.0);
    double rzx (0.0), rzy (0.0), rzz (0.0);
    ar_ & boost::serialization::make_nvp ("xx", rxx); 
    if (rxx == rxx)
      {
	ar_ & boost::serialization::make_nvp ("xy", rxy);
	ar_ & boost::serialization::make_nvp ("xz", rxz);
	ar_ & boost::serialization::make_nvp ("yx", ryx);
	ar_ & boost::serialization::make_nvp ("yy", ryy);
	ar_ & boost::serialization::make_nvp ("yz", ryz);
	ar_ & boost::serialization::make_nvp ("zx", rzx);
	ar_ & boost::serialization::make_nvp ("zy", rzy);
	ar_ & boost::serialization::make_nvp ("zz", rzz);
	r_ = geomtools::rotation_wrapper_t (rxx, rxy, rxz, ryx, ryy, ryz, rzx, rzy, rzz);
      }
    else 
      {
	r_ = geomtools::rotation_3d ();
	geomtools::invalidate (r_);
      }
  }

  template<class Archive>
  void 
  serialize (Archive & ar_,
	     geomtools::rotation_3d  & r_,
	     const unsigned int version_)
  {
    boost::serialization::split_free (ar_, r_, version_);
  } 

} // namespace serialization
} // namespace boost
  

#endif // __geomtools__utils_h

// end of utils.h
