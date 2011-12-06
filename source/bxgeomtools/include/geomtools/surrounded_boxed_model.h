// -*- mode: c++ ; -*- 
/* surrounded_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-04-02
 * 
 * License: 
 * 
 * Description: 
 *
 *   Geometry model with multiple surrounded boxes.
 * 
 * History: 
 * 
 */

#ifndef __geomtools__surrounded_boxed_model_h
#define __geomtools__surrounded_boxed_model_h 1

#include <string> 
#include <iostream>
#include <vector>

#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/model_macros.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with some surrounded boxed models along
  // a specific axis (X, Y or Z)
  class surrounded_boxed_model : GEOMTOOLS_BOXED_MODEL_INHERIT
  {
  public:

    static const string LABEL_PROPERTY_PREFIX;
    static const string MODEL_PROPERTY_PREFIX;
    static const string SURROUNDED_LABEL;
    static const string SURROUNDING_LABEL;

    struct surrounding_item
    {
      string           label;
      const i_model *  model;
      placement        placmt;
      physical_volume  phys;

    public:
      const string & get_label () const
      {
        return label;
      }
      const i_model & get_model () const
      {
        return *model;
      }
      const placement & get_placement () const
      {
        return placmt;
      }
      const physical_volume & get_physical_volume () const
      {
        return phys;
      }
    };

    typedef map<int, surrounding_item> surrounding_dict_t;
    typedef map<string, int> labels_dict_t;

  private:

    const string & __get_material_name () const;

  public: 

    bool is_debug () const;
    
    void set_debug (bool);

    const string & get_material_name () const;

    void set_material_name (const string &);

    virtual const geomtools::box & get_box () const;
    const geomtools::box & get_solid () const;

    void set_surrounded_label (const string & label_);
    const string & get_surrounded_label () const;
    void set_surrounded_model (const i_model & model_);
    const i_model & get_surrounded_model () const;

    size_t get_number_of_surrounding_items () const;
    bool has_surrounding_model (const string & label_) const;
    void add_surrounding_model (int pos_, const i_model &, const string & label_ = "");
    const i_model & get_surrounding_model (int i_) const;
    const surrounding_item & get_surrounding_item (int i_) const;
    const i_model & get_surrounding_model (const string & label_) const;
    const surrounding_item & get_surrounding_item (const string & label_) const;
    const labels_dict_t & get_surrounding_labels () const;
    const surrounding_dict_t & get_surrounding_items () const;

  public:
  
    surrounded_boxed_model ();
  
    virtual ~surrounded_boxed_model ();

  public:

    virtual string get_model_id () const;

  protected:
  
    virtual void _at_construct (const string & name_,
                                const datatools::utils::properties & config_,
                                models_col_t * models_ = 0);
      
  public: 

    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;
 
  private:

    bool _debug_;

    vector<string>      _position_labels_;
    string              _material_name_;

    const i_model *     _surrounded_model_;
    string              _surrounded_label_;
    placement           _surrounded_placmt_;
    physical_volume     _surrounded_phys_;

    surrounding_dict_t  _surrounding_items_;
    labels_dict_t       _surrounding_labels_;
    bool                _centered_x_;
    bool                _centered_y_;
    bool                _centered_z_;
    geomtools::box      _solid_;

    // 2011-12-05 FM : add support for additional internal objects :
    // internal items :
    MWIM                    _internals_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(surrounded_boxed_model);

  };

} // end of namespace geomtools

#endif // __geomtools__surrounded_boxed_model_h

// end of surrounded_boxed_model.h
