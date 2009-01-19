#include <gtest/gtest.h>

#include "amino_acid_alphabet.h"
#include "nucleotide_alphabet.h"

TEST(AlphabetTest, AminoAcidAlphabet)
{
    cs::AminoAcidAlphabet* aa = cs::AminoAcidAlphabet::instance();
    EXPECT_EQ(20, aa->size());
    EXPECT_EQ('R', aa->itoc(aa->ctoi('R')));
    EXPECT_EQ(1, aa->ctoi(aa->itoc(1)));
    EXPECT_EQ('A', aa->itoc(0));
}

TEST(AlphabetTest, NucleotideAlphabet)
{
    cs::NucleotideAlphabet* na = cs::NucleotideAlphabet::instance();
    EXPECT_EQ(4, na->size());
    EXPECT_EQ('C', na->itoc(na->ctoi('C')));
    EXPECT_EQ(1, na->ctoi( na->itoc(1)));
    EXPECT_EQ('A', na->itoc(0));
}