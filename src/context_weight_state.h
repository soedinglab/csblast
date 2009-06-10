// Copyright 2009, Andreas Biegert

#ifndef SRC_CONTEXT_WEIGHT_STATE_H_
#define SRC_CONTEXT_WEIGHT_STATE_H_

#include <cstdio>
#include <cstdlib>

#include <google/sparsetable>
#include <iostream>
#include <vector>

#include "globals.h"
#include "matrix.h"
#include "profile.h"
#include "shared_ptr.h"
#include "transition.h"

using google::sparsetable;

namespace cs {

// Forward declarations
template< class Alphabet, template<class> class State >
class ChainGraph;

// A class encapsulating context weights, pseudocount parameters, and transition
// parameters of a CRF state.
template<class Alphabet>
class ContextWeightState {
 public:
  typedef matrix<float>::row_type ColType;
  typedef matrix<float>::const_row_type ConstColType;
  typedef matrix<float>::iterator WeightIter;
  typedef matrix<float>::const_iterator ConstWeightIter;
  typedef sparsetable<AnchoredTransition> TransitionTable;
  typedef typename TransitionTable::const_nonempty_iterator ConstTransitionIter;

  // Constructs state with given index, number of states, and number of columns.
  ContextWeightState(int index, int num_states, int num_cols);
  // Constructs state with given index with parameters initialized from
  // given profile.
  ContextWeightState(int index, int num_states, const Profile<Alphabet>& profile);
  // Constructs a state from serialized data read from input stream.
  explicit ContextWeightState(FILE* fin);

  ~ContextWeightState() {}

  // Returns the index of this state.
  int index() const { return index_; }
  // Sets the index of this state.
  void set_index(int i) { index_ = i; }
  // Access methods to get the context weight for letter j in column i
  ColType operator[] (int i) { return weights_[i]; }
  ConstColType operator[] (int i) const { return weights_[i]; }
  // Accessors for pseudocount weight of letter a in central column
  float& operator() (int a) { return pc_[a]; }
  const float& operator() (int a) const { return pc_[a]; }
  float& pc(int a) { return pc_[a]; }
  const float& pc(int a) const { return pc_[a]; }
  // Returns the sum of pseudocount parameters.
  float pc_sum() const { return pc_.sum(); }
  // Returns number of context columns
  int num_cols() const { return weights_.num_rows(); }
  // Returns number of context columns
  int length() const { return weights_.num_rows(); }
  // Returns number of weights per context column
  int alphabet_size() const { return weights_.num_cols(); }
  // Returns the total number of context weights
  int size() const { return weights_.size(); }
  // Returns number of in-transitions.
  int num_in_transitions() const { return in_transitions_.num_nonempty(); }
  // Returns number of out-transitions.
  int num_out_transitions() const { return out_transitions_.num_nonempty(); }
  // Returns index of central column.
  int center() const { return (num_cols() - 1) / 2; }
  // Returns an iterator to the first element in context column i.
  ColType col_begin(int i) { return weights_.row_begin(i); }
  // Returns an iterator just past the end of context column i.
  ColType col_end(int i) { return weights_.row_end(i); }
  // Returns a const iterator to the first element in context column i.
  ConstColType col_begin(int i) const { return weights_.row_begin(i); }
  // Returns a const iterator just past the end of context column i.
  ConstColType col_end(int i) const { return weights_.row_end(i); }
  // Returns an iterator to the first weight in the context weight matrix.
  WeightIter begin() { return weights_.begin(); }
  // Returns an iterator just past the end of the context weight matrix.
  WeightIter end() { return weights_.end(); }
  // Returns a const iterator to the first element in the context matrix.
  ConstWeightIter begin() const { return weights_.begin(); }
  // Returns a const iterator just past the end of the context matrix.
  ConstWeightIter end() const { return weights_.end(); }
  // Returns a const iterator to start of list with non-null in-transition
  // pointers.
  ConstTransitionIter in_transitions_begin() const
  { return in_transitions_.nonempty_begin(); }
  // Returns a const iterator past the end of list with non-null
  // in-transition pointers.
  ConstTransitionIter in_transitions_end() const
  { return in_transitions_.nonempty_end(); }
  // Returns a const iterator to start of list with non-null out-transition
  // pointers.
  ConstTransitionIter out_transitions_begin() const
  { return out_transitions_.nonempty_begin(); }
  // Returns a const iterator past the end of list with non-null out-transition
  // pointers.
  ConstTransitionIter out_transitions_end() const
  { return out_transitions_.nonempty_end(); }
  // Initializes the state object with serialized data read from stream.
  void Read(FILE* fin);
  // Writes the profile in serialization format to output stream.
  void Write(FILE*) const;
  // Clears all in- and out-transitions.
  void ClearTransitions();

  // Prints profile in human-readable format for debugging.
  friend std::ostream& operator<< (std::ostream& out,
                                   const ContextWeightState& p) {
    p.Print(out);
    return out;
  }

 private:
  // Scaling factor for serialization of weights
  static const int kLogScale = 1000;
  // Buffer size for reading
  static const int kBufferSize = KB;

  // Reads and initializes serialized scalar class members from stream.
  virtual void ReadHeader(FILE* fin);
  // Reads and initializes weights from stream.
  virtual void ReadBody(FILE* fin);
  // Writes serialized scalar class members to stream.
  virtual void WriteHeader(FILE* fout) const;
  // Writes serialized weights to stream.
  virtual void WriteBody(FILE* fout) const;
  // Prints the state weights and parameters in human-readable format.
  virtual void Print(std::ostream& out) const;
  // Resize the weight matrices to given dimensions.
  void Resize(int num_cols, int alphabet_size);
  // Initializes context weights and pseudocounts with profile probabilities.
  void Init(const Profile<Alphabet>& prof);

  // Index of state in states vector of CRF.
  int index_;
  // List of in-transitions.
  sparsetable<AnchoredTransition> in_transitions_;
  // List of out-transitions.
  sparsetable<AnchoredTransition> out_transitions_;
  // Matrix with context weights
  matrix<float> weights_;
  // Unnormalized logs of pseudocounts in central column
  std::valarray<float> pc_;

  // CRF needs access to transition tables.
  friend class ChainGraph<Alphabet, ::cs::ContextWeightState>;
};  // class ContextWeightState

// Resets all weights and pseudocounts to the given value or zero.
template<class Alphabet>
void Reset(ContextWeightState<Alphabet>* s, float value = 0.0f);

}  // namespace cs

#endif  // SRC_CONTEXT_WEIGHT_STATE_H_
