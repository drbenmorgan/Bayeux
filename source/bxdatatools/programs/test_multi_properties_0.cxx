// -*- mode: c++; -*- 
// test_multi_properties_0.cxx 

#include <datatools/utils/multi_properties.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  {
    // Declare a *bag of bags*:
    datatools::utils::multi_properties my_bag_of_bags ("name", "type", "A sample list of setups");

    // Add a new bag (*properties*) named 'display' within the bag of bags:
    my_bag_of_bags.add ("display", "display_type");
    // Retrieve a hook to the bag hosted by our *bag of bags*:
    datatools::utils::multi_properties::entry & e1 = my_bag_of_bags.get ("display");
    // Add properties within this bag:
    e1.get_properties ().store_flag ("debug");
    e1.get_properties ().store ("colour", "blue");
    e1.get_properties ().store ("depth", 1.234);
	
    // Add a new 'water' bag;
    my_bag_of_bags.add ("water", "material_type");
    datatools::utils::multi_properties::entry & e2 = my_bag_of_bags.get ("water");
    e2.get_properties ().store_flag ("liquid");
    e2.get_properties ().store ("density", 1.0);
    e2.get_properties ().store ("temperature", 300.0);
    e2.get_properties ().store ("pressure", 1.0);
    e2.get_properties ().store ("temperature.unit", "kelvin");
    e2.get_properties ().store ("pressure.unit", "bar");
	 
    // Add a new 'particle' bag;
    my_bag_of_bags.add ("shape", "shape_type");
    datatools::utils::multi_properties::entry & e3 = my_bag_of_bags.get ("shape");
    e3.get_properties ().store_flag ("visible");
    e3.get_properties ().store ("type", "box");
    e3.get_properties ().store ("material", "water");
    e3.get_properties ().store ("length", 0.12);
    e3.get_properties ().store ("width", 0.04);
    e3.get_properties ().store ("depth", 0.06);

    // Remove a bag from the bag of bags;
    my_bag_of_bags.remove ("display");

    // Save in ASCII file: 
    my_bag_of_bags.write ("my_bag_of_bags.conf", 
			  datatools::utils::multi_properties::with_header_footer,
			  datatools::utils::multi_properties::write_private_also);  
  }
  
  {
    // Load from ASCII file:
    datatools::utils::multi_properties my_bag_of_bags ("name", "type");
    my_bag_of_bags.read ("my_bag_of_bags.conf", 
			 datatools::utils::multi_properties::read_public_only);  

    // Print:
    my_bag_of_bags.dump (clog);
  }
 
  return 0;
}

// end of test_multi_properties_0.cxx
