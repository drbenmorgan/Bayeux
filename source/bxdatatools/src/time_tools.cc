// time_tools.cc

// Ourselves:
#include <datatools/time_tools.h>

// Standard Library:
#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>

// This Project:
#include <datatools/clhep_units.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/exception.h>

namespace datatools {

double computing_time::system_dead_time()
{
  static double sdt = std::numeric_limits<double>::quiet_NaN();
  if (sdt != sdt) {
    sdt = compute_system_dead_time();
  }
  return sdt;
}

// Constructor:
computing_time::computing_time() {
  system_dead_time_ = system_dead_time();
  this->reset();
}

// Destructor:
computing_time::~computing_time() {}


bool computing_time::is_stopped() const {
  return ((start_.tv_sec == 0) && (start_.tv_usec == 0));
}


bool computing_time::is_started() const {
  return !this->is_stopped();
}


size_t computing_time::get_counts() const {
  return counts_;
}


double computing_time::get_last_elapsed_time() const {
  return last_elapsed_time_;
}


double computing_time::get_total_time() const {
  return this->get_sum_time() - 2 * this->get_counts()*system_dead_time_;
}


double computing_time::get_sum_time() const {
  return sum_time_;
}


double computing_time::get_sum2_time() const {
  return sum2_time_;
}


double computing_time::get_min_time() const {
  return min_time_;
}


double computing_time::get_max_time() const {
  return max_time_;
}


double computing_time::get_mean_time() const {
  if (counts_ == 0) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  return sum_time_ / counts_;
}


double computing_time::get_sigma_time() const {
  if (counts_ < 2) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  double m = this->get_mean_time();
  double m2 = sum2_time_ / counts_;
  return sqrt(m2 - (m * m));
}


void computing_time::start() {
  this->resume();
}


void computing_time::stop() {
  this->pause();
  counts_++;
}


void computing_time::pause() {
  DT_THROW_IF((gettimeofday(&stop_, NULL)) == -1,
              std::runtime_error,
              "gettimeofday failed !");
  double elapsed_time = 0.0;
  timeval diff;
  g_timeval_subtract(stop_, start_, diff);
  elapsed_time = diff.tv_sec * CLHEP::second
      + diff.tv_usec * CLHEP::microsecond;
  sum_time_ += elapsed_time;
  sum2_time_ += (elapsed_time * elapsed_time);

  if (!datatools::is_valid(min_time_)) {
    min_time_ = elapsed_time;
  } else if (elapsed_time < min_time_) {
    min_time_ = elapsed_time;
  }

  if (!datatools::is_valid(max_time_)) {
    max_time_ = elapsed_time;
  } else if (elapsed_time > max_time_) {
    max_time_ = elapsed_time;
  }

  last_elapsed_time_ = elapsed_time;
  start_.tv_sec = 0;
  start_.tv_usec = 0;
  stop_.tv_sec = 0;
  stop_.tv_usec = 0;
}


void computing_time::resume() {
  DT_THROW_IF((gettimeofday(&start_, NULL)) == -1,
              std::runtime_error,
              "gettimeofday failed !");
}


void computing_time::reset() {
  start_.tv_sec = 0;
  start_.tv_usec = 0;
  stop_.tv_sec = 0;
  stop_.tv_usec = 0;
  counts_ = 0;
  sum_time_ = 0.0;
  sum2_time_ = 0.0;
  datatools::invalidate(min_time_);
  datatools::invalidate(max_time_);
  datatools::invalidate(last_elapsed_time_);
}


void computing_time::tree_dump(std::ostream& a_out,
                               const std::string& a_title,
                               const std::string& a_indent,
                               bool a_inherit) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  a_out << indent << i_tree_dumpable::tag
        << "System dead time   : "
        << system_dead_time_ / CLHEP::microsecond << " us" << std::endl;

  if (counts_ == 0) {
    a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
          << "No statistics" << std::endl;
  } else {
    a_out << indent << i_tree_dumpable::tag
          << "Count(s)      : " << counts_ << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Sum time      : "
          << sum_time_ / CLHEP::second << " s" << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Min time      : "
          << min_time_ / CLHEP::second << " s" << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Max time      : "
          << max_time_ / CLHEP::second << " s" << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Sum squ. time : "
          << sum2_time_ / (CLHEP::second * CLHEP::second)
          << " s²" << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Total time    : "
          << this->get_total_time() / CLHEP::second << " s" << std::endl;
    a_out << indent << i_tree_dumpable::tag
          << "Mean time     : "
          << this->get_mean_time() / CLHEP::second << " s" << std::endl;
    a_out << indent <<  i_tree_dumpable::tag
          << "Sigma time    : ";
    if (std::isnan(this->get_sigma_time())) {
      a_out << '-';
    } else {
      a_out << this->get_sigma_time() / CLHEP::second << " s";
    }

    a_out << std::endl;
    a_out << indent <<  i_tree_dumpable::inherit_tag(a_inherit)
          << "Last elapsed time : "
          << last_elapsed_time_ / CLHEP::second << " s" << std::endl;
  }
}


bool computing_time::g_timeval_subtract(const timeval& a_start,
                                        const timeval& a_stop,
                                        timeval& a_result) {
  timeval x = a_start;
  timeval y = a_stop;
  /* Perform the carry for the later subtraction by updating y. */
  if (x.tv_usec < y.tv_usec) {
    int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
    y.tv_usec -= 1000000 * nsec;
    y.tv_sec += nsec;
  }
  if (x.tv_usec - y.tv_usec > 1000000) {
    int nsec = (x.tv_usec - y.tv_usec) / 1000000;
    y.tv_usec += 1000000 * nsec;
    y.tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  a_result.tv_sec = x.tv_sec - y.tv_sec;
  a_result.tv_usec = x.tv_usec - y.tv_usec;

  /* Return 1 if result is negative. */
  if (x.tv_sec < y.tv_sec) {
    return false;
  }
  return true;
}


double computing_time::compute_system_dead_time() {
  double system_dead_time = 0.0 * CLHEP::second;
  timeval start, stop;
  gettimeofday(&start, NULL);
  size_t n = 50;

  for (size_t i = 0; i < (n - 1); i++) {
    gettimeofday(&stop, NULL);
  }

  double elapsed_time = 0.0;
  timeval diff;
  g_timeval_subtract(stop, start, diff);
  elapsed_time = diff.tv_sec * CLHEP::second
      + diff.tv_usec * CLHEP::microsecond;
  system_dead_time = elapsed_time / n;
  return system_dead_time;
}

} // namespace datatools