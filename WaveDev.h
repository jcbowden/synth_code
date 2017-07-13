// WaveDev.h: interface for the WaveDev class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEDEV_H__5226DF29_52A5_42CE_92B9_2C0F6481E69B__INCLUDED_)
#define AFX_WAVEDEV_H__5226DF29_52A5_42CE_92B9_2C0F6481E69B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSYSTEM.H>
#include <dsound.h>

#include <stdio.h>
#include <iostream>
#include <fstream>		// for file input
#include <string>
#include <vector>
#include <cstdlib>		// equiv to <stdlib.h> - contains atof()

#include "WaveClass.h" // used to get wave data from file to load into secondary buffers

using namespace std ;

struct  SecondaryBuf
{
	string								bufName ;
	IDirectSoundBuffer*		pDSBuff ;
} ;


class WaveDev  
{
public:
	// devNum is sound card to select for I/O
	WaveDev();  
	~WaveDev();

	// uses DSEnumProc() friend function
	int			GetDSDevOutputCaps(int devNum) ;	
	friend	BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID passedPointer ) ;
	 
	void		SelectDSDriver( int driverNum,  int askUser) ;
	void		PrintDevCaps() ;
	// selectedSndCardDrv used as index to lpGuid[] array
	int			DSInitalise( int selectedSndCardDrv ) ;  
	int			printDevCapsFlag  ;

	int			CreatePrimaryBuf( void ) ;
	int			GetBufferCaps( IDirectSoundBuffer*  buf ) ; 
	void		PrintBufferCaps( void ) ;				// Prints capabilities of last buffer sent to GetBufferCaps()
	int			SetBufFormat(IDirectSoundBuffer* buf, int channels, int samplesPerSec, int bitsPerSample) ; // CreatePrimaryBuf() has to be called first
	
	// loads from file "_filename" if wavInput == NULL or loads from wavInput
	int			LoadSecondaryBuffFromWav( string _fileName, WaveClass* wavInput ) ; 
	char *	SetErrorText(int res) ;

	int			selectedSndCardDrv ;
	int			numDrivers ;

	IDirectSoundBuffer* pPrimarySndBuf ;
	// Used to specify buffer capabilities required
	DSBUFFERDESC bufferDesc; 
	// Used to return buffer capabilities given
	DSBCAPS bufferCaps ;					
	/**********************************************************************/
	/* This is array of named DirectSound buffers 
	/* containing sounds that are playable ( _DSBVect[x]->play(x,y,z) ) 
	/**********************************************************************/
	vector<SecondaryBuf*> _DSBVect ;  
	IDirectSound* pDS ;			// Direct Sound Object
	LPGUID	lpGuid[128] ;		// Direct Sound Device ID
	DSCAPS	deviceCaps ;		// Direct Sound device capabilities
	
	WAVEFORMATEX  waveFmt ;				// Used to set primary buffers format (default = 22050Hz, mono, 16bit)

	int			res ;
	char		errText[256] ;
} ;

#endif // !defined(AFX_WAVEDEV_H__5226DF29_52A5_42CE_92B9_2C0F6481E69B__INCLUDED_)
