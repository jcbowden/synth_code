// WaveClass.h: interface for the WaveClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVECLASS_H__DE0D1969_6A92_4FE5_A425_C1A91EAE6195__INCLUDED_)
#define AFX_WAVECLASS_H__DE0D1969_6A92_4FE5_A425_C1A91EAE6195__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Windows include files
#include <windows.h>
#include <MMSYSTEM.H>
//#include <mmreg.h> 
#include <dsound.h>


// C/C++ includes files
//#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>		// for file input
#include <string>
#include <vector>
#include <cstdlib>		// equiv to <stdlib.h> - contains atof()*/

// Application include files

using namespace std ;


class WaveClass  
{
public:
	WaveClass();
	~WaveClass();

	int GetWaveDataFromFile(char * filename,  int printWavInfo) ;
	int WriteWaveDataToFile(char * filename) ;
	int CreateWavDataBuf(int sizeInBytes) ;
	int ClearDataBuffer() ;
	

	char * SetErrorText(int res) ;

	MMCKINFO RIFFChunkInfo ;
	MMCKINFO FormatChunkInfo ;
	WAVEFORMATEX waveFmt;
	MMCKINFO DataChunkInfo ; // size of data is in : "DataChunkInfo.cksize"
	unsigned char * wavData ;
	char errText[256] ;
} ;



#endif // !defined(AFX_WAVECLASS_H__DE0D1969_6A92_4FE5_A425_C1A91EAE6195__INCLUDED_)
