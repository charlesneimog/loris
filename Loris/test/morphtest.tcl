#!tclsh
#
#   This is the Loris C++ Class Library, implementing analysis, 
#   manipulation, and synthesis of digitized sounds using the Reassigned 
#   Bandwidth-Enhanced Additive Sound Model.
#   
#   Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
#  
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY, without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  
#  
#	morphtest.tcl
#
#	Very simple Loris instrument tone morphing demonstration using a
#	dynamically-loaded Tcl module.
#
#   Kelly Fitz, 21 May 2001
#   loris@cerlsoundgroup.org
#  
#   http://www.cerlsoundgroup.org/Loris/
#
puts "
Welcome to the very simple Loris morphing test!
Kelly Fitz 2000

Generates a simple linear morph between a 
clarinet and a flute.
"

load ../lib/loris.so loris

#
#	analyze clarinet tone
#
puts "analyzing clarinet 3G#"
set a [ Analyzer -args 390 ]

set cf [ AiffFile -args clarinet.aiff ]
set v [ $cf samples ]
set samplerate [ $cf sampleRate ]

set clar [ $a analyze $v $samplerate ]

puts "checking SDIF export/import"
exportSdif clarinet.sdif $clar
PartialList -this [ set clar [ importSdif clarinet.sdif ] ]

puts "making a bogus attempt at writing an Spc file"
if { [ catch { [ExportSpc -args 90] write bad_spc_file.spc $clar } ] } {
	puts "Spc export failed!"
	puts "Error Info: $errorInfo"
	puts "Error Code: $errorCode"
}

channelize $clar [ createFreqReference $clar 20 0 1000 ] 1
distill $clar

# just for fun, print out the average 
# frequency of the first partial in the
# clarinet analysis:
set f 0
set n 0
PartialIterator -this [ set iter [ Partial_begin [ PartialListIterator_partial [ $clar begin ] ] ] ] 
set end [ Partial_end [ PartialListIterator_partial [ $clar begin ] ] ]

while { ! [ $iter equals $end ] } {
	set f [expr $f + [ Breakpoint_frequency [ PartialIterator_breakpoint $iter ] ] ]
	incr n
	$iter next
}
puts "avg frequency of first distilled clarinet partial is [expr $f/$n]"

puts "shifting pitch of clarinet"
shiftPitch $clar [ BreakpointEnvelopeWithValue -600 ]

# check clarinet synthesis:
exportAiff clarOK.aiff [ synthesize $clar $samplerate ] $samplerate 1 16


#
#	analyze flute tone (reuse Analyzer)
#
puts "analyzing flute 3D"
$a configure 270
set v [ [ AiffFile -args flute.aiff ] samples ]
set flut [ $a analyze $v $samplerate ]

channelize $flut [ createFreqReference $flut 20 0 1000 ] 1
distill $flut

# check flute synthesis:
exportAiff flutOK.aiff [ synthesize $flut $samplerate ] $samplerate 1 16

#
#	perform temporal dilation
#
set flute_times "0.4, 1."
set clar_times  "0.2, 1."
set tgt_times  "0.3, 1.2"

puts "dilating sounds to match $tgt_times"
puts "flute times: $flute_times"
dilate $flut $flute_times $tgt_times
puts "clarinet times: $clar_times"
dilate $clar $clar_times $tgt_times

#
#	perform morph
#
puts "morphing flute and clarinet"
set mf [BreakpointEnvelope]
$mf insertBreakpoint 0.6 0
$mf insertBreakpoint 2 1
set m [ morph $clar $flut $mf $mf $mf ]
exportAiff morph.test.aiff [ synthesize $m $samplerate ] $samplerate 1 16

puts "done"

