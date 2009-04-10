#include <gtest/gtest.h>

#include <cstdio>

#include "amino_acid.h"
#include "count_profile-inl.h"
#include "emitter-inl.h"
#include "hmm-inl.h"
#include "hmm_pseudocounts-inl.h"
#include "library_pseudocounts-inl.h"
#include "log.h"

namespace cs {

const float kFloatDelta = 0.01f;

TEST(HMMPseudocountsTest, AddToSequence) {
  Sequence<AminoAcid> seq("triple zinc finger",
                          "KPSRMRKYPNRPSKTPPHERPYACPVESCDRRFSR"
                          "SDELTRHIRIHTGQKPFQCRICMRNFSRSDHLTTH");
  Profile<AminoAcid> profile(seq.length());

  FILE* fin = fopen("../data/scop20_K100.hmm", "r");
  HMM<AminoAcid> hmm(fin);
  fclose(fin);

  ASSERT_EQ(100, hmm.num_states());

  EmissionParams params;
  HMMPseudocounts<AminoAcid> pc(&hmm, params);
  pc.add_to_sequence(seq, DivergenceDependentAdmixture(1.0f, 10.0f), &profile);

  EXPECT_NEAR(0.8121f, profile[23][AminoAcid::instance().ctoi('C')], kFloatDelta);
  EXPECT_NEAR(0.8121f, profile[28][AminoAcid::instance().ctoi('C')], kFloatDelta);
}

TEST(HMMPseudocountsTest, AddProfileSequence) {
  FILE* ali_in = fopen("../data/zinc_finger.fas", "r");
  Alignment<AminoAcid> ali(ali_in, Alignment<AminoAcid>::FASTA);
  fclose(ali_in);
  CountProfile<AminoAcid> profile(ali, false);

  FILE* fin = fopen("../data/scop20_K100.hmm", "r");
  HMM<AminoAcid> hmm(fin);
  fclose(fin);

  ASSERT_EQ(100, hmm.num_states());

  EmissionParams params;
  HMMPseudocounts<AminoAcid> pc(&hmm, params);
  pc.add_to_profile(DivergenceDependentAdmixture(1.0f, 10.0f), &profile);

  EXPECT_NEAR(0.7756f, profile[53][AminoAcid::instance().ctoi('C')], kFloatDelta);
  EXPECT_NEAR(0.7720f, profile[56][AminoAcid::instance().ctoi('C')], kFloatDelta);
}

}  // namespace cs


