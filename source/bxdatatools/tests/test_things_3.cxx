// test_things_3.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/archives_instantiation.h>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

// The serializable 'things' container :
#include <datatools/utils/things.h>
// The serializable 'properties' container :
#include <datatools/utils/properties.h>

#include <datatools_test_my_data.cc>
#include <datatools_test_my_data.ipp>

// the datatools writer and reader classes:
#include <datatools/serialization/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/serialization/bio_guard.h>

using namespace std;

/*** the serializable A sample class ***/

class A : public datatools::serialization::i_serializable
{
public:

	static const string SERIAL_TAG;

public:
  
  void set_value (double v)
  {
    value_ = v;
    return;
  }

	double get_value () const
	{
		return value_;
	}

  A ();

  A (double v_);

  virtual ~A (); 

  virtual const string & get_serial_tag () const;

  void dump (ostream & = clog) const;

private:
  friend class boost::serialization::access;
	BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

private :

  double value_;

};
  
const string A::SERIAL_TAG = "test_things::A";
  
template<class Archive>
void A::serialize (Archive & ar, const unsigned int file_version)
{
	ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	ar & boost::serialization::make_nvp ("value", value_);
	return;
}

void A::dump (ostream & out) const
{
  out << "A::dump : value = " << value_ << endl;
  return;
}

A::A () : value_ (0.0) 
{
	return;
}
 
A::A (double v) : value_ (v) 
{
	return;
}

A::~A ()
{
}

const string & A::get_serial_tag () const
{
  return A::SERIAL_TAG;
}

/*** serializable B  sample class ***/

class B : public datatools::serialization::i_serializable
{
public:

	static const string SERIAL_TAG;

public:
  
  void set_index (int i)
  {
    index_ = i;
    return;
  }

	int32_t get_index () const
	{
		return index_;
	}

  B () : index_ (0)
  {
		return;
  }

  B (int i) : index_ (i)
  {
		return;
  }

  virtual ~B ()
  {
  }

  void dump (ostream & = clog) const;

  virtual const string & get_serial_tag () const;

private:

  friend class boost::serialization::access;
	BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

private:

  int32_t index_;

};

template<class Archive>
void B::serialize (Archive & ar, const unsigned int file_version)
{
	ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	ar & boost::serialization::make_nvp ("index", index_);
	return;
}

const string B::SERIAL_TAG = "test_things::B";

const string & B::get_serial_tag () const
{
  return B::SERIAL_TAG;
}

void B::dump (ostream & out) const
{
  out << "B::dump : index = " << index_ << endl;
  return;
}

/***********************************************************
 * Boost/Serialization export/implement/instantiation code *
 * for the A and B classes                                 *
 ***********************************************************/

/*** use some macros to implement serialization stuff for class A ***/
BOOST_CLASS_EXPORT_KEY2 (A, "test_things::A")
BOOST_CLASS_EXPORT_IMPLEMENT (A)

/*** use some macros to implement serialization stuff for class B ***/
BOOST_CLASS_EXPORT_KEY2 (B, "test_things::B")
BOOST_CLASS_EXPORT_IMPLEMENT (B)

/*** use some macros to implement serialization stuff for class datatools::test::data_t ***/
BOOST_CLASS_EXPORT_KEY2 (datatools::test::data_t, "datatools::test::data_t")
BOOST_CLASS_EXPORT_IMPLEMENT (datatools::test::data_t)

/*** use some macros to implement serialization stuff for class datatools::test::more_data_t ***/
BOOST_CLASS_EXPORT_KEY2 (datatools::test::more_data_t, "datatools::test::more_data_t")
BOOST_CLASS_EXPORT_IMPLEMENT (datatools::test::more_data_t)


/*** main ***/
int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'datatools::utils::things' !" << endl; 
  
      bool debug = false;
			bool out   = true;
			bool in    = true;
			string format = "text";
			string compression = "";

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
							string option = token; 
							if ((option == "-d") || (option == "--debug")) 
								{
									debug = true;
								}
							else if ((option == "-O") || (option == "--no-out")) 
								{
									out = false;
								}
							else if ((option == "-I") || (option == "--no-in")) 
								{
									in = false;
								}
							else if ((option == "-x") || (option == "--xml")) 
								{
									format = "xml";
								}
							else if ((option == "-b") || (option == "--bin")) 
								{
									format = "binary";
								}
							else if ((option == "-z") || (option == "--gzip")) 
								{
									compression = ".gz";
								}
							else if ((option == "-B") || (option == "--bz2")) 
								{
									compression = ".bz2";
								}
							else 
								{ 
									clog << "warning: ignoring option '" << option << "'!" << endl; 
								}
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
				}
 
			string filename = "test_things_3.txt";
			if (format == "xml")
				{
					filename = "test_things_3.xml";
				}				
			if (format == "binary")
				{
					filename = "test_things_3.data";
				}				
			filename = filename + compression;
			
			if (out)
				{
					// declare the 'bag' instance as a 'things' container:
					datatools::utils::things bag ("bag1", "A bag with things in it");	
			
					// add some objects of type 'A' and 'B' in it
					// perform some on-the-fly setter on some of them :
					bag.add<A> ("a1").set_value (666.6666);
					bag.add<A> ("a2").set_value (3.1415);
					bag.add<B> ("b1").set_index (7654321);
					bag.add<B> ("b2").set_index (1);
					bag.add<B> ("b3").set_index (2);
					bag.add<A> ("a3").set_value (42.0);
					bag.add<datatools::utils::properties> ("p1").set_description ("A list of properties");
					bag.add<A> ("a4");
					bag.grab<B> ("b3").set_index (7777); 
					bag.grab<A> ("a4").set_value (1.6e-19); 
					bag.add<B> ("b4");
					// here we put a bag in the bag :
					bag.add<datatools::utils::things> ("g1").set_name ("sub_bag").set_description ("A bag stored in another bag");

					// fetch the 'propeties' container stored with name 'p1' :
					datatools::utils::properties & p1 = bag.grab<datatools::utils::properties> ("p1");
					p1.store_flag ("test");
					p1.store ("version.major", 1);
					p1.store ("version.minor", 2);
					p1.store ("version.patch", 10);
					p1.store ("pi", 3.14159);

					datatools::test::data_t & d1 = bag.add<datatools::test::data_t> ("d1");
					datatools::test::more_data_t & d2 = bag.add<datatools::test::more_data_t> ("d2");

					// fetch the 'things' container stored with name 'g1' :
					datatools::utils::things & g1 = bag.grab<datatools::utils::things> ("g1");
					g1.add<A> ("x1").set_value (33.0);
					g1.add<A> ("x2").set_value (12.0);
					g1.add<B> ("y1").set_index (7);
 
					// dump the bag :
					bag.dump (clog);
				
					// now we store the 'bag' contents within a Boost archive :
					clog << "Store 'things'..." << endl;
					datatools::serialization::data_writer writer (filename,
																												datatools::serialization::using_multi_archives);
					writer.store (bag);
					clog << "Done." << endl << endl << endl;
				}

			if (in)
				{
					// declare the 'bag' instance as an empty 'things' container:
					datatools::utils::things bag;

					// now we load the 'bag' from a Boost archive :
					clog << "Load 'things'..." << endl;
					datatools::serialization::data_reader reader (filename,
																												datatools::serialization::using_multi_archives);
					if (reader.has_record_tag ())
						{
							if (reader.record_tag_is (datatools::utils::things::SERIAL_TAG))
								{
									reader.load (bag);
								}
							else
								{
									cerr << "Unknown record tag !" << endl;
								}
						}
					else
						{
							cerr << "Reader has no serialized data !" << endl;
						}
					clog << "Done." << endl;

					// dump it and check that is has been properly restored
					// from the serialization stream :
					bag.dump (clog);
				
					clog << "Fetching 'p1'..." << endl;
					const datatools::utils::properties & p1 = bag.get<datatools::utils::properties> ("p1");
					p1.tree_dump (clog, "p1");
					
					clog << "Fetching 'd1'..." << endl;
					const datatools::test::data_t & d1 = bag.get<datatools::test::data_t> ("d1");	
					d1.tree_dump (clog, "d1");
					
					clog << "Fetching 'd2'..." << endl;
					const datatools::test::more_data_t & d2 = bag.get<datatools::test::more_data_t> ("d2");	
					d2.tree_dump (clog, "d2");
 
					// fetch the 'things' container stored with name 'g1' :
					datatools::utils::things & g1 = bag.grab<datatools::utils::things> ("g1");
					g1.dump (clog);
					if (g1.has ("x1") && g1.is_a<A> ("x1") )
						{
							const A & x1 = g1.get<A> ("x1");
							x1.dump (clog);
						}
					if (g1.has ("x2") && g1.is_a<A> ("x2") )
						{
							const A & x2 = g1.get<A> ("x2");
							x2.dump (clog);
						}
					if (g1.has ("y1") && g1.is_a<B> ("y1") )
						{
							const B & y1 = g1.get<B> ("y1");
							y1.dump (clog);
						}
				}

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}
 
// end of test_things_3.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
