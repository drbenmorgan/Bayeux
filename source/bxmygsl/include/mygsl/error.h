/// \file mygsl/error.h

#ifndef MYGSL_ERROR_H
#define MYGSL_ERROR_H 1

// Standard library:
#include <string>

// Third party:
// - GSL:
#include <gsl/gsl_errno.h>

namespace mygsl {

  /// \brief GSL error handling
  class error
  {
  public:

    static void to_string (const int gsl_errno_ ,
                           std::string & err_string_);

    static std::string to_string (const int gsl_errno_);

    static void set_handler (gsl_error_handler_t &);

    static void off ();

    static void on ();

    static void set_default ();

    static void set_gsl_default ();

    static void default_handler(const char * reason_ ,
                                const char * file_ ,
                                int line_ ,
                                int gsl_errno_);
  private:

    error();

    ~error();

  private:

    static bool  _active_;
    static error _singleton_;

  };

#define MYGSL_ERROR(reason_, gsl_errno_)                        \
  do {                                                          \
    gsl_error (reason_, __FILE__, __LINE__, gsl_errno_) ;       \
  } while (0)

}

#endif // MYGSL_ERROR_H

// Local Variables:
// mode: c++
// coding: utf-8
// End:
