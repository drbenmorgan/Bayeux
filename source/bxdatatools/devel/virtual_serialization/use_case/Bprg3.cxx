//

#include <iostream>
#include <fstream>
#include <exception>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <Ac1.hpp>
#include <Ac2.hpp>
#include <Bc3.hpp>

int main (void)
{
  try
    {
      const size_t NOBJS = 6;
      {
	A::base * objects[NOBJS];
	objects[0] = dynamic_cast <A::base *> (new B::c3 (11));
	objects[1] = dynamic_cast <A::base *> (new B::c3 (22));
	objects[2] = dynamic_cast <A::base *> (new B::c3 (33));
	objects[3] = dynamic_cast <A::base *> (new B::c3 (44));
	objects[4] = dynamic_cast <A::base *> (new B::c3 (55));
	objects[5] = dynamic_cast <A::base *> (new B::c3 (66));
	for (int i = 0; i < NOBJS; i++)
	  {
	    objects[i]->print ();
	  }
	{
	  std::clog << "Storing..." << std::endl;
	  std::ofstream tof ("Bprg3.txt");
	  boost::archive::text_oarchive toa (tof);
	  toa & objects;
	}
	for (int i = 0; i < NOBJS; i++)
	  {
	    delete objects[i];
	    objects[i] = 0;
	  }
      }

      {
	A::base * objects[NOBJS];
	{
	  std::clog << "Loading..." << std::endl;
	  std::ifstream tif ("Bprg3.txt");
	  boost::archive::text_iarchive tia (tif);
	  tia & objects;
	}
	for (int i = 0; i < NOBJS; i++)
	  {
	    objects[i]->print ();
	  }
	for (int i = 0; i < NOBJS; i++)
	  {
	    delete objects[i];
	    objects[i] = 0;
	  }
      }

    }
  catch (std::exception &x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}
