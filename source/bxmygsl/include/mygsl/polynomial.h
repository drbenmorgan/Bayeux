// mygsl::polynomial.h

#ifndef __mygsl__polynomial_h 
#define __mygsl__polynomial_h 1

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <vector>

#include <gsl/gsl_math.h>
#include <gsl/gsl_poly.h>

#include <mygsl/unary_eval.h>

using namespace std;

namespace mygsl {

  class polynomial : public unary_eval
    {
    private:

      size_t         __size;

      vector<double> __c;

    public:

      size_t get_degree () const;

      size_t get_ncoeffs () const;

      double get_coeff (size_t i_) const;

      void set_coeff (size_t i_, double c_);

      polynomial (size_t sz_);

      polynomial (double c0_);

      polynomial (double c0_, double c1_);

      polynomial (double c0_, double c1_, double c2_);

      polynomial (const vector<double> & c_);

      polynomial (const polynomial & p_);

      virtual ~polynomial ();

      double eval (double x_) const;

      void print (ostream & out_= clog, 
		  int format_ = 0, 
		  bool eol_ = false) const;


      /*
       * L(x) = p0 + p1 * x^1
       *
       */
      static bool solve_linear (double p0_, double p1_, 
				size_t & nsols_,
				double & x0_);


      /*
       * Q(x) = p0 + p1 * x^1 + p2 * x^2
       *
       */
      static bool solve_quadratic (double p0_, double p1_, double p2_, 
				   size_t & nsols_,
				   double & x0_, double & x1_);


      /*
       * C(x) =  p0 + p1 * x^1 + p2 * x^2 + x^3
       *
       */
      static bool solve_cubic (double p0_, double p1_, double p2_, 
			       size_t & nsols_,
			       double & x0_, double & x1_, double & x2_);


      /*
       * C(x) = p0 + p1 * x^1 + p2 * x^2 + p3 * x^3
       *
       */
      static bool solve_cubic (double p0_, double p1_, double p2_, double p3_,
			       size_t & nsols_,
			       double & x0_, double & x1_, double & x2_);
  
      class solver
	{
	private:
	  int    __status;
	  size_t __sz;
	  gsl_poly_complex_workspace * __ws;

	private:

	  void __init (size_t);

	  void __reset ();

	public:

	  solver (size_t sz_ = 0);

	  virtual ~solver ();

	  bool solve (const polynomial & p_);

	};

    };



}

#endif // __mygsl__polynomial_h

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::polynomial.h
