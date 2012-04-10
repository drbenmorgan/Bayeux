// -*- mode: c++ ; -*- 
/* i_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2012-04-09
 * 
 * License: 
 * 
 * Description: 
 *
 *   Interface of a geometry model
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_model_h
#define __geomtools__i_model_h 1

#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>

#include <geomtools/logical_volume.h>
#include <geomtools/detail/model_tools.h>
#include <datatools/factory/factory_macros.h>

namespace geomtools {
  
  using namespace std;  
  
  class i_model : public datatools::utils::i_tree_dumpable
  {
  public:
    static bool g_devel;
   
    struct constants
    {
      std::string SOLID_SUFFIX;
      std::string LOGICAL_SUFFIX;
      std::string PHYSICAL_SUFFIX;
      std::string PHANTOM_SOLID_FLAG;
      //std::string DEFAULT_WORLD_NAME;
 
      constants ();

      static const constants & instance ();

    };

    typedef geomtools::models_col_t models_col_t;
     
  public: 

    void assert_constructed (const std::string & where_, 
                             const std::string & what_ = "") const;

    void assert_unconstructed (const std::string & where_, 
                               const std::string & what_ = "") const;

    bool is_constructed () const;
    
    bool is_debug () const;

    bool is_phantom_solid () const;
    
    void set_name (const std::string & name_); 
    
    void set_debug (bool); 
    
    const std::string & get_name () const;
    
    const datatools::utils::properties & parameters () const;
    
    datatools::utils::properties & parameters ();

  protected :
 
    void _set_phantom_solid (bool);
   
  public: 

    /// Constructor
    i_model (const std::string & name_ = "");
    
    /// Destructor
    virtual ~i_model ();
    
    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;
    
    /// Get a non mutable reference to the embedded logical volume
    const geomtools::logical_volume & get_logical () const;

    /// Get a mutable reference to the embedded logical volume
    geomtools::logical_volume & grab_logical ();

    /// Get a mutable reference to the embedded logical volume (deprecated : use i_model::grab_logical, still used in geomtools models...)
    geomtools::logical_volume & get_logical ();
    
    virtual void construct (const std::string & name_,
                            const datatools::utils::properties & setup_,
                            models_col_t * models_ = 0);
     
  protected:

    virtual void _pre_construct (datatools::utils::properties & setup_);

    virtual void _post_construct (datatools::utils::properties & setup_);
 
    virtual void _at_construct (const std::string & name_,
                                const datatools::utils::properties & setup_,
                                models_col_t * models_ = 0);
      
  private: 

    bool _debug_;
    bool _constructed_;
    datatools::utils::properties _parameters_;
    std::string _name_;
    
  protected:

    bool                      _phantom_solid;
    geomtools::logical_volume _logical;
      
  public:
    
    virtual std::string get_model_id () const = 0;

  public:

    static std::string make_solid_name (const std::string & basename_);

    static std::string make_logical_volume_name (const std::string & basename_);

    static std::string make_physical_volume_name (const std::string & basename_);

    static std::string make_physical_volume_name_per_item (const std::string & basename_, 
                                                           int i_);

    static std::string make_physical_volume_name_per_item (const std::string & basename_, 
                                                           int i_, int j_);

    static std::string make_physical_volume_name (const std::string & basename_,
                                                  int nitems_);
 
    static std::string make_physical_volume_name (const std::string & basename_,
                                                  int ncols_,
                                                  int nrows_);

    static std::string extract_label_from_physical_volume_name (const std::string & physical_volume_name_);

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_model);
 
  }; // class i_model
  
} // end of namespace geomtools

#include <geomtools/model_macros.h>

#endif // __geomtools__i_model_h

// end of i_model.h
