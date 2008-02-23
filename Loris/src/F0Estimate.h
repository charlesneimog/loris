#ifndef INCLUDE_F0ESTIMATE_H
#define INCLUDE_F0ESTIMATE_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2007 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * F0Estimate.h
 *
 * Implementation of an iterative alrogithm for computing an 
 * estimate of fundamental frequency from a sequence of sinusoidal
 * frequencies and amplitudes using a maximum likelihood algorithm
 * adapted from Quatieri's Speech Signal Processing text. The 
 * algorithm here takes advantage of the fact that spectral peaks
 * have already been identified and extracted in the analysis/modeling
 * process.
 *
 * Kelly Fitz, 28 March 2006
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <vector>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//  class F0estimate
//
//  Represents a configuration of an iterative alrogithm for computing an 
//  estimate of fundamental frequency from a sequence of sinusoidal
//  frequencies and amplitudes using a maximum likelihood algorithm
//  adapted from Quatieri's Speech Signal Processing text. The 
//  algorithm here takes advantage of the fact that spectral peaks
//  have already been identified and extracted in the analysis/modeling
//  process.

class F0estimate
{
private:

    double m_frequency;   //  estimated fundamental frequency in Hz
    double m_confidence;  //  normalized confidence for this estimate, 
                          //  equal to 1.0 when all frequencies are perfect
                          //  harmonics of this estimate's frequency

public:

    //  --- lifecycle ---

    //  Construct from parameters of the iterative F0 estimation 
    //  algorithm.
    //
    //	Iteratively compute the value of the likelihood function
    //	at a range of frequencies around the peak likelihood.
    //	Store the maximum value when the range of likelihood
    //	values computed is less than the specified resolution.
    //  Store the frequency and the normalized value of the 
    //  likelihood function at that frequency (1.0 indicates that
    //  all the peaks are perfect harmonics of the estimated
    //  frequency).

    F0estimate ( const std::vector<double> & amps, 
                 const std::vector<double> & freqs, 
                 double fmin, double fmax,
                 double resolution );

    // F0estimate( void ) : m_frequency( 0 ), m_confidence( 0 ) {}
    // F0estimate( double f, double c ) : m_frequency( f ), m_confidence( c ) {}
    
    //  --- accessors ---
    
    //  F0 frequency estimate, in Hz    
    double frequency( void ) const { return m_frequency; }
    
    //  Normalized confidence for this estimate, 
    //  equal to 1.0 when all frequencies are perfect
    //  harmonics of this estimate's frequency    
    double confidence( void ) const { return m_confidence; }
    
};  //  end of class F0estimate


}	//	end of namespace Loris

#endif  //  ndef INCLUDE_F0ESTIMATE_H