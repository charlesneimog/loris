/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
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
 *	lorisBpEnvelope.i
 *
 *	SWIG interface file describing the Loris::BreakpointEnvelope class.
 *	Include this file in loris.i to include the BreakpointEnvelope class
 *	interface in the scripting module. (Can be used with the -shadow 
 *	option to SWIG to build a BreakpointEnvelope class in the Python 
 *	interface.) This file does not support exactly the public interface 
 *	of the C++ class, but has been modified to	better support SWIG 
 *	and scripting languages.
 *
 * Kelly Fitz, 17 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
%{
#include "BreakpointEnvelope.h"
using Loris::BreakpointEnvelope;

//	for procedural interface construction and 
//	destruction, see comment below:
#define LORIS_OPAQUE_POINTERS 0
#include "loris.h"
%}

// ---------------------------------------------------------------------------
//	class BreakpointEnvelope
//
class BreakpointEnvelope
/*	A BreakpointEnvelope represents a linear segment breakpoint 
	function with infinite extension at each end (that is, the 
	values past either end of the breakpoint function have the 
	values at the nearest end).
*/
{
public:
//	construction:
//
//	Mac ONLY problem:
//	use the construction and destruction functions in the 
//	procedural interface until I can determine why deleting
//	objects with destructors defined out of line (in the Loris
//	DLL) cause the Macintosh to crash. Using the procedural 
//	interface causes the objects with out of line destructors
//	to be constructed and destructed in the DLL, instead of 
//	across DLL boundaries, which might make a difference on
//	the Mac.
//	
%addmethods
{
	BreakpointEnvelope( void )
	{
		return createBreakpointEnvelope();
	}
	/*	Construct and return a new BreakpointEnvelope having no 
		breakpoints and an implicit value of 0. everywhere, 
		until the first breakpoint is inserted.			
	 */
	%name( BreakpointEnvelopeWithValue ) BreakpointEnvelope( double initialValue )
	{
		BreakpointEnvelope * env = createBreakpointEnvelope();
		env->insertBreakpoint( 0., initialValue );
		return env;
	}
	/*	Construct and return a new BreakpointEnvelope having a 
		single breakpoint at 0. having the specified initialValue.
	 */
	//%name( BreakpointEnvelopeCopy ) BreakpointEnvelope( const BreakpointEnvelope & other )
	%name( BreakpointEnvelopeCopy ) BreakpointEnvelope( const BreakpointEnvelope * other )
	{
		BreakpointEnvelope * env = createBreakpointEnvelope();
		*env = *other;
		return env;
	}
	/*	Construct and return a new BreakpointEnvelope that is
		a copy of this BreapointEnvelope (has the same value
		as this BreakpointEnvelope everywhere).			
	 */
	~BreakpointEnvelope( void )
	{
		destroyBreakpointEnvelope( self );
	}
	/*	Destroy this BreakpointEnvelope. 								
	 */
}
	
//	Envelope interface:
	virtual double valueAt( double x ) const;	
	/*	Return the interpolated value of this BreakpointEnvelope at 
		the specified time.							
	 */
	
//	envelope composition:
	void insertBreakpoint( double time, double value );
	/*	Insert a breakpoint representing the specified (time, value) 
		pair into this BreakpointEnvelope. If there is already a 
		breakpoint at the specified time, it will be replaced with 
		the new breakpoint.
	 */
	 
};	//	end of abstract class BreakpointEnvelope