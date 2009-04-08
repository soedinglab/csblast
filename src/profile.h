// Copyright 2009, Andreas Biegert

#ifndef SRC_PROFILE_H_
#define SRC_PROFILE_H_

#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <vector>

#include "exception.h"
#include "log.h"
#include "matrix.h"
#include "shared_ptr.h"
#include "utils.h"

namespace cs {

// A profile class representing columns of frequencies over a sequence alphabet.
template<class Alphabet>
class Profile {
 public:
  typedef matrix<float>::row_type col_type;
  typedef matrix<float>::const_row_type const_col_type;
  typedef matrix<float>::iterator iterator;
  typedef matrix<float>::const_iterator const_iterator;

  // Constructs a dummy profile.
  Profile();
  // Constructs a profile with num_cols columns initialized to zero.
  explicit Profile(int num_cols);
  // Constructs profile from serialized profile read from input stream.
  explicit Profile(std::istream& in);
  // Constructs profile from serialized profile read from input stream.
  explicit Profile(FILE* fin);
  // Creates a profile from subprofile starting at column index and length
  // columns long.
  Profile(const Profile& other, int index, int length);

  virtual ~Profile() { }

  // Reads all available profiles from the input stream and returns them in a
  // vector.
  static void readall(std::istream& in, std::vector< shared_ptr<Profile> >& v);
  // Reads all available profiles from the input stream and returns them in a
  // vector.
  static void readall(FILE* in, std::vector< shared_ptr<Profile> >* v);

  // Access methods to get the (i,j) element
  col_type operator[](int i) { return data_[i]; }
  const_col_type operator[](int i) const { return data_[i]; }
  // Returns #columns in the profile
  int num_cols() const { return data_.num_rows(); }
  // Returns #columns in the profile
  int length() const { return data_.num_rows(); }
  // Returns #entries per column
  int alphabet_size() const { return data_.num_cols(); }
  // Returns the total number of elements in the profile.
  int size() const { return data_.size(); }
  // Transforms profile to logspace
  virtual void transform_to_logspace();
  // Transforms profile to linspace
  virtual void transform_to_linspace();
  // Returns true if the profile is in logspace
  bool logspace() const { return logspace_; }
  // Returns an iterator to the first element in profile column i.
  col_type col_begin(int i) { return data_.row_begin(i); }
  // Returns an iterator just past the end of profile column i.
  col_type col_end(int i) { return data_.row_end(i); }
  // Returns a const iterator to the first element in profile column i.
  const_col_type col_begin(int i) const { return data_.row_begin(i); }
  // Returns a const iterator just past the end of profile column i.
  const_col_type col_end(int i) const { return data_.row_end(i); }
  // Returns an iterator to the first element in the profile matrix.
  iterator begin() { return data_.begin(); }
  // Returns an iterator just past the end of the profile matrix.
  iterator end() { return data_.end(); }
  // Returns a const iterator to the first element in the profile matrix.
  const_iterator begin() const { return data_.begin(); }
  // Returns a const iterator just past the end of the profile matrix.
  const_iterator end() const { return data_.end(); }
  // Initializes the profile object with a serialized profile read from stream.
  void read(std::istream& in);
  // Initializes the profile object with a serialized profile read from stream.
  void read(FILE* fin);
  // Writes the profile in serialization format to output stream.
  void write(std::ostream& out) const;

  // Prints profile in human-readable format for debugging.
  friend std::ostream& operator<< (std::ostream& out, const Profile& p) {
    p.print(out);
    return out;
  }

 protected:
  // Scaling factor for serialization of profile log values
  static const int SCALE_FACTOR = 1000;
  static const int LOG_SCALE = 1000;
  // Buffer size for reading
  static const int BUFFER_SIZE = 1024;

  // Reads and initializes serialized scalar data members from stream.
  virtual void read_header(std::istream& in);
  // Reads and initializes serialized scalar data members from stream.
  virtual void read_header(FILE* fin);
  // Reads and initializes array data members from stream.
  virtual void read_body(std::istream& in);
  // Reads and initializes array data members from stream.
  virtual void read_body(FILE* fin);
  // Writes serialized scalar data members to stream.
  virtual void write_header(std::ostream& out) const;
  // Writes serialized scalar data members to stream.
  virtual void write_header(FILE* fout) const;
  // Writes serialized array data members to stream.
  virtual void write_body(std::ostream& out) const;
  // Prints the profile in human-readable format to output stream.
  virtual void print(std::ostream& out) const;
  // Resize the profile matrix to given dimensions.
  void resize(int num_cols, int alphabet_size);

  // Profile matrix in row major format
  matrix<float> data_;
  // Flag indicating if profile is in log- or linspace
  bool logspace_;

 private:
  // Class identifier
  static const char* CLASS_ID;

  // Returns serialization class identity.
  virtual const std::string class_identity() const {
    static std::string id("Profile");
    return id;
  }
  virtual const char* class_id() const { return CLASS_ID; }

};  // Profile

// Resets all entries in given profile to the given value or zero if none is given.
template<class Alphabet>
void reset(Profile<Alphabet>* p, float value = 0.0f);

// Normalize profile columns to value or to one if none provided.
template<class Alphabet>
bool normalize(Profile<Alphabet>* p, float value = 1.0f);

}  // cs

#endif  // SRC_PROFILE_H_
