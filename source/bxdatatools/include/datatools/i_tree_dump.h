/* i_tree_dump.h */
#ifndef DATATOOLS_I_TREE_DUMP_H_
#define DATATOOLS_I_TREE_DUMP_H_

// Standard Library
#include <iostream>
#include <string>

// Datatools
#include <datatools/datatools_config.h>

// Datatools reflection utilities :
#include <datatools/reflection_macros.h>

namespace datatools {

/*! \brief An abstract interface with utilities for printable objects.
 *
 */
class i_tree_dumpable {
 public:

  struct tags {
    static const std::string & item();
    static const std::string & last_item();
    static const std::string & skip_item();
    static const std::string & last_skip_item();
  };

  enum ostream_type {
    OSTREAM_COUT=1,
    OSTREAM_CERR=2,
    OSTREAM_CLOG=3
  };

 public:

  static std::ostream & last_skip_tag(std::ostream& out);

  static std::ostream & skip_tag(std::ostream& out);

  static std::ostream & last_tag(std::ostream& out);

  static std::ostream & tag(std::ostream& out);

  /// Main interface method for smart dump
  virtual void tree_dump (std::ostream& out = std::clog,
                          const std::string& title  = "",
                          const std::string& indent = "",
                          bool inherit = false) const = 0;

  void tree_dump (int out_type = OSTREAM_CLOG,
		  const std::string& title  = "",
		  const std::string& indent = "",
		  bool inherit = false) const;

  void tree_print (int out_type = OSTREAM_CLOG,
		   const std::string& title= "") const;

  void smart_print (int out_type = OSTREAM_CLOG,
		    const std::string& title = "",
		    const std::string& indent = "") const;

  /// Output stream manipulator
  class inherit_tag {
   public:
    inherit_tag(bool inherit);

    friend std::ostream & operator<<(std::ostream& out,
                                     const inherit_tag& last_tag);

   private:
     bool inherit_;
  };

  /// Output stream manipulator
  class inherit_skip_tag {
   public:
    inherit_skip_tag(bool inherit);

    friend std::ostream& operator<<(std::ostream& out,
                                    const inherit_skip_tag& last_tag);
   private:
    bool inherit_;
  };

    DR_CLASS_RTTI();

};

} // end of namespace datatools

// Activate reflection layer for the 'datatools::i_tree_dumpable' class:
DR_CLASS_INIT(::datatools::i_tree_dumpable);

#endif // DATATOOLS_I_TREE_DUMP_H_

