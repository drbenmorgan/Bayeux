/// \file mygsl/histogram_2d.ipp

#ifndef MYGSL_HISTOGRAM_2D_IPP
#define MYGSL_HISTOGRAM_2D_IPP 1

// Ourselves:
#include <mygsl/histogram_2d.h>

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>
#include <datatools/properties.ipp>

namespace mygsl {

  template<class Archive>
  void histogram_2d::serialize (Archive & ar,
                                const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("x_binning_info", _x_binning_info_);
    ar & boost::serialization::make_nvp ("y_binning_info", _y_binning_info_);
    ar & boost::serialization::make_nvp ("counts",         _counts_);
    ar & boost::serialization::make_nvp ("x_underflow",    _x_underflow_);
    ar & boost::serialization::make_nvp ("x_overflow",     _x_overflow_);
    ar & boost::serialization::make_nvp ("y_underflow",    _y_underflow_);
    ar & boost::serialization::make_nvp ("y_overflow",     _y_overflow_);

    if (Archive::is_saving::value)
      {
        uint32_t nx = _h_->nx;
        ar & boost::serialization::make_nvp ("nx", nx);
        uint32_t ny = _h_->ny;
        ar & boost::serialization::make_nvp ("ny", ny);
      }
    if  (Archive::is_loading::value)
      {
        if (_h_ != 0)
          {
            gsl_histogram2d_free (_h_);
            _h_ = 0;
          }
        uint32_t nx = 0;
        ar & boost::serialization::make_nvp ("nx", nx);
        uint32_t ny = 0;
        ar & boost::serialization::make_nvp ("ny", ny);
        _h_ = gsl_histogram2d_alloc (nx, ny);
      }
    if (_h_->nx > 0 && _h_->ny > 0)
      {
        for (size_t ii = 0 ; ii < _h_->nx + 1; ii ++ )
          {
            std::string tag = "xrange_" + boost::lexical_cast<std::string>(ii) ;
            ar & boost::serialization::make_nvp(tag.c_str(), _h_->xrange[ii]) ;
          }
        for (size_t ii = 0 ; ii < _h_->ny + 1; ii ++ )
          {
            std::string tag = "yrange_" + boost::lexical_cast<std::string>(ii) ;
            ar & boost::serialization::make_nvp(tag.c_str(), _h_->yrange[ii]) ;
          }
        for (size_t ii = 0 ; ii < _h_->nx * _h_->ny; ii ++ )
          {
            std::string tag = "bin_" + boost::lexical_cast<std::string>(ii) ;
            ar & boost::serialization::make_nvp(tag.c_str(), _h_->bin[ii]) ;
          }
      }
    ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
    return;
  }

} // end of namespace mygsl

#endif // MYGSL_HISTOGRAM_2D_IPP

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
