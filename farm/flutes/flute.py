#!python

"""
flute.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the breathy flute tone that represented our
first successful bandwidth-enhanced synthesis, using ad-hoc bandwidth
association. This is one of our poster-child sounds, we use it in
many demos and for testing of basic morphing functionality (usually
with the clarinet). The pitch is D above middle C, around 291 Hz, 
and there's an interesting pre-attack whistle. 

Sounds good with resolution of 240 Hz and a window as wide as the fundamental
frequency (291 Hz), or thereabouts, and distill at one partial per harmonic.

Last updated: 16 May 2003 by Kelly Fitz
"""
print __doc__

import loris, time

print """
Using Loris version %s
"""%loris.version()

anal = loris.Analyzer( 240, 300 )
orate = 44100

#
# 291 Hz flute tone
#
name = 'flute291'
f = loris.AiffFile( name + '.aiff' )
print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
p = anal.analyze( f.samples(), f.sampleRate() )

# distilled using one partial per harmonic
ref = loris.createFreqReference( p, 291*.5, 291*1.5, 100 )
loris.channelize( p, ref, 1 )
loris.distill( p )
print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
samples = loris.synthesize( p, orate )
loris.exportAiff( name + '.recon.aiff', samples, orate )
loris.exportSdif( name + '.sdif', p )
loris.exportSpc( name + '.s.spc', p, 62, 0 )
loris.exportSpc( name + '.e.spc', p, 62, 1 )

##The parameters we once liked for this are resolution and
##window width 270 Hz.
##
##notes from trial 1:
##	cannot use resolution as great as 290 (not surprising)
##	otherwise, these are mostly pretty tough to distinguish
##	region width doesn't seem to matter much
##	distillation sounds about the same at 145 and 291
##	
##notes from trial 2:
##	- 150 Hz resolution sounds very nice, but distilling it
##	at 291 ruins it completely
##	- 270 Hz resolution also sounds okay, but are also ruined 
##	by distilling at 291
##	- HOWEVER 240 sounds pretty good and CAN be distilled at 291
##	without affecting the sound much at all
##
##notes from trial 3:
##	- all these dilated things sound okay, retaining their noisiness
##	- the smoothest (not necessarily best) sounding noise is from the
##	longer (narrower) windows
##	
##trial 4: 
##	- sifting has no discernable effect
##
##trial 5:
##	- it really seems like the window should be wider, give it one
##	more try...
##	- windows 240, 336, and 432 Hz wide produce indistinguishible
##	results, so I conclude that a wider window _could_ be used,
##	but it doesn't seem to make any difference.
##
##Conclusion: use a resolution of 240 Hz and a window as wide as the fundamental
##frequency (291 Hz), or thereabouts, and distill at one partial per harmonic.
##
##from trials import *
##
### use this trial counter to skip over
### eariler trials
##trial = 5
##
##print "running trial number", trial, time.ctime(time.time())
##
##source = 'flute291.aiff'
##
##if trial == 1:
##	resolutions = (150, 200, 240, 260, 270, 280, 290)
##	wmults = ( .9, 1., 1.2, 1.5, 1.8, 2.0, 2.2 )
##	bws = (2000, 4000)
##	funds = ( 291, 145 )
##	for r in resolutions:
##		for mult in wmults:
##			w = r * mult
##			for bw in bws:
##				p = analyze( source, r, w, bw )
##				ofile = 'flute.%i.%i.%ik.aiff'%(r, w, bw/1000)
##				synthesize( ofile, p )
##				for f in funds:
##					p2 = p.copy()
##					harmonicDistill( p2, f )
##					ofile = 'flute.%i.%i.%ik.d%i.aiff'%(r, w, bw/1000, f)
##					synthesize( ofile, p2 )
##					p2 = p.copy()
##					harmonicSift( p2, f )
##					ofile = 'flute.%i.%i.%ik.s%i.aiff'%(r, w, bw/1000, f)
##					synthesize( ofile, p2 )
##					
##if trial == 2:
##	resolutions = (150, 240, 270)
##	wmults = ( 1., 1.5, 2.0 )
##	funds = ( 291, 145 )
##	for r in resolutions:
##		for mult in wmults:
##			w = r * mult
##			p = analyze( source, r, w )
##			ofile = 'flute.%i.%i.aiff'%(r, w)
##			synthesize( ofile, p )
##			for f in funds:
##				p2 = p.copy()
##				harmonicDistill( p2, f )
##				ofile = 'flute.%i.%i.d%i.aiff'%(r, w, f)
##				synthesize( ofile, p2 )
##
##if trial == 3:
##	resolutions = ( 150, 240 )
##	wmults = ( 1., 1.5 )
##	funds = ( 291, 145 )
##	for r in resolutions:
##		for mult in wmults:
##			w = r * mult
##			p = analyze( source, r, w )
##			p = timescale( p, 2.0 )
##			for f in funds:
##				p2 = p.copy()
##				harmonicDistill( p2, f )
##				ofile = 'flute.%i.%i.d%i.T2.aiff'%(r, w, f)
##				synthesize( ofile, p2 )
##				
##if trial == 4:
##	r = 240
##	w = 291
##	f = 291
##	a = loris.Analyzer( r, w )
##	sfile = loris.AiffFile( source )
##	p = a.analyze( sfile.samples(), sfile.sampleRate() )
##	ref = loris.createFreqReference( p, f*.5, f*1.5, 100 )
##	loris.channelize( p, ref, 1 )
##	siftme = p.copy()
##	loris.distill( p )
##	ofilebase = 'flute.%i.%i.dst'%(r, w)
##	synthesize( ofilebase + '.aiff', p )
##	loris.exportSdif( ofilebase + '.sdif', p )
##	loris.exportSpc( ofilebase + '.s.spc', p, 62, 0 )
##	loris.exportSpc( ofilebase + '.e.spc', p, 62, 1 )
##	p = timescale( p, 2.0 )
##	synthesize( ofilebase + '.T2.aiff', p )
##	
##	# do a sifted version
##	p = siftme
##	loris.sift( p )
##	loris.distill( p )
##	ofilebase = 'flute.%i.%i.sft'%(r, w)
##	synthesize( ofilebase + '.aiff', p )
##	loris.exportSdif( ofilebase + '.sdif', p )
##	loris.exportSpc( ofilebase + '.s.spc', p, 62, 0 )
##	loris.exportSpc( ofilebase + '.e.spc', p, 62, 1 )
##	p = timescale( p, 2.0 )
##	synthesize( ofilebase + '.T2.aiff', p )
##	
##if trial == 5:
##	sfile = loris.AiffFile( source )
##	orate = 44100
##	r = 240
##	f = 291
##	wmults = ( 1., 1.4, 1.8 )
##	for mult in wmults:
##		w = r * mult
##		a = loris.Analyzer(r,w)
##		p = a.analyze( sfile.samples(), sfile.sampleRate() )
##		name = 'flute.%i.%i'%(r, w)
##
##		# collated
##		pcollate = p.copy()
##		loris.distill( pcollate )
##		print 'synthesizing raw (collated) %s (%s)'%(name, time.ctime(time.time()))
##		samples = loris.synthesize( pcollate, orate )
##		loris.exportAiff( name + '.raw.aiff', samples, orate )
##		loris.exportSdif( name + '.raw.sdif', pcollate )
##
##		# remove any Partials labeled greater than 512
##		iter = pcollate.begin()
##		end = pcollate.end()
##		while not iter.equals(end):
##			next = iter.next()
##			if iter.partial().label() > 512:
##				pcollate.erase(iter)
##			iter = next
##		loris.exportSpc( name + '.raw.s.spc', pcollate, 62, 0 )
##		loris.exportSpc( name + '.raw.e.spc', pcollate, 62, 1 )
##
##		# distilled
##		ref = loris.createFreqReference( p, f*.5, f*1.5, 100 )
##		loris.channelize( p, ref, 1 )
##		loris.distill( p )
##		print 'synthesizing distilled %s (%s)'%(name, time.ctime(time.time()))
##		samples = loris.synthesize( p, orate )
##		loris.exportAiff( name + '.recon.aiff', samples, orate )
##		loris.exportSdif( name + '.sdif', p )
##
##		# remove any Partials labeled greater than 512
##		iter = p.begin()
##		end = p.end()
##		while not iter.equals(end):
##			next = iter.next()
##			if iter.partial().label() > 512:
##				p.erase(iter)
##			iter = next
##		loris.exportSpc( name + '.s.spc', p, 62, 0 )
##		loris.exportSpc( name + '.e.spc', p, 62, 1 )
##		
