#include <cxxtest/TestSuite.h>

#include <string>
#include <sstream>

#include "amino_acid_alphabet.h"
#include "nucleic_acid_alphabet.h"
#include "sequence_profile.h"

class SequenceProfileTestSuite : public CxxTest::TestSuite
{
  public:
    void test_construction_from_input_stream( void )
    {
        cs::NucleicAcidAlphabet* na = cs::NucleicAcidAlphabet::instance();
        std::string data;
        data.append("#\tA\tC\tG\tT\n");
        data.append("1\t0\t*\t*\t*\n");
        data.append("2\t*\t0\t*\t*\n");
        data.append("3\t*\t*\t0\t*\n");
        data.append("4\t*\t*\t*\t0\n");
        data.append("5\t0\t*\t*\t*\n");
        data.append("6\t*\t0\t*\t*\n");

        std::istringstream ss(data);
        cs::SequenceProfile profile(ss, na);

        TS_ASSERT_EQUALS( profile.ncols(), 6 );
        TS_ASSERT_EQUALS( profile.ndim(), 4 );
        TS_ASSERT_EQUALS( profile(0,0), 1.0f );
        TS_ASSERT_EQUALS( profile(1,0), 0.0f );
    }
};
