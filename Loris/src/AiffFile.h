#ifndef __Loris_aiff_file__
#define __Loris_aiff_file__

// ===========================================================================
//	AiffFile.h
//	
//
//
//	-kel 28 Sept 99
//
// ===========================================================================

#include "LorisLib.h"
#include "LorisTypes.h"
#include "ieee.h"

Begin_Namespace( Loris )

class SampleBuffer;
class BinaryFile;

// ---------------------------------------------------------------------------
//	��class AiffFile
//
//	The SampleBuffer must be provided by clients; it is not owned by 
//	the AiffFile.
//	
class AiffFile
{
public:
//	construction:
	AiffFile( double rate, int chans, int bits, SampleBuffer & buf, BinaryFile & file );
	
//	defaults are okay for these, but don't subclass
	//	AiffFile( const AiffFile & other );
	//	~AiffFile( void );
	
//	access/mutation:
	double sampleRate( void ) const { return _sampleRate; }
	int numChans( void ) const { return _nChannels; }
	int sampleSize( void ) const { return _sampSize; }
	
	void setSampleRate( double x ) { _sampleRate = x; }
	void setNumChannels( int n ) { _nChannels = n; }
	void setSampleSize( int n ) { _sampSize = n; }
	
	SampleBuffer & samples( void ) { return _samples; }
	const SampleBuffer & samples( void ) const { return _samples; }
	
//	reading and writing:
	void write( void );
	
//	-- chunk types --
private:
	enum { 
		ContainerId = 'FORM', 
		AiffType = 'AIFF', 
		CommonId = 'COMM',
		SoundDataId = 'SSND'
	};
	
	struct CkHeader {
		Int_32 id;
		Uint_32 size;
	};
	
	struct ContainerCk
	{
		CkHeader header;
		Int_32 formType;
	};
	
	struct CommonCk
	{
		CkHeader header;
		Int_16 channels;			// number of channels 
		Int_32 sampleFrames;		// channel independent sample frames 
		Int_16 bitsPerSample;		// number of bits per sample 
		IEEE::extended80 srate;		// sampling rate IEEE 10 byte format 
	};
	
	struct SoundDataCk
	{
		CkHeader header;	
		Uint_32 offset;				
		Uint_32 blockSize;	
		//	sample frames follow
	};
	
//	-- helpers --
	//	reading:
	void readCommon( CommonCk & );
	void readContainer( ContainerCk & );
	void readSampleData( SoundDataCk & );
	
	//	writing:
	void writeCommon( void );
	void writeContainer( void );
	void writeSampleData( void );
	void writeSamples( void );
	
	//	data sizes:
	Uint_32 sizeofCommon( void );
	Uint_32 sizeofCkHeader( void );
	Uint_32 sizeofSoundData( void );

	//	checking parameters:
	void validateParams( void );
	
//	-- instance variables --
	double _sampleRate;	//	in Hz
	int _nChannels;		//	samples per frame, usually one (mono) in Loris
	int _sampSize;		//	in bits
	
	SampleBuffer & _samples;
	BinaryFile & _file;
	
};	//	end of class AiffFile

End_Namespace( Loris )

#endif //	ndef __Loris_aiff_file__