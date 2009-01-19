#ifndef CS_CONTEXT_PROFILE_H
#define CS_CONTEXT_PROFILE_H
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
#include "smart_ptr.h"

namespace cs
{

// Forward declarations
class SequenceAlphabet;

class ContextProfile : public Profile
{
  public:
    // TODO
    ContextProfile(const Profile&);
    // Creates a profile from subprofile in other, starting at column index and length columns long.
    ContextProfile(const Profile& other, int index, int length);
    // Constructs profile from serialized profile read from input stream.
    ContextProfile(std::istream& in, const SequenceAlphabet* alphabet);

    virtual ~ContextProfile() {}

    // Reads all available profiles from the input stream and returns them in a vector.
    static std::vector< SmartPtr<ContextProfile> > read(std::istream& in,
                                                        const SequenceAlphabet* alphabet);


    // Access methods to get the (i,j) element
    float&       operator() (int i, int j) { return data_[i*ndim_ + j]; }
    const float& operator() (int i, int j) const { return data_[i*ndim_ + j]; }

    // Access methods to get the element j of central column
    float&       operator() (int j) { return data_[central_ * ndim_ + j]; }
    const float& operator() (int j) const { return data_[central_ * ndim_ + j]; }

  protected:
    // Initializes the profile object with a serialized profile read from stream.
    virtual void unserialize(std::istream& in);
    // Prints the profile in serialization format to output stream.
    virtual void serialize(std::ostream& out) const;

  private:
    // Disallow copy and assign
    ContextProfile(const ContextProfile&);
    void operator=(const ContextProfile&);

    // Index of central column
    int center_;
};  // ContextProfile

}  // cs

#endif