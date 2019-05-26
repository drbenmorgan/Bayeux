/// \file datatools/bit_utils.h
/// \brief Utilities for manipulating bits of integral types

#ifndef DATATOOLS_BIT_UTILS_H
#define DATATOOLS_BIT_UTILS_H

// This project
#include <datatools/exception.h>

namespace datatools {

  /* Bit manipulation utility template functions */

  /// Set the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  void set_bit(Integral & number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");
    number_ |= 1 << pos_;
    return;
  }

  /// Unset the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  void unset_bit(Integral & number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");
    number_ &= ~(1 << pos_);
    return;
  }


  /// Toggle the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  void toggle_bit(Integral & number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");
    number_ ^= 1 << pos_;
    return;
  }


  /// Check the bit at a given position in an integral number
  /// @arg number the integral number
  /// @arg pos the bit index
  template<typename Integral>
  bool check_bit(Integral& number_, int pos_)
  {
    DT_THROW_IF(pos_ < 0 || pos_ >= (int) sizeof(Integral) * 8,
                std::range_error,
                "Invalid bit position [pos = " << pos_ << "]");

    if (number_ & (1 << pos_)) return false;
    return true;
  }

} // namespace datatools

#endif // DATATOOLS_BIT_UTILS_H
