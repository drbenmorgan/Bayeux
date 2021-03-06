// -*- mode: c++; -*-
// test_utils.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>

#include <datatools/io_factory.h>
#include <mygsl/rng.h>


void test1()
{
  std::clog << std::endl << "*** test1 : Serialization..." << std::endl;

  int32_t seed = 314159;
  mygsl::rng random("taus2", seed);

  geomtools::vector_3d pos;
  pos.set (1., 2., 3.);
  std::clog << "Vector: " << pos << std::endl;

  geomtools::rotation rot;
  geomtools::create_rotation(rot, 0.0, 90. * CLHEP::degree, 0.0);
  geomtools::tree_dump (rot, std::clog, "Rotation matrix:");

  geomtools::vector_3d origin;
  origin.set (0., 0., 0.);
  geomtools::vector_3d dir;
  dir.set (random.uniform(), random.uniform(), random.uniform());
  std::clog << "Direction: " << dir << std::endl;

  geomtools::gnuplot_draw::basic_draw_point (std::cout, origin);
  geomtools::gnuplot_draw::basic_draw_point (std::cout, dir);
  std::cout << std::endl;
  std::cout << std::endl;


  for (int i = 0; i < 16; i++)
    {
      geomtools::vector_3d orth_dir;
      geomtools::randomize_orthogonal_direction (random, dir, orth_dir);
      std::clog << "Random orthogonal direction: " << orth_dir << std::endl;

      geomtools::gnuplot_draw::basic_draw_point (std::cout, origin);
      geomtools::gnuplot_draw::basic_draw_point (std::cout, orth_dir);
      if (i == 0) // trick
        geomtools::gnuplot_draw::basic_draw_point (std::cout, origin);
      std::cout << std::endl;
    }
  std::cout << std::endl;

  {
    std::cout << "Serialize..." << std::endl;
    datatools::data_writer writer;
    writer.init ("test_utils.txt",
                 datatools::using_multi_archives);

    for (int i = 0; i < 5; i++)
      {
        geomtools::vector_3d v (random.uniform(),
                                random.uniform(),
                                random.uniform());
        if (i == 3) geomtools::invalidate (v);
        std::clog << "v[" << i << "] = " << v << std::endl;
        writer.store (geomtools::io::vector_3d_serial_tag(), v);
      }
    writer.reset ();
    std::cout << "Done" << std::endl << std::endl;
  }

  {
    std::cout << "Deserialize..." << std::endl;
    datatools::data_reader reader;
    reader.init ("test_utils.txt",
                 datatools::using_multi_archives);
    int i = 0;
    while (reader.has_record_tag ())
      {
        if (reader.get_record_tag () == geomtools::io::vector_3d_serial_tag())
          {
            geomtools::vector_3d v;
            reader.load (geomtools::io::vector_3d_serial_tag(), v);
            std::clog << "v[" << i << "] = " << v << std::endl;
          }
        i++;
      }
    reader.reset ();
    std::cout << "Done" << std::endl << std::endl;
  }

  {
    std::cout << "Serialize (2)..." << std::endl;
    datatools::data_writer writer;
    writer.init ("test_utils2.txt",
                 datatools::using_multi_archives);

    for (int i = 0; i < 4; i++)
      {
        geomtools::rotation r;
        if (i == 3)
          {
            geomtools::invalidate (r);
          }
        else
          {
            geomtools::create_rotation (r,
                                        2. * M_PI * random(),
                                        0.25 * M_PI * random(),
                                        0.0);
          }
        std::clog << "r[" << i << "] = " << r << std::endl;
        geomtools::tree_dump (r,
                              std::clog,
                              "Rotation matrix (saved):");
        writer.store (geomtools::io::rotation_3d_serial_tag(), r);
      }
    writer.reset ();
    std::cout << "Done" << std::endl << std::endl;
  }

  {
    std::cout << "Deserialize (2)..." << std::endl;
    datatools::data_reader reader;
    reader.init ("test_utils2.txt",
                 datatools::using_multi_archives);
    int i = 0;
    while (reader.has_record_tag ())
      {
        if (reader.get_record_tag () == geomtools::io::rotation_3d_serial_tag())
          {
            geomtools::rotation r;
            reader.load (geomtools::io::rotation_3d_serial_tag(), r);
            geomtools::tree_dump (r,
                                  std::clog,
                                  "Rotation matrix (loaded):");
            std::clog << std::endl;
          }
        i++;
      }
    reader.reset ();
    std::cout << "Done" << std::endl << std::endl;
  }

  geomtools::create_rotation(rot,
                             30.0 * CLHEP::degree,
                             45. * CLHEP::degree,
                             0.0);
  geomtools::tree_dump (rot, std::clog, "Rotation matrix (valid):");
  geomtools::invalidate (rot);
  geomtools::tree_dump (rot, std::clog, "Rotation matrix (invalid):");

  return;
}

int
main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
    {

      test1();

      std::cerr << "The end." << std::endl;
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what() << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}
