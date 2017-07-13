// SndSynth.cpp: implementation of the SndSynth class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "SndSynth.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SndSynth::SndSynth(WaveForm wavForm, float baseFreq, vector<xy> xyInput)
{
	CreateNoteTable() ;
	_wavForm = wavForm ;
//	_finalEnvelope(xyInput) ;

}

SndSynth::~SndSynth()
{

}

/*	8-bit samples are stored as unsigned bytes, ranging from 0 to 255. 
		16-bit samples are stored as 2's-complement signed integers, ranging from -32768 to 32767
*/
int SndSynth::SetUpHeader( SECONDS length, int channels, SampleRate samplesPerSec, int bitsPerSample )
{

		// Start of header information
		_wavFile.waveFmt.nAvgBytesPerSec		= channels * samplesPerSec * (bitsPerSample/8) ;
		_wavFile.waveFmt.nChannels					= channels ;
		_wavFile.waveFmt.nSamplesPerSec			= samplesPerSec ;
		_wavFile.waveFmt.wBitsPerSample			= bitsPerSample ;
		_wavFile.waveFmt.wFormatTag					= WAVE_FORMAT_PCM ; // == 1 
		_wavFile.waveFmt.nBlockAlign				= (_wavFile.waveFmt.wBitsPerSample / 8) * _wavFile.waveFmt.nChannels ;

		_wavFile.DataChunkInfo.dwDataOffset		= 44 ;
		_wavFile.DataChunkInfo.cksize					=	length * _wavFile.waveFmt.nAvgBytesPerSec ;
		_wavFile.DataChunkInfo.dwFlags				= 0 ;
		_wavFile.DataChunkInfo.fccType				= 0 ;
		_wavFile.DataChunkInfo.ckid						=	mmioFOURCC('d', 'a', 't', 'a');

		_wavFile.RIFFChunkInfo.dwDataOffset		= 8 ;
		_wavFile.RIFFChunkInfo.cksize					=	_wavFile.DataChunkInfo.cksize + 36;
		_wavFile.RIFFChunkInfo.dwFlags				= 0 ;
		_wavFile.RIFFChunkInfo.fccType				= mmioFOURCC('W', 'A', 'V', 'E') ;
		_wavFile.RIFFChunkInfo.ckid						=	mmioFOURCC('R', 'I', 'F', 'F') ;

		_wavFile.FormatChunkInfo.dwDataOffset	= 20 ;
		_wavFile.FormatChunkInfo.cksize				=	16 ;
		_wavFile.FormatChunkInfo.dwFlags			= 0 ;
		_wavFile.FormatChunkInfo.fccType			= 0 ;
		_wavFile.FormatChunkInfo.ckid					=	mmioFOURCC('f', 'm', 't', ' ') ;
		// End of header information 

		// This is the important bit for data storage
		_wavFile.CreateWavDataBuf(_wavFile.DataChunkInfo.cksize) ;	

	return (1) ;
}


int GetNumNotes( void ) 
{
	return ( NUMBEROFOCTAVES * NOTESPEROCTAVE ) ;
}

// returns value between 0..1 
double SndSynth::Sawtooth( double freq, double time )
{

	double tdouble ;
	long int tint ;

	tdouble = time * freq ;  // how many cycles
	tint = (long int) tdouble ;  // how many full cycles

	tdouble = (double) (tdouble - tint) ;
//	tdouble = tdouble*2 ;  // scale from -1...1 
//	tdouble -= 1 ;

	return (tdouble) ;
}

// returns value between 0..1 
double SndSynth::Square( double freq, double time )
{

	double tdouble ;
	long int tint ;

	tdouble = time * freq ;  // how many cycles
	tint = (long int) tdouble ;  // how many full cycles

	tdouble = (double) (tdouble - tint) ;
//	tdouble = tdouble*2 ;  // scale from -1...1 
//	tdouble -= 1 ;
	if (tdouble < 0.5)
		tdouble = 0.0 ;
	else
		tdouble = 1.0 ;

//	cout << tdouble << endl ;

	return (tdouble) ;
}


int SndSynth::LinearFreqRampMultiplesSawtooth(double startf, double finf, double freq_ramp )
{
	// linear ramp from initial freq to final freq from specified start time
	double inctime = 1.0 / _wavFile.waveFmt.nSamplesPerSec ;
//	double freq_ramp = 55  ;
//	double startf  = 55 ;
//	double finf = 220 ;
	double time ;
	double slope = ( startf-finf ) * -freq_ramp ; ;
	double tfreq = startf ; 

	if (_wavFile.waveFmt.wBitsPerSample == 16)
	{
		__int16 tint = 0 ;
		__int16 * ptint ;
		ptint = (__int16 *) _wavFile.wavData ;

//		FILE* outfile = fopen("output.txt","w") ;
		if (_wavFile.waveFmt.nChannels == 1 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/2) ; i++)
			{
				time = i * inctime ;
				if (this->_wavForm == SINE)
					tint = 32768.0 * ( sin(tfreq* PI2 * time) );
				else if (this->_wavForm == SAWTOOTH)
					tint = (Sawtooth(tfreq,time) * 65536 ) - 32768;
				else if (this->_wavForm == SQUARE)
					tint = (Square(tfreq,time) * 65536 ) - 32768;
				*(ptint) = tint ;
				ptint++ ;

		//		fprintf(outfile,"time = %f,  frequency = %f\n",time, tfreq) ;

			
			//	this moves like a saw tooth
				if (tfreq <= finf)
					tfreq += (slope * inctime) ;
				else 
					tfreq = startf ; 

		  /*//	this goes up in freq then down cyclically
				if (tfreq <= finf && tfreq >= startf)
					tfreq += (slope * inctime) ;
				else 
				{
					slope = slope * -1 ;
					tfreq += (slope * inctime) ;
				}*/

			}
//			fclose(outfile) ;
		}
		else // if (channels == 2 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/4) ; i++)
			{
				if ((ptint - (__int16 *)_wavFile.wavData) < _wavFile.DataChunkInfo.cksize)
				{
			/*		tint = 32768.0 * ( sin(i*const_stuff) );
					*(ptint) = tint ;
					ptint++ ;  // left channel
					tint = 32768.0 * ( sin(i*const_stuff/8) );
					*(ptint) = tint ;
					ptint++ ;  // right channel */
				}
			}
		}
	}
	return 1 ;
}

int SndSynth::LinearFreqRampMultiplesHilltop(double startf, double finf, double freq_ramp )
{
	// linear ramp from initial freq to final freq from specified start time
	double inctime = 1.0 / _wavFile.waveFmt.nSamplesPerSec ;
//	double freq_ramp = 55  ;
//	double startf  = 55 ;
//	double finf = 220 ;
	double time ;
	double slope = ( startf-finf ) * -freq_ramp ; ;
	double tfreq = startf ; 

	if (_wavFile.waveFmt.wBitsPerSample == 16)
	{
		__int16 tint = 0 ;
		__int16 * ptint ;
		ptint = (__int16 *) _wavFile.wavData ;

//		FILE* outfile = fopen("output.txt","w") ;
		if (_wavFile.waveFmt.nChannels == 1 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/2) ; i++)
			{
				time = i * inctime ;
				tint = 32768.0 * ( sin(tfreq* PI2 * time) );
				*(ptint) = tint ;
				ptint++ ;

		//		fprintf(outfile,"time = %f,  frequency = %f\n",time, tfreq) ;

			
		/*	//	this moves like a saw tooth
				if (tfreq <= finf)
					tfreq += (slope * inctime) ;
				else 
					tfreq = startf ; */

		  //	this goes up in freq then down cyclically
				if (tfreq <= finf && tfreq >= startf)
					tfreq += (slope * inctime) ;
				else 
				{
					slope = slope * -1 ;
					tfreq += (slope * inctime) ;
				}

			}
//			fclose(outfile) ;
		}
		else // if (channels == 2 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/4) ; i++)
			{
				if ((ptint - (__int16 *)_wavFile.wavData) < _wavFile.DataChunkInfo.cksize)
				{
			/*		tint = 32768.0 * ( sin(i*const_stuff) );
					*(ptint) = tint ;
					ptint++ ;  // left channel
					tint = 32768.0 * ( sin(i*const_stuff/8) );
					*(ptint) = tint ;
					ptint++ ;  // right channel */
				}
			}
		}
	}
	return 1 ;
}




int SndSynth::LinearFreqRamp(double fstart, double ffinish, double start_t, double fin_t )
{
	// linear ramp from initial freq to final freq from specified start time
	double inctime = 1.0 / _wavFile.waveFmt.nSamplesPerSec ;

	double time ;
	double slope = (fstart - ffinish )/( start_t - fin_t) ;
	double tfreq ; 

	if (_wavFile.waveFmt.wBitsPerSample == 16)
	{
		__int16 tint = 0 ;
		__int16 * ptint ;
		ptint = (__int16 *) _wavFile.wavData ;

		//FILE* outfile = fopen("output.txt","w") ;
		if (_wavFile.waveFmt.nChannels == 1 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/2) ; i++)
			{
				time = i * inctime ;
				if (time < start_t)
					tfreq =  fstart ;
				else if (time >= start_t && time < fin_t)
					tfreq = (fstart + (slope*(time-start_t))) ;
				else // time > fin_t 
					tfreq =  ffinish ;

		//		fprintf(outfile,"time = %f,  frequency = %f\n",time, tfreq) ;

				tint = 32768.0 * ( sin(tfreq* PI2 * time) );
				*(ptint) = tint ;
				ptint++ ;
			}
		//	fclose(outfile) ;
		}
		else // if (channels == 2 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/4) ; i++)
			{
				if ((ptint - (__int16 *)_wavFile.wavData) < _wavFile.DataChunkInfo.cksize)
				{
			/*		tint = 32768.0 * ( sin(i*const_stuff) );
					*(ptint) = tint ;
					ptint++ ;  // left channel
					tint = 32768.0 * ( sin(i*const_stuff/8) );
					*(ptint) = tint ;
					ptint++ ;  // right channel */
				}
			}
		}
	}
	return 1 ;
}







int SndSynth::SineOscilatingSineWaveform(double oscFreq, double upper, double lower )
{
	double inctime = 1.0 / _wavFile.waveFmt.nSamplesPerSec ;
	double const_stuff = inctime*PI2*oscFreq ;
	double tfreq ;
	
	if (_wavFile.waveFmt.wBitsPerSample == 16)
	{
		__int16 tint = 0 ;
		__int16 * ptint ;
		ptint = (__int16 *) _wavFile.wavData ;


		if (_wavFile.waveFmt.nChannels == 1 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/2) ; i++)
			{
				tfreq = PI*i*inctime*( lower + upper + (sin(i*const_stuff)*(upper-lower)) ) ;
				tint = 32768.0 * ( sin(tfreq) );
				*(ptint) = tint ;
				ptint++ ;
			}
		}
		else // if (channels == 2 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/4) ; i++)
			{
				if ((ptint - (__int16 *)_wavFile.wavData) < _wavFile.DataChunkInfo.cksize)
				{
					tfreq = PI*i*inctime*( lower + upper + (sin(i*const_stuff)*(upper-lower)) ) ;
					tint = 32768.0 * ( sin(tfreq) );
					*(ptint) = tint ; // left channel
					ptint++ ;  
					tfreq = PI*i*inctime*( lower + upper + (sin(i*const_stuff)*(upper-lower)) ) ;
					tint = 32768.0 * ( sin(tfreq) );
					*(ptint) = tint ; // right channel
					ptint++ ;  
				}
			}
		}
	}
	return 1 ;
}


int SndSynth::ConstFreqWaveform(double freq)
{
	double inctime = 1.0 / _wavFile.waveFmt.nSamplesPerSec ;
	double const_stuff = inctime*PI2*freq ;
	
	if (_wavFile.waveFmt.wBitsPerSample == 16)
	{
		__int16 tint = 0 ;
		__int16 * ptint ;
		ptint = (__int16 *) _wavFile.wavData ;


		if (_wavFile.waveFmt.nChannels == 1 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/2) ; i++)
			{
				if (this->_wavForm == SINE)
					tint = 32768.0 * ( sin(i*const_stuff) ) ;
				else if (this->_wavForm == SAWTOOTH)
					tint = (Sawtooth(freq,i*inctime) * 65536 ) - 32768 ;
				else if (this->_wavForm == SQUARE)
					tint = (Square(freq,i*inctime) * 65536 ) - 32768;
				*(ptint) = tint ;
				ptint++ ;
			}
		}
		else // if (channels == 2 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/4) ; i++)
			{
				if ((ptint - (__int16 *)_wavFile.wavData) < _wavFile.DataChunkInfo.cksize)
				{
					if (this->_wavForm == SINE)
						tint = 32768.0 * ( sin(i*const_stuff) );  
					else if (this->_wavForm == SAWTOOTH)
						tint = (Sawtooth(freq,i*inctime) * 65536 ) - 32768 ;
					else if (this->_wavForm == SQUARE)
						tint = (Square(freq,i*inctime) * 65536 ) - 32768;

					*(ptint) = tint ; // left channel
					ptint++ ;  
					*(ptint) = tint ; // right channel
					ptint++ ;
	  
				}
			}
		}
	}

	return 1 ;
}

int SndSynth::ConstFreqFeedbackWaveform(double freq)
{
	double inctime = 1.0 / _wavFile.waveFmt.nSamplesPerSec ;
	double const_stuff = inctime*PI2*freq ;
	
	if (_wavFile.waveFmt.wBitsPerSample == 16)
	{
		__int16 tint = 0 ;
		double tdoub = 0 ;
		double tdoub2 = 0 ;
		__int16 * ptint ;
		ptint = (__int16 *) _wavFile.wavData ;


		if (_wavFile.waveFmt.nChannels == 1 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/2) ; i++)
			{
				if (this->_wavForm == SINE)
				{
					tdoub = sin (i*const_stuff/2) ;
					tdoub2 = sin (i*80*inctime*PI2) ;
					tdoub =  sin(   (i*const_stuff) + ((i/1000*tdoub) + (tdoub2))  /*+ ((i/10000)*tdoub)*/ ) ;
				//	tdoub =  sin(   (i*const_stuff) + ((i/1000)*tdoub)  ) ;
					tint = 32768.0 * tdoub ;
				//	cout << tdoub << "    " << tint << endl; 
				}
				else if (this->_wavForm == SAWTOOTH)
					tint = (Sawtooth(freq,i*inctime) * 65536 ) - 32768 ;
				else if (this->_wavForm == SQUARE)
					tint = (Square(freq,i*inctime) * 65536 ) - 32768;
				*(ptint) = tint ;
				ptint++ ;
			}
		}
		else // if (channels == 2 )
		{
			for (int i = 0 ; i < (_wavFile.DataChunkInfo.cksize/4) ; i++)
			{
				if ((ptint - (__int16 *)_wavFile.wavData) < _wavFile.DataChunkInfo.cksize)
				{
					if (this->_wavForm == SINE)
						tint = 32768.0 * ( sin(i*const_stuff) );  
					else if (this->_wavForm == SAWTOOTH)
						tint = (Sawtooth(freq,i*inctime) * 65536 ) - 32768 ;
					else if (this->_wavForm == SQUARE)
						tint = (Square(freq,i*inctime) * 65536 ) - 32768;

					*(ptint) = tint ; // left channel
					ptint++ ;  
					*(ptint) = tint ; // right channel
					ptint++ ;
	  
				}
			}
		}
	}

	return 1 ;
}



int SndSynth::InputFrequencyDataFromKeyboard( void )
{
	Frequency tfreq ;
	_freq.push_back( tfreq) ;

	cout << "Enter Length of sound synthesis (seconds): "  ;
	cin >> _sampleLength ;

	
	PrintTableOfNoteFrequencies() ;
	cout << "Enter base frequency: "  ;
	cin.clear(0) ; 
	cin >>  _freq[0]._freq ;

	cout << "_freq[0]._freq = " << _freq[0]._freq << endl ;

//	_freq[0]._wavForm = EnterWaveForm() ;

//	EnterEnvelopeData( &(_freq[0]._envelope) ) ;

	return (1) ;
}


WaveForm  SndSynth::EnterWaveForm( void ) 
{
	int input = -1 ;
	while ( input == -1 )
	{
		cout << "Enter Waveform 1 - 6 "  << endl ;
		cout << " 1/ SINE, 2/ SAWTOOTH, 3/ SQUARE, 4/ SINEFUZZ, 5/ SAWFUZZ, 6/ SQUAREFUZZ "  << flush;
		cin.clear(0) ; 
		cin >>  input ;
		cout << endl << input << endl ;
		switch( input ) 
		{
			case 1:
					return SINE ;
				break ;
			case 2:
					return SAWTOOTH ;
					break ;
			case 3:
					return SQUARE ;
					break ;
			case 4:
					return SINEFUZZ ;
					break ;
			case 5:
					return SAWFUZZ ;
					break ;
			case 6:
					return SQUAREFUZZ ;
					break ;
			default :
					input = -1 ; 
		}	
	}
}

void SndSynth::CreateNoteTable( void ) 
{
	double last = 27.5 ;
	double ratio = pow(2.0,0.08333333333) ;
	int i, j ;

	// this creates ratios of frequencies for
	// "equal temperament" tuning with a = 440Hz
	for ( i = 0 ; i < NUMBEROFOCTAVES; i++)
	{
		for ( j = 0 ; j < NOTESPEROCTAVE; j++)
		{
			if (i == 0 && j==0)
				noteTable[i][j] = last ;
			else
				noteTable[i][j] = last * ratio ;

			last = noteTable[i][j] ;
		}
	}
}


void SndSynth::PrintTableOfNoteFrequencies( void ) 
{

	int i, j ;
	string notes[] = {"a", "a#", "b", "c", "c#", "d", "d#", "e", "f", "f#", "g", "g#"} ;

	// This prints the table to stdout
	cout << setiosflags(ios::fixed) ;
	cout << setiosflags(ios::showpoint) ;
	cout << setiosflags(ios::right) ;
	cout.precision(1) ;
	cout << endl ;

	for ( i = 0 ; i < NOTESPEROCTAVE ; i++)
	{
		cout << setw(2) << notes[i] << " |" ;
		for ( j = 0 ; j < NUMBEROFOCTAVES ; j++)
		{
			if (j < 2)
				cout << " " << setw(5) << noteTable[j][i] ;
			else if (j < 6)
				cout << " " << setw(6) << noteTable[j][i] ;
			else
				cout << " " << setw(7) << noteTable[j][i] ;

		}
		cout << endl ;
	}

}

void SndSynth::EnterEnvelopeData( Envelope* envelope ) 
{
	envelope->EnterEnvelopeData() ;
}


int SndSynth::LoadFrequencyDataFromFile(string filename)
{
	_wavFile.GetWaveDataFromFile((char *)filename.c_str(),0) ;
	return (1) ;
}


int SndSynth::SaveWaveFile(string filename) 
{
	int ret ;
	ret = _wavFile.WriteWaveDataToFile( (char *) filename.c_str() ) ;
	return (ret) ;
}

int SndSynth::ReverseData()
{
	int div ;
	// Reverse the wave data.
	// Create the new data buffer.
 	unsigned char* data2 = new unsigned char[_wavFile.DataChunkInfo.cksize];
	if (data2 == NULL)
		return (0) ;

	if (_wavFile.waveFmt.wBitsPerSample = 16) div = 2 ;
	else div = 1 ;

	// Copy the original data into the new buffer in reverse order.
	for (unsigned int i=0; i< _wavFile.DataChunkInfo.cksize / div; i+=(div)) {
		data2[(_wavFile.DataChunkInfo.cksize/div) - i] = _wavFile.wavData[i];
		if (div == 1)
			data2[(_wavFile.DataChunkInfo.cksize/div) - i-1 ] = _wavFile.wavData[i+1];
		}
	_wavFile.ClearDataBuffer() ;
	_wavFile.wavData = data2 ;

	return (1) ;
}