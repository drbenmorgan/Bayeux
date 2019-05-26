// string_utils.cc

// Ourselves
#include <datatools/string_utils.h>

// This project
#include <datatools/exception.h>

namespace datatools {

  const std::string & empty_string()
  {
    static const std::string _e;
    return _e;
  }

  const std::string & none_label()
  {
    static const std::string _l("none");
    return _l;
  }

  const std::string & yes_label()
    {
    static const std::string _l("yes");
    return _l;
  }

  const std::string & no_label()
  {
    static const std::string _l("no");
    return _l;
  }

  // const boost::property_tree::ptree & empty_options()
  // {
  //   static const boost::property_tree::ptree _empty;
  //   return _empty;
  // }

  /* String utility functions */

  bool name_validation(const std::string & name_, uint32_t flags_ )
  {
    static const std::string _base_allowed_chars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.:-";
    if (name_.empty()) return false;
    bool forbid_colon  = flags_ & NV_NO_COLON;
    bool forbid_dot    = flags_ & NV_NO_DOT;
    bool forbid_hyphen = flags_ & NV_NO_HYPHEN;
    bool forbid_underscore = flags_ & NV_NO_UNDERSCORE;
    bool allow_leading_digit = flags_ & NV_LEADING_DIGIT;
    if (forbid_colon) {
      if (name_.find(':') != name_.npos) return false;
    }
    if (forbid_dot) {
      if (name_.find('.') != name_.npos) return false;
    }
    if (forbid_hyphen) {
      if (name_.find('-') != name_.npos) return false;
    }
    if (forbid_underscore) {
      if (name_.find('_') != name_.npos) return false;
    }
    if (name_.find_first_not_of(_base_allowed_chars) != name_.npos) return false;
    if (!allow_leading_digit) {
      if (name_.find_first_of("0123456789.") == 0) return false;
    }
    // Reject trailing dot:
    if (name_[name_.size()-1] == '.') return false;
    // Reject trailing colon:
    if (name_[name_.size()-1] == ':') return false;
    // Reject trailing hyphen:
    if (name_[name_.size()-1] == '-') return false;
    // // Colon must be followed by one other colon (example: "::")
    // unsigned int colon_first = name_.find(':');
    // if (colon_first != name_.npos) {
    //   if (name_[colon_first + 1] != ':') return false;
    // }
    return true;
  }

  bool is_quoted(const std::string & text_, char q_)
  {
    DT_THROW_IF(q_ != '\'' && q_ != '"',
                std::range_error,
                "Unsupported quoting character '" << q_ << "'!");
    char c = q_;
    if (text_.size() > 1) {
      return (text_[0] == c) && (text_[text_.size() - 1] == c);
    }
    return false;
  }

  void add_quotes(const std::string & from_, std::string & to_, char q_)
  {
    DT_THROW_IF(q_ != '\'' && q_ != '"',
                std::range_error,
                "Unsupported quoting character '" << q_ << "'!");
    if (! is_quoted(from_, q_)) {
      std::ostringstream oss;
      oss << q_ << from_ << q_;
      to_ = oss.str();
    } else {
      to_ = from_;
    }
    return;
  }

  void add_quotes(std::string & text_, char q_)
  {
    std::string t;
    add_quotes(text_, t, q_);
    text_ = t;
    return;
  }

  void remove_all_quotes(std::string & text_)
  {
    if (is_quoted(text_, '"')) {
      remove_quotes(text_, '"');
    } else if (is_quoted(text_, '\'')) {
      remove_quotes(text_, '\'');
    }
    return;
  }

  void remove_quotes(const std::string & from_, std::string & to_, char q_)
  {
    DT_THROW_IF(q_ != '\'' && q_ != '"',
                std::range_error,
                "Unsupported quoting character '" << q_ << "'!");
    if (is_quoted(from_, q_)) {
      to_ = from_.substr(1,from_.length()-2);
    } else {
      to_ = from_;
    }
    return;
  }

  void remove_quotes(std::string & text_, char q_)
  {
    std::string t;
    remove_quotes(text_, t, q_);
    text_ = t;
    return;
  }

  /* From:
     Data Structures in C++ Using the STL
     by Timothy A. Budd
     published by Addison Wesley Longman
     ISBN 0-201-31659-5
     http://web.engr.oregonstate.edu/~budd/Books/stl/info/ReadMe.html
  */
  void split_string(const std::string& text, const std::string& separators,
                    std::list<std::string>& words) {
    // split a string into a list of words
    // 'text_' and 'separators_' are input,
    // list of 'words_' is output
    int text_len = text.length();

    // find first non-separator character
    int start = text.find_first_not_of(separators, 0);

    // loop as long as we have a non-separator character
    while ((start >= 0) && (start < text_len)) {
      // find end of current word
      int stop = text.find_first_of(separators, start);

      if ((stop < 0) || (stop > text_len)) stop = text_len;

      // add word to list of words
      words.push_back(text.substr(start, stop - start));

      // find start of next word
      start = text.find_first_not_of(separators, stop+1);
    }
  }

} // namespace datatools

