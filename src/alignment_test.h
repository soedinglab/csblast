#include <cxxtest/TestSuite.h>

#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "matrix.h"
#include "nucleic_acid_alphabet.h"
#include "alignment.h"

class AlignmentTestSuite : public CxxTest::TestSuite
{
  public:
    void test_construction_from_input_stream( void )
    {
        cs::NucleicAcidAlphabet* na = cs::NucleicAcidAlphabet::instance();
        std::string data;
        data.append(">seq1\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACA---ACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        data.append(">seq2\nACGT--GTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTA---GTACGT--\n");
        std::istringstream ss(data);
        cs::Alignment alignment(ss, na);

        TS_ASSERT_EQUALS( alignment.nseqs(), 2 );
        TS_ASSERT_EQUALS( alignment.ncols(), 80 );
        TS_ASSERT_EQUALS( alignment(0,0), na->ctoi('A') );
        TS_ASSERT_EQUALS( alignment(1,1), na->ctoi('C') );
        TS_ASSERT( alignment.gap(1,4) );
        TS_ASSERT( alignment.endgap(1,78) );
    }

    void test_global_weights_and_neff( void )
    {
        cs::NucleicAcidAlphabet* na = cs::NucleicAcidAlphabet::instance();
        std::string data;
        data.append(">seq1\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        data.append(">seq2\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        data.append(">seq3\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        data.append(">seq4\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        std::istringstream ss(data);
        cs::Alignment alignment(ss, na);

        TS_ASSERT_EQUALS( alignment.nseqs(), 4 );
        TS_ASSERT_EQUALS( alignment.ncols(), 80 );

        std::pair< std::vector<float>, float> wg_neff = cs::global_weights_and_diversity(alignment);

        TS_ASSERT_EQUALS( static_cast<int>(wg_neff.first.size()), 4 );
        TS_ASSERT_EQUALS( wg_neff.first[0], 0.25 );
        TS_ASSERT_EQUALS( wg_neff.second, 1.0 );
    }

    void test_position_dependent_weights_and_neff( void )
    {
        cs::NucleicAcidAlphabet* na = cs::NucleicAcidAlphabet::instance();
        std::string data;
        data.append(">seq1\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        data.append(">seq2\nACGTTACGTACACGTACGTACACGTACGTA\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        data.append(">seq3\n----GTACGTACACGTACGTACACGTACGT\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        data.append(">seq4\n----CGTACGTACACGTACGTACACGTACG\nACGTACGTACACGTACGTACACGTACGTAC\nACGTACGTACACGTACGTAC\n");
        std::istringstream ss(data);
        cs::Alignment alignment(ss, na);

        TS_ASSERT_EQUALS( alignment.nseqs(), 4 );
        TS_ASSERT_EQUALS( alignment.ncols(), 80 );

        std::pair< Matrix<float>, std::vector<float> > wi_neff = cs::position_dependent_weights_and_diversity(alignment);

        TS_ASSERT_EQUALS( wi_neff.first(0,0), 0.5 );
    }
};
