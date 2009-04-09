// Copyright 2009, Andreas Biegert

#ifndef SRC_PROFILE_LIBRARY_INL_H_
#define SRC_PROFILE_LIBRARY_INL_H_

#include "profile_library.h"

#include <string>

namespace cs {

template<class Alphabet>
ProfileLibrary<Alphabet>::ProfileLibrary(int num_profiles, int num_cols)
    : num_profiles_(num_profiles),
      num_cols_(num_cols),
      iterations_(0),
      profiles_(),
      logspace_(false) {
  profiles_.reserve(num_profiles);
}

template<class Alphabet>
ProfileLibrary<Alphabet>::ProfileLibrary(std::istream& in)
    : num_profiles_(0),
      num_cols_(0),
      iterations_(0),
      profiles_(),
      logspace_(false) {
  read(in);
}

template<class Alphabet>
ProfileLibrary<Alphabet>::ProfileLibrary(
    int num_profiles,
    int num_cols,
    const ProfileInitializer<Alphabet>& profile_init)
    : num_profiles_(num_profiles),
      num_cols_(num_cols),
      iterations_(0),
      profiles_(),
      logspace_(false) {
  profiles_.reserve(num_profiles);
  profile_init.init(*this);
}

template<class Alphabet>
inline void ProfileLibrary<Alphabet>::clear() {
  profiles_.clear();
  profiles_.reserve(num_profiles());
}

template<class Alphabet>
inline int ProfileLibrary<Alphabet>::add_profile(
    const Profile<Alphabet>& profile) {
  if (full())
    throw Exception("Profile library contains already %i profiles!",
                    num_profiles());
  if (profile.num_cols() != num_cols())
    throw Exception("Profile to add as state has %i columns but should have %i!",
                    profile.num_cols(), num_cols());

  shared_ptr< ContextProfile<Alphabet> >
    profile_ptr(new ContextProfile<Alphabet>(profiles_.size(), profile));
  profile_ptr->set_prior(1.0f / num_profiles());

  profiles_.push_back(profile_ptr);
  return profiles_.size() - 1;
}

template<class Alphabet>
inline void ProfileLibrary<Alphabet>::transform_to_logspace() {
  if (!logspace()) {
    for (profile_iterator pi = profiles_.begin(); pi != profiles_.end(); ++pi)
      (*pi)->transform_to_logspace();
    logspace_ = true;
  }
}

template<class Alphabet>
inline void ProfileLibrary<Alphabet>::transform_to_linspace() {
  if (logspace()) {
    for (profile_iterator pi = profiles_.begin(); pi != profiles_.end(); ++pi)
      (*pi)->transform_to_linspace();
    logspace_ = false;
  }
}

template<class Alphabet>
void ProfileLibrary<Alphabet>::read(std::istream& in) {
  LOG(DEBUG1) << "Reading profile library from stream ...";

  // Check if stream actually contains a serialized profile library
  std::string tmp;
  while (getline(in, tmp) && tmp.empty()) continue;
  if (tmp.find("ProfileLibrary") == std::string::npos)
    throw Exception("Profile library does not start with 'ProfileLibrary' keyword!");
  // Read number of profiles
  if (getline(in, tmp) && tmp.find("num_profiles") != std::string::npos)
    num_profiles_ = atoi(tmp.c_str() + 12);
  else
    throw Exception("Profile library does not contain 'num_profiles' record!");
  // Read number of columns
  if (getline(in, tmp) && tmp.find("num_cols") != std::string::npos)
    num_cols_= atoi(tmp.c_str() + 8);
  else
    throw Exception("Profile library does not contain 'num_cols' record!");
  // Read number of iterations
  if (getline(in, tmp) && tmp.find("iterations") != std::string::npos)
    iterations_= atoi(tmp.c_str() + 10);
  else
    throw Exception("Profile library does not contain 'num_cols' record!");
  // Read profiles_logspace
  if (getline(in, tmp) && tmp.find("logspace") != std::string::npos)
    logspace_ = atoi(tmp.c_str() + 8) == 1;

  // Read profile records
  profiles_.reserve(num_profiles());
  while (!full() && in.good()) {
    shared_ptr< ContextProfile<Alphabet> >
      profile_ptr(new ContextProfile<Alphabet>(in));
    profiles_.push_back(profile_ptr);
  }
  if (!full())
    throw Exception("Profile library has %i profiles but should have %i!",
                    profiles_.size(), num_profiles());

  LOG(DEBUG1) << *this;
}

template<class Alphabet>
void ProfileLibrary<Alphabet>::write(std::ostream& out) const {
  // Write header
  out << "ProfileLibrary"   << std::endl;
  out << "num_profiles\t\t" << num_profiles() << std::endl;
  out << "num_cols\t\t"     << num_cols() << std::endl;
  out << "iterations\t\t"   << iterations() << std::endl;
  out << "logspace\t\t"     << (logspace() ? 1 : 0) << std::endl;

  // Serialize profiles
  for (const_profile_iterator pi = profiles_.begin(); pi != profiles_.end(); ++pi)
    (*pi)->write(out);
}

template<class Alphabet>
void ProfileLibrary<Alphabet>::print(std::ostream& out) const {
  out << "ProfileLibrary" << std::endl;
  out << "Total number of profiles: " << num_profiles() << std::endl;
  out << "Context profile columns:  " << num_cols() << std::endl;
  out << "Clustering iterations:    " << iterations() << std::endl;

  for (const_profile_iterator pi = profiles_.begin(); pi != profiles_.end(); ++pi)
    out << **pi;
}


template<class Alphabet>
void SamplingProfileInitializer<Alphabet>::init(
    ProfileLibrary<Alphabet>& lib) const {
  LOG(DEBUG) << "Initializing profile library with " << lib.num_profiles()
             << " profile windows randomly sampled from "
             << profiles_.size() << " training profiles ...";

  // Iterate over randomly shuffled profiles and add them to the library until
  // the library is full.
  for (profile_iterator pi = profiles_.begin();
       pi != profiles_.end() && !lib.full(); ++pi) {
    if (lib.num_cols() != (*pi)->num_cols())
      throw Exception("Library has num_cols=%i but training profile has %i!",
                      lib.num_cols(), (*pi)->num_cols());

    CountProfile<Alphabet> p(**pi);
    p.convert_to_frequencies();
    if (pc_) pc_->add_to_profile(ConstantAdmixture(pc_admixture_), &p);
    lib.add_profile(p);
  }
  if (!lib.full())
    throw Exception("Could not fully initialize all %i library profiles. "
                    "Maybe too few training profiles provided?",
                    lib.num_profiles());

  LOG(DEBUG) << "Profile library after profile initialization:";
  LOG(DEBUG) << lib;
}

}  // namespace cs

#endif  // SRC_PROFILE_LIBRARY_INL_H_