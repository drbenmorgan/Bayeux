// -*- mode: c++; -*-
// temporary_files.h
#ifndef DATATOOLS_TEMPORARY_FILES_H_
#define DATATOOLS_TEMPORARY_FILES_H_
// Standard Library
#include <iostream>
#include <fstream>
#include <string>

namespace datatools {

/*! \brief A class for the management of temporary unique file stored in a given directory
 */
class temp_file {
 public:
  static const std::string & default_pattern();

 public:
  temp_file();

  temp_file(const char* a_pattern, bool a_remove_at_destroy = true);

  temp_file(std::string a_pattern, bool a_remove_at_destroy = true);

  temp_file(const char* a_path_dir, const char* a_pattern,
            bool a_remove_at_destroy = true);

  temp_file(std::string a_path_dir, const char* a_pattern,
            bool a_remove_at_destroy = true);

  temp_file(std::string a_path_dir, std::string a_pattern,
            bool a_remove_at_destroy = true);

  ~temp_file();

  const std::string& get_filename() const;

  bool is_valid() const;

  bool is_read_open() const;

  bool is_write_open() const;

  void set_remove_at_destroy(bool);

  void set_verbose(bool);

  std::ofstream& out();

  std::ifstream& in();

  void create(std::string a_path_dir, std::string a_pattern);

  void close();

  void remove();


 protected:
  void set_defaults();


 private:
  bool   verbose_;
  bool   remove_at_destroy_;
  std::string path_dir_;
  std::string pattern_;
  std::string full_pattern_;
  std::string filename_;
  bool   read_open_;
  bool   write_open_;
  char  *template_;
  std::ofstream out_;
  std::ifstream in_;
};

} // namespace datatools

#endif // DATATOOLS_TEMPORARY_FILES_H_
