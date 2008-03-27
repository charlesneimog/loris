#ifndef INCLUDE_MORPHER_H
#define INCLUDE_MORPHER_H
/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2008 by Kelly Fitz and Lippold Haken
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
 * Morpher.h
 *
 * Definition of class Morpher.
 *
 * Kelly Fitz, 15 Oct 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "PartialList.h"
#include "Partial.h"

#include <memory>   // for auto_ptr

//	begin namespace
namespace Loris {

class Envelope;

// ---------------------------------------------------------------------------
//	Class Morpher
//
//!	Class Morpher performs sound morphing and Partial parameter
//!	envelope interpolation according to a trio of frequency, amplitude,
//!	and bandwidth morphing functions, described by Envelopes.
//!	Sound morphing is achieved by interpolating the time-varying 
//!	frequencies, amplitudes, and bandwidths of corresponding partials 
//!	obtained from reassigned bandwidth-enhanced analysis of the source 
//!	and target sounds. Partial correspondences may be established by 
//!	labeling, using instances of the Channelizer and Distiller classes.
//!
//!	The Morpher collects morphed Partials in a PartialList, that is
//!	accessible to clients.
//!
//!	For more information about sound morphing using 
//!	the Reassigned Bandwidth-Enhanced Additive Sound 
//!	Model, refer to the Loris website: 
//!	www.cerlsoundgroup.org/Loris/.
//!	
//!	Morpher is a leaf class, do not subclass.
//
class Morpher
{
//	-- instance variables --

	std::auto_ptr< Envelope > _freqFunction;  //!	frequency morphing function
	std::auto_ptr< Envelope > _ampFunction;   //!	amplitude morphing function
	std::auto_ptr< Envelope > _bwFunction;    //!	bandwidth morphing function
	
	PartialList _partials;	                  //!	collect Partials here
	
	Partial _srcRefPartial;         //!	reference Partials
	Partial _tgtRefPartial;			//!	for source and target sounds when 
									//!	morphing sequences of labeled Partials,
									//!	default (empty Partial) implies no 
									//! reference Partial is used

	double _freqFixThresholdDb;		//!	amplitude threshold below which Partial
									//!	frequencies are corrected according to
									//!	a reference Partial, if specified.
	
	double _ampMorphShape;			//!	shaping parameter that controls the 
									//!	slope of the amplitude morphing function,
									//!	for values greater than 1, this function
									//!	gets nearly linear (like the old amplitude
									//!	morphing function), for values much less 
									//!	than 1 (e.g. 1E-5) the slope is gently
									//!	curved and sounds pretty "linear", for 
									//!	very small values (e.g. 1E-12) the curve
									//!	is very steep and sounds un-natural because
									//!	of the huge jump from zero amplitude to
									//!	very small amplitude.
															
	double _minBreakpointGapSec;	//!	the minimum time gap between two Breakpoints
									//!	in the morphed Partials. Morphing two
									//!	Partials can generate a third Partial having
									//!	Breakpoints arbitrarily close together in time,
									//!	and this makes morphs huge. Raising this 
									//!	threshold limits the Breakpoint density in
									//!	the morphed Partials. 
									//!	Default is 1/10 ms.
//	-- public interface --
public:
//	-- construction --

	//!	Construct a new Morpher using the same morphing envelope for 
	//!	frequency, amplitude, and bandwidth (noisiness).
	//!	
	//!	\param 	f is the Envelope to clone for all three morphing 
	//!			   functions.
	Morpher( const Envelope & f );

	//!	Construct a new Morpher using the specified morphing envelopes for
	//!	frequency, amplitude, and bandwidth (noisiness).
	//!
	//!	\param 	ff is the Envelope to clone for the frequency morphing function
	//!	\param 	af is the Envelope to clone for the amplitude morphing function
	//!	\param 	bwf is the Envelope to clone for the bandwidth morphing function	
	Morpher( const Envelope & ff, const Envelope & af, const Envelope & bwf );

	//!	Construct a new Morpher that is a duplicate of rhs.
	//!
	//! \param  rhs is the Morpher to duplicate
	Morpher( const Morpher & rhs );
	
	//!	Destroy this Morpher.
	~Morpher( void );
	
   //! Make this Morpher a duplicate of rhs.
	//!
	//! \param  rhs is the Morpher to duplicate
	Morpher & operator= ( const Morpher & rhs );

//	-- morphed parameter computation --

//	-- Partial morphing --

	//!	Morph a pair of Partials to yield a new morphed Partial. 
	//!	Dummy Partials (having no Breakpoints) don't contribute to the
	//!	morph, except to cause their opposite to fade out. 
	//!	Either (or neither) the source or target Partial may be a dummy 
	//!	Partial (no Breakpoints), but not both. The morphed
	//!	Partial has Breakpoints at times corresponding to every Breakpoint 
	//!	in both source Partials, omitting Breakpoints that would be
	//!   closer than the minBreakpointGap to their predecessor. 
	//!	The new morphed Partial is assigned the specified label and returned.
	//!
	//!	\param 	src is the Partial corresponding to a morph function
	//!		   	value of 0, evaluated at the specified time.
	//!	\param 	tgt is the Partial corresponding to a morph function
	//!		   	value of 1, evaluated at the specified time.
	//!	\param 	assignLabel is the label assigned to the morphed Partial
	//! \return the morphed Partial
	Partial morphPartials( Partial src, Partial tgt, int assignLabel );
	
	//! Bad legacy name for morphPartials.
	//! \deprecated Use morphPartials instead.
	Partial morphPartial( Partial src, Partial tgt, int assignLabel )
	    { return morphPartials( src, tgt, assignLabel ); }

	//!	Morph two sounds (collections of Partials labeled to indicate
	//!	correspondences) into a single labeled collection of Partials.
	//!	Unlabeled Partials (having label 0) are crossfaded. The morphed
	//!	and crossfaded Partials are stored in the Morpher's PartialList.
	//!
	//!	The Partials in the first range are treated as components of the 
	//!	source sound, corresponding to a morph function value of 0, and  
	//!	those in the second are treated as components of the target sound, 
	//!	corresponding to a morph function value of 1.
	//!
	//!	\sa     crossfade, morphPartials
	//!
	//!	\param  beginSrc is the beginning of the sequence of Partials
	//!	        corresponding to a morph function value of 0.
	//!	\param  endSrc is (one past) the end of the sequence of Partials
	//!		    corresponding to a morph function value of 0.
	//!	\param  beginTgt is the beginning of the sequence of Partials
	//!		    corresponding to a morph function value of 1.
	//!	\param  endTgt is (one past) the end of the sequence of Partials
	//!		    corresponding to a morph function value of 1.
	void morph( PartialList::const_iterator beginSrc, 
                PartialList::const_iterator endSrc,
                PartialList::const_iterator beginTgt, 
                PartialList::const_iterator endTgt );

	//!	Crossfade Partials with no correspondences.
	//!
	//!	Unlabeled Partials (having the specified label) are considered to 
	//!	have no correspondences, so they are just faded out, and not 
	//!	actually morphed. Consistent with the morphing behavior, 
	//! crossfaded Partials are thinned, if necssary, so that no
	//! two Breakpoints are closer in time than the minBreakpointGap.
	//!
	//!	The Partials in the first range are treated as components of the 
	//!	source sound, corresponding to a morph function value of 0, and  
	//!	those in the second are treated as components of the target sound, 
	//!	corresponding to a morph function value of 1.
	//!
	//!	The crossfaded Partials are stored in the Morpher's PartialList.
	//!
	//!	\param  beginSrc is the beginning of the sequence of Partials
	//!		    corresponding to a morph function value of 0.
	//!	\param  endSrc is (one past) the end of the sequence of Partials
	//!			corresponding to a morph function value of 0.
	//!	\param  beginTgt is the beginning of the sequence of Partials
	//!		    corresponding to a morph function value of 1.
	//!	\param  endTgt is (one past) the end of the sequence of Partials
	//!			corresponding to a morph function value of 1.
	//!	\param  label is the label to associate with unlabeled
	//!	        Partials (default is 0).
	void crossfade( PartialList::const_iterator beginSrc, 
                    PartialList::const_iterator endSrc,
                    PartialList::const_iterator beginTgt, 
                    PartialList::const_iterator endTgt,
                    Partial::label_type label = 0 );


    //!	Compute morphed parameter values at the specified time, using
    //!	the source and target Breakpoints (assumed to correspond exactly
    //!	to the specified time).
    //!
    //!	\param  srcBkpt is the Breakpoint corresponding to a morph function
    //!	        value of 0.
    //!	\param  tgtBkpt is the Breakpoint corresponding to a morph function
    //!	        value of 1.
    //!	\param  time is the time corresponding to srcBkpt (used
    //!	        to evaluate the morphing functions and tgtPartial).
    //!	\return the morphed Breakpoint
    //
    Breakpoint
    morphBreakpoints( Breakpoint srcBkpt, Breakpoint tgtBkpt, 
                      double time  ) const;
                               
    //!	Compute morphed parameter values at the specified time, using
    //!	the source Breakpoint (assumed to correspond exactly to the
    //!	specified time) and the target Partial (whose parameters are
    //!	examined at the specified time).
    //!
    //! DEPRECATED do not use.
    //!
    //!	\pre    the target Partial may not be a dummy Partial (no Breakpoints).
    //!
    //!	\param 	bp is the Breakpoint corresponding to a morph function
    //!		   	value of 0.
    //!	\param 	tgtPartial is the Partial corresponding to a morph function
    //!		   	value of 1, evaluated at the specified time.
    //!	\param 	time is the time corresponding to srcBkpt (used
    //!	        to evaluate the morphing functions and tgtPartial).
    //! \return the morphed Breakpoint
    Breakpoint 
    morphSrcBreakpoint( const Breakpoint & bp, const Partial & tgtPartial, 
                        double time ) const;

    //!	Compute morphed parameter values at the specified time, using
    //!	the target Breakpoint (assumed to correspond exactly to the
    //!	specified time) and the source Partial (whose parameters are
    //!	examined at the specified time).
    //!
    //! DEPRECATED do not use.
    //!
    //!	\pre    the source Partial may not be a dummy Partial (no Breakpoints).
    //!
    //!	\param 	bp is the Breakpoint corresponding to a morph function
    //!		   	value of 1.
    //!	\param 	srcPartial is the Partial corresponding to a morph function
    //!		   	value of 0, evaluated at the specified time.
    //!	\param 	time is the time corresponding to srcBkpt (used
    //!	        to evaluate the morphing functions and tgtPartial).
    //! \return the morphed Breakpoint
    Breakpoint 
    morphTgtBreakpoint( const Breakpoint & bp, const Partial & srcPartial, 
                        double time ) const;

    //!	Compute morphed parameter values at the specified time, using
    //!	the source Breakpoint, assumed to correspond exactly to the
    //!	specified time, and assuming that there is no corresponding 
    //! target Partial, so the source Breakpoint should be simply faded.
    //!
    //!	\param 	bp is the Breakpoint corresponding to a morph function
    //!		   	value of 0.
    //!	\param 	time is the time corresponding to bp (used
    //!			to evaluate the morphing functions).
    //!	\return the faded Breakpoint
    Breakpoint fadeSrcBreakpoint( Breakpoint bp, double time ) const;

    //!	Compute morphed parameter values at the specified time, using
    //!	the target Breakpoint, assumed to correspond exactly to the
    //!	specified time, and assuming that there is not corresponding 
    //! source Partial, so the target Breakpoint should be simply faded.
    //!
    //!	\param 	bp is the Breakpoint corresponding to a morph function
    //!		   	value of 1.
    //!	\param 	time is the time corresponding to bp (used
    //!			to evaluate the morphing functions).
    //!	\return the faded Breakpoint
    Breakpoint fadeTgtBreakpoint( Breakpoint bp, double time ) const;
    
//	-- morphing function access/mutation --

	//!	Assign a new frequency morphing envelope to this Morpher.
	void setFrequencyFunction( const Envelope & f );
	
	//!	Assign a new amplitude morphing envelope to this Morpher.
	void setAmplitudeFunction( const Envelope & f );
	
	//!	Assign a new bandwidth morphing envelope to this Morpher.
	void setBandwidthFunction( const Envelope & f );

	//!	Return a reference to this Morpher's frequency morphing envelope.
	const Envelope & frequencyFunction( void ) const;
	
	//!	Return a reference to this Morpher's amplitude morphing envelope.
	const Envelope & amplitudeFunction( void ) const;
	
	//!	Return a reference to this Morpher's bandwidth morphing envelope.
	const Envelope & bandwidthFunction( void ) const;
	
	//!	Return the shaping parameter for the amplitude moprhing
	//!	function (only used in new log-amplitude morphing).
	//!	This shaping parameter controls the 
	//!	slope of the amplitude morphing function,
	//!	for values greater than 1, this function
	//!	gets nearly linear (like the old amplitude
	//!	morphing function), for values much less 
	//!	than 1 (e.g. 1E-5) the slope is gently
	//!	curved and sounds pretty "linear", for 
	//!	very small values (e.g. 1E-12) the curve
	//!	is very steep and sounds un-natural because
	//!	of the huge jump from zero amplitude to
	//!	very small amplitude.	
	double amplitudeShape( void ) const;
	
	//!	Set the shaping parameter for the amplitude moprhing
	//!	function (only used in new log-amplitude morphing).
	//!	This shaping parameter controls the 
	//!	slope of the amplitude morphing function,
	//!	for values greater than 1, this function
	//!	gets nearly linear (like the old amplitude
	//!	morphing function), for values much less 
	//!	than 1 (e.g. 1E-5) the slope is gently
	//!	curved and sounds pretty "linear", for 
	//!	very small values (e.g. 1E-12) the curve
	//!	is very steep and sounds un-natural because
	//!	of the huge jump from zero amplitude to
	//!	very small amplitude.
	//!
	//!	\param 	x is the new shaping parameter, it must be positive.
	void setAmplitudeShape( double x );
	
	//!	Return the minimum time gap (secs) between two Breakpoints
	//!	in the morphed Partials. Morphing two
	//!	Partials can generate a third Partial having
	//!	Breakpoints arbitrarily close together in time,
	//!	and this makes morphs huge. Raising this 
	//!	threshold limits the Breakpoint density in
	//!	the morphed Partials. Default is 1/10 ms.
	double minBreakpointGap( void ) const;

	//!	Set the minimum time gap (secs) between two Breakpoints
	//!	in the morphed Partials. Morphing two
	//!	Partials can generate a third Partial having
	//!	Breakpoints arbitrarily close together in time,
	//!	and this makes morphs huge. Raising this 
	//!	threshold limits the Breakpoint density in
	//!	the morphed Partials. Default is 1/10 ms.
	//!
	//!	\param 	x is the new minimum gap in seconds, it must be 
	//!			positive
	//! \throw  InvalidArgument if the specified gap is not positive
	void setMinBreakpointGap( double x );


//	-- reference Partial label access/mutation --
	
	//! Return the Partial to be used as a reference
	//!	Partial for the source sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//!	A default (empty) Partial indicates that no reference Partial
	//!	should be used for the source sequence.
	const Partial & sourceReferencePartial( void ) const;
	
	//! Return the Partial to be used as a reference
	//!	Partial for the source sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//!	A default (empty) Partial indicates that no reference Partial
	//!	should be used for the source sequence.
	Partial & sourceReferencePartial( void );
	
	//! Return the Partial to be used as a reference
	//!	Partial for the target sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//!	A default (empty) Partial indicates that no reference Partial
	//!	should be used for the target sequence.
	const Partial & targetReferencePartial( void ) const;

	//! Return the Partial to be used as a reference
	//!	Partial for the target sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//!	A default (empty) Partial indicates that no reference Partial
	//!	should be used for the target sequence.
	Partial & targetReferencePartial( void );
	
	//! Specify the Partial to be used as a reference
	//!	Partial for the source sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//! The specified Partial must be labeled with its harmonic number.
	//!	A default (empty) Partial indicates that no reference 
	//!	Partial should be used for the source sequence.
	void setSourceReferencePartial( const Partial & p = Partial() );
	
	//! Specify the Partial to be used as a reference
	//!	Partial for the source sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//!	A default (empty) Partial indicates that no reference 
	//!	Partial should be used for the source sequence.
	//!
	//!	\param	partials a sequence of Partials to search
	//!			for the reference Partial
	//!	\param	refLabel the label of the Partial in partials
	//!			that should be selected as the reference
	void setSourceReferencePartial( const PartialList & partials, 
									Partial::label_type refLabel );
	
	//! Specify the Partial to be used as a reference
	//!	Partial for the target sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//! The specified Partial must be labeled with its harmonic number.
	//!	A default (empty) Partial indicates that no reference 
	//!	Partial should be used for the target sequence.
	void setTargetReferencePartial( const Partial & p = Partial() );

	//! Specify the Partial to be used as a reference
	//!	Partial for the target sequence in a morph of two Partial
	//!	sequences. The reference partial is used to compute 
	//!	frequencies for very low-amplitude Partials whose frequency
	//!	estimates are not considered reliable. The reference Partial
	//!	is considered to have good frequency estimates throughout.
	//!	A default (empty) Partial indicates that no reference 
	//!	Partial should be used for the target sequence.
	//!
	//!	\param	partials a sequence of Partials to search
	//!			for the reference Partial
	//!	\param	refLabel the label of the Partial in partials
	//!			that should be selected as the reference
	void setTargetReferencePartial( const PartialList & partials, 
									Partial::label_type refLabel );
	
//	-- PartialList access --

	//!	Return a reference to this Morpher's list of morphed Partials.
	PartialList & partials( void ); 

	//!	Return a const reference to this Morpher's list of morphed Partials.
	const PartialList & partials( void ) const; 

//	-- global morphing defaults and constants --

    //! Amplitude threshold (dB) below which 
    //! Partial frequencies are corrected using
    //! the reference Partial frequency envelope
    //!	(if specified).
    static const double DefaultFixThreshold;

    //!	Default amplitude shaping parameter, used in
    //!	interpolateLogAmplitudes to perform logarithmic 
    //!	amplitude morphs. 
    //!
    //!	Compile Loris with LINEAR_AMP_MORPHS defined for
    //!	legacy-style linear amplitude morphs by default.
    //!
    //!	Change from default using setAmplitudeShape.
    static const double DefaultAmpShape;    

    //!	Default minimum time (sec) between Breakpoints in 
    //!	morphed Partials. 
    //!	Change from default using setMinBreakpointGap.
    static const double DefaultBreakpointGap;

private:
	
//	-- helper --

	//	PartialCorrespondence represents a map from non-zero Partial 
	//	labels to pairs of pointers to Partials that should be morphed 
	//	into a single Partial that is assigned that label. 
	//	MorphingPair is a pair of pointers to Partials that are
	//	initialized to zero, and it is the element type for the
	//	PartialCorrespondence map.
	struct MorphingPair
	{
		Partial src;
		Partial tgt;	
	};
	typedef std::map< Partial::label_type, MorphingPair > PartialCorrespondence;
	
	//!	Helper function that performs the morph between corresponding pairs
	//!	of Partials identified in a PartialCorrespondence. Called by the
	//!	morph() implementation accepting two sequences of Partials.
	void morph_aux( PartialCorrespondence & correspondence );
	
    //!	Compute morphed parameter values at the specified time, using
    //!	the source Breakpoint (assumed to correspond exactly to the
    //!	specified time) and the target Partial (whose parameters are
    //!	examined at the specified time). Append the morphed Breakpoint
    //!	to newp only if the target should contribute to the morph at
    //!	the specified time.
    //!
    //!	\pre    the target Partial may not be a dummy Partial (no Breakpoints).
    //!
    //!	\param  srcBkpt is the Breakpoint corresponding to a morph function
    //!	        value of 0.
    //!	\param  tgtPartial is the Partial corresponding to a morph function
    //!	        value of 1, evaluated at the specified time.
    //!	\param  time is the time corresponding to srcBkpt (used
    //!	        to evaluate the morphing functions and tgtPartial).
    //!	\param  newp is the morphed Partial under construction, the morphed
    //!	        Breakpoint is added to this Partial.
    //
    void appendMorphedSrc( Breakpoint srcBkpt, const Partial & tgtPartial, 
                           double time, Partial & newp  );
                           
    //!	Compute morphed parameter values at the specified time, using
    //!	the target Breakpoint (assumed to correspond exactly to the
    //!	specified time) and the source Partial (whose parameters are
    //!	examined at the specified time). Append the morphed Breakpoint
    //!	to newp only if the target should contribute to the morph at
    //!	the specified time.
    //!
    //!	\pre    the source Partial may not be a dummy Partial (no Breakpoints).
    //!
    //!	\param  tgtBkpt is the Breakpoint corresponding to a morph function
    //!	        value of 1.
    //!	\param  srcPartial is the Partial corresponding to a morph function
    //!	        value of 0, evaluated at the specified time.
    //!	\param  time is the time corresponding to srcBkpt (used
    //!	        to evaluate the morphing functions and srcPartial).
    //!	\param  newp is the morphed Partial under construction, the morphed
    //!	        Breakpoint is added to this Partial.
    //
    void appendMorphedTgt( Breakpoint tgtBkpt, const Partial & srcPartial, 
                           double time, Partial & newp  );
                           

};	//	end of class Morpher

}	//	end of namespace Loris

#endif /* ndef INCLUDE_MORPHER_H */
