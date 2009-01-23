/***************************************************************************
 *   Copyright (C) 2008 by Andreas Biegert                                 *
 *   andreas.biegert@googlemail.com                                        *
 ***************************************************************************/

#include "profile.h"

#include <cmath>
#include <cstring>

#include <iostream>
#include <iomanip>
#include <limits>

#include "exception.h"
#include "profile.h"
#include "sequence_alphabet.h"
#include "shared_ptr.h"
#include "util.h"

namespace cs
{

Profile::Profile(const SequenceAlphabet* alphabet)
        : alphabet_(alphabet)
{}

Profile::Profile(int ncols,
                 const SequenceAlphabet* alphabet)
        : data_(ncols, alphabet->size(), 0.0f),
          alphabet_(alphabet)
{}

Profile::Profile(std::istream& in, const SequenceAlphabet* alphabet)
        : alphabet_(alphabet)
{ read(in); }

Profile::Profile(const Profile& other,
                 int index,
                 int length)
        : data_(length, other.nalph(), 0.0f),
          alphabet_(other.alphabet_)
{
    if (index + length > other.ncols())
        throw Exception("Arguments index=%i and length=%i for construction of sub-profile are out of bounds!", index, length);
    for (int i = 0; i < ncols(); ++i)
        for (int a = 0; a < nalph(); ++a)
            data_[i][a] = other[i+index][a];
}

std::vector< shared_ptr<Profile> > Profile::readall(std::istream& in,
                                                 const SequenceAlphabet* alphabet)
{
    std::vector< shared_ptr<Profile> > profiles;
    while (in.peek() && in.good()) { //peek first to make sure that we don't read beyond '//'
        shared_ptr<Profile> p(new Profile(in, alphabet));
        profiles.push_back(p);
    }

    return profiles;
}

void Profile::transform_to_logspace()
{
    if (!logspace_) {
        for(int i = 0; i < ncols(); ++i)
            for(int a = 0; a < nalph(); ++a)
                if (data_[i][a] == 0.0f)
                    data_[i][a] = std::numeric_limits<double>::infinity();
                else
                    data_[i][a] = log2(data_[i][a]);
        logspace_ = true;
    }
}

void Profile::transform_to_linspace()
{
    if (logspace_) {
        for(int i = 0; i < ncols(); ++i)
            for(int a = 0; a < nalph(); ++a)
                data_[i][a] = pow(2.0, data_[i][a]);
        logspace_ = false;
    }
}

void Profile::read(std::istream& in)
{
    // Check if stream actually contains a serialized profile
    std::string buffer;
    while (getline(in, buffer) && !strscn(buffer.c_str())) continue;
    if (buffer != class_identity())
        throw Exception("Bad format: serialized profile does not start with '%s'!", class_identity().c_str());

    read_header(in);
    read_body(in);
}

void Profile::read_header(std::istream& in)
{
    const int kBufferSize = 1000;
    std::vector<char> char_arr(kBufferSize, '\0');
    char* buffer = &char_arr[0];

    // Read ncols
    int ncols = 0;
    if (in.getline(buffer, kBufferSize) && strncmp(buffer, "ncols", 5) == 0)
        ncols = atoi(buffer + 5);
    else
        throw Exception("Bad format: serialized profile does not contain 'ncols' record!");

    // Read nalph
    int nalph = 0;
    if (in.getline(buffer, kBufferSize) && strncmp(buffer, "nalph", 5) == 0)
        nalph = atoi(buffer + 5);
    else
        throw Exception("Bad format: serialized profile does not contain 'nalph' record!");
    if (nalph != alphabet_->size())
        throw Exception("Bad format: nalph=%i does not fit with provided alphabet!", nalph);

    resize(ncols, nalph);
}

void Profile::read_body(std::istream& in)
{
    const int kBufferSize = 1000;
    std::vector<char> char_arr(kBufferSize, '\0');
    char* buffer = &char_arr[0];

    in.getline(buffer, kBufferSize);  // Skip description line
    const char* ptr;  // for string traversal
    int i = 0;        // column index
    while (in.getline(buffer, kBufferSize)) {
        if (!strscn(buffer)) continue;
        if (strlen(buffer) > 1 && buffer[0] == '/' && buffer[1] == '/') break;

        ptr = buffer;
        i = strtoi(ptr) - 1;
        if (!ptr)
            throw Exception("Bad format: malformed line after column record %i!", i - 1);

        for (int a = 0; a < nalph(); ++a) {
            int log_p = strtoi_asterix(ptr);
            data_[i][a] = pow(2.0, static_cast<float>(-log_p) / kScaleFactor);
        }
    }
    if (i != ncols() - 1)
        throw Exception("Bad format: profile has %i column records but should have %i!", i+1, ncols());
}

void Profile::write(std::ostream& out) const
{
    out << class_identity() << std::endl;
    write_header(out);
    write_body(out);
}

void Profile::write_header(std::ostream& out) const
{
    // print dimensions
    out << "ncols\t\t" << ncols() << std::endl;
    out << "nalph\t\t" << nalph() << std::endl;
}

void Profile::write_body(std::ostream& out) const
{
    out << "\t";
    alphabet_->print(out, "\t");
    out << std::endl;
    for (int i = 0; i < ncols(); ++i) {
        out << i+1;
        for (int a = 0; a < nalph(); ++a) {
            double logval = log2(data_[i][a]);
            if (-logval == std::numeric_limits<double>::infinity())
                out << "\t*";
            else
                out << "\t" << -iround(logval * kScaleFactor);
        }
        out << std::endl;
    }
}

void Profile::print(std::ostream& out) const
{
    const int kWidth = 6;
    std::ios_base::fmtflags flags = out.flags(); // save old flags

    out << std::string(2*kWidth-2, ' ');
    alphabet_->print(out, std::string(kWidth-1, ' '));
    out << std::endl;
    for (int i = 0; i < ncols(); ++i) {
        out << std::left << std::setw(kWidth-1) << i+1;
        for (int a = 0; a < nalph(); ++a)
            out << std::right << std::setw(kWidth) << std::fixed << std::setprecision(3) << data_[i][a];
        out << std::endl;
    }

    out.flags(flags);
}

void Profile::resize(int ncols, int nalph)
{
    if (ncols == 0 || nalph == 0)
        throw Exception("Bad dimensions for profile resizing: ncols=%i nalph=%i", ncols, nalph);
    data_ = matrix<float>(ncols, nalph);
}

void reset(Profile& profile, float value)
{
    const int ncols = profile.ncols();
    const int nalph = profile.nalph();
    for(int i = 0; i < ncols; ++i)
        for(int a = 0; a < nalph; ++a)
            profile[i][a] = value;
}

void normalize(Profile& profile, float value)
{
    const int ncols = profile.ncols();
    const int nalph  = profile.nalph();

    for (int i = 0; i < ncols; ++i) {
        float sum = 0.0f;
        for (int a = 0; a < nalph; ++a) sum += profile[i][a];
        if (sum != 0.0f) {
            float fac = value / sum;
            for (int a = 0; a < nalph; ++a) profile[i][a] *= fac;
        } else {
            throw Exception("Unable to normalize profile to one. Sum of column %i is zero!", i);
        }
    }
}

}//cs


