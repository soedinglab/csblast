#ifndef CS_COUNTS_PROFILE_H
#define CS_COUNTS_PROFILE_H
/***************************************************************************
 *   Copyright (C) 2008 by Andreas Biegert                                 *
 *   andreas.biegert@googlemail.com                                        *
 ***************************************************************************/

// DESCRIPTION:
// A container class for profiles derived from alignments.

#include <iostream>
#include <vector>

#include "alignment.h"
#include "profile.h"
#include "shared_ptr.h"

namespace cs
{

// Forward declarations
class SequenceAlphabet;

class CountsProfile : public Profile
{
  public:
    // Constructs profile from serialized profile read from input stream.
    CountsProfile(std::istream& in, const SequenceAlphabet* alphabet);
    // Constructs a profile of the given sequence.
    explicit CountsProfile(const Sequence& sequence);
    // Constructs a profile of the given alignment with specified sequence weighting method.
    explicit CountsProfile(const Alignment& alignment, bool position_specific_weights = true);
    // Creates a profile from subprofile in other, starting at column index and length columns long.
    CountsProfile(const CountsProfile& other, int index, int length);

    virtual ~CountsProfile() {}

    // Reads all available profiles from the input stream and returns them in a vector.
    static std::vector< shared_ptr<CountsProfile> > readall(std::istream& in,
                                                          const SequenceAlphabet* alphabet);
    // Returns the number of effective sequences in alignment column i
    float neff(int i) const { return neff_[i]; }
    // Converts the profile to counts of alphabet letters.
    void convert_to_counts();
    // Converts the profile back to relative frequencies of alphabet letters.
    void convert_to_frequencies();
    // Returns true if the profile contains counts.
    bool has_counts() const { return has_counts_; }

  protected:
    // Reads and initializes serialized scalar data members from stream.
    virtual void read_header(std::istream& in);
    // Reads and initializes array data members from stream.
    virtual void read_body(std::istream& in);
    // Writes serialized scalar data members to stream.
    virtual void write_header(std::ostream& out) const;
    // Writes serialized array data members to stream.
    virtual void write_body(std::ostream& out) const;
    // Prints the profile in human-readable format to output stream.
    virtual void print(std::ostream& out) const;

  private:
    // Disallow copy and assign
    CountsProfile(const CountsProfile&);
    void operator=(const CountsProfile&);

    // Return serialization class identity.
    virtual const std::string class_identity() const { static std::string id("CountsProfile"); return id;}

    // Flag indicating if the profile contains counts or (relative) frequencies.
    bool has_counts_;
    // Number of effective sequences in each alignment column.
    std::vector<float> neff_;
};//CountsProfile

}//cs

#endif
