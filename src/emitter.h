// Copyright 2009, Andreas Biegert

#ifndef SRC_EMITTER_H_
#define SRC_EMITTER_H_

#include <valarray>

#include "globals.h"
#include "context_profile-inl.h"
#include "count_profile-inl.h"
#include "sequence-inl.h"

namespace cs {

// Parameters for computation of emitter probabilities.
struct EmissionParams {
  EmissionParams()
      : ignore_context(false),
        weight_center(1.6f),
        weight_decay(0.85f) { }

  EmissionParams(const EmissionParams& p)
      : ignore_context(p.ignore_context),
        weight_center(p.weight_center),
        weight_decay(p.weight_decay) { }

  virtual ~EmissionParams() { }

  bool ignore_context;
  float weight_center;
  float weight_decay;
};

// Encapsulation for computation of emitter probabilities for profiles.
template< class Alphabet>
class Emitter {
 public:
  // Constructs a profile matcher with positional window weights.
  Emitter(int num_cols, const EmissionParams& params);

  ~Emitter() { }

  // Calculates the log emission probability of profile window centered at given
  // index.
  double operator() (const ContextProfile<Alphabet>& profile,
                     const CountProfile<Alphabet>& counts,
                     int index) const;
  // Calculates the log emission probability of sequence window centered at given
  // index.
  double operator() (const ContextProfile<Alphabet>& profile,
                     const Sequence<Alphabet>& seq,
                     int index) const;
  // Calculates the sum of positional weights.
  float sum_weights() const;
  // Initializes positional window weights.
  void init_weights();

 private:
  // Paramter wrapper
  const EmissionParams& params_;
  // Number of columns in context profiles.
  int num_cols_;
  // Index of central column in context profiles.
  int center_;
  // Positional window weights
  std::valarray<float> weights_;

  DISALLOW_COPY_AND_ASSIGN(Emitter);
};

}  // namespace cs

#endif  // SRC_EMITTER_H_