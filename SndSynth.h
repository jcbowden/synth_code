// SndSynth.h: interface for the SndSynth class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNDSYNTH_H__50CFB608_DD69_425C_850B_588CF8BA04E7__INCLUDED_)
#define AFX_SNDSYNTH_H__50CFB608_DD69_425C_850B_588CF8BA04E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// C/C++ includes files
#include <math.h>
#include <iostream>
#include <fstream>		// for file input
#include <string>
#include <vector>
#include <cstdlib>		// equiv to <stdlib.h> - contains atof()*/
#include <iomanip>

#define PI2 6.283185307
#define PI  3.141592654
#define NUMBEROFOCTAVES 4
#define NOTESPEROCTAVE  12

#include "WaveClass.h"
#include "WaveDev.h"
#include "Envelope.h"


using namespace std ;


enum WaveForm
{
	SINE, SAWTOOTH, SQUARE, SINEFUZZ, SAWFUZZ, SQUAREFUZZ
}  ;

enum SampleRate
{
	_44k = 44100,
	_22k  = 22050,
	_11k  = 11025
} ;

#define SECONDS float 



struct Frequency {
	double _freq ;
	enum WaveForm	_wavForm ;
	Envelope _envelope ;
	float _inclusion ;  // values of 0.0-1.0 govern % of inclusion in waveform of each overtone
} ;


class SndSynth  
{
public:
	SndSynth(WaveForm _wavForm, float baseFreq, vector<xy> xyInput);
	~SndSynth();

	WaveForm	_wavForm ;
	float _sampleLength ;
	vector<Frequency> _freq ;
	Envelope _finalEnvelope ;

	WaveClass _wavFile ;  // this contains buffer for puting sound data into

	// SetUpHeader has to be called before any of the frequency modulation files
	int SetUpHeader( SECONDS length, int channels, SampleRate samplesPerSec, int bitsPerSample) ;
	int InputFrequencyDataFromKeyboard( void ) ;
//	int SaveFrequencyDataToFile(string filename) ;
	int LoadFrequencyDataFromFile(string filename) ;
	int SaveWaveFile(string filename) ;


	void CreateNoteTable( void ) ;
	void PrintTableOfNoteFrequencies() ;
	WaveForm EnterWaveForm( void ) ;

	// frequency modulation production functions
	int ConstFreqWaveform(double freq) ;  // SINE or SAWTOOTH
	int ConstFreqFeedbackWaveform(double freq) ;
	int SineOscilatingSineWaveform( double oscFreq, double upper, double lower ) ;
	int LinearFreqRamp(double fstart, double ffinish, double start_t, double fin_t ) ;
	int LinearFreqRampMultiplesSawtooth(double startf, double finf, double freq_ramp ) ;
	int LinearFreqRampMultiplesHilltop(double startf, double finf, double freq_ramp ) ;

	int ReverseData() ;

	// waveform production
	double Sawtooth( double freq, double time ) ;
	double Square( double freq, double time ) ;

	int GetNumNotes( void ) ; // returns number of notes produced 
	double noteTable[NUMBEROFOCTAVES][NOTESPEROCTAVE] ;

private :
	void EnterEnvelopeData( Envelope* envelope ) ;

	
};

#endif // !defined(AFX_SNDSYNTH_H__50CFB608_DD69_425C_850B_588CF8BA04E7__INCLUDED_)
