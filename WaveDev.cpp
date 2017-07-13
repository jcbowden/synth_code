// WaveDev.cpp: implementation of the WaveDev class.
//
//////////////////////////////////////////////////////////////////////

//#include <mmreg.h> 

//#include "stdafx.h"
#include "WaveDev.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WaveDev::WaveDev( )
{
//	GetDSDevOutputCaps( devNum ) ;
//	memset(&deviceCaps, 0, sizeof(DSCAPS) ) ;
	pDS = NULL ;
	printDevCapsFlag = 0 ;
	selectedSndCardDrv = 0 ;
	numDrivers = 0 ;
	memset(lpGuid, 0, 128*sizeof(LPGUID) ) ;
//	memset(&bufferDesc, 0, sizeof(bufferDesc) ) ;
}

WaveDev::~WaveDev()
{
	unsigned long  dwStatus = 0 ;
	int numPlaying = 0;


	do {
		numPlaying = 0 ;
		for  ( int i = 0; i < _DSBVect.size() ; i++)
		{
			_DSBVect[i]->pDSBuff->GetStatus(&dwStatus);
			if (dwStatus & DSBSTATUS_PLAYING)
				numPlaying++ ;
			dwStatus = 0 ;
		}
		Sleep(40);
	} while (numPlaying > 0);



	if (pDS != NULL) 
		pDS->Release() ;
}




int		WaveDev::LoadSecondaryBuffFromWav( string _fileName, WaveClass* wavInput ) 
{
	void * pMem1 = NULL ;
	void * pMem2 = NULL ;
	unsigned long bufSize1, bufSize2 ;
	WaveClass *tempWav ;

	if (wavInput == NULL)
	{
		tempWav = new WaveClass() ;

		tempWav->GetWaveDataFromFile( (char *) _fileName.c_str(), 1) ;
	}
	else
	{
		tempWav = wavInput ;
	}

	SecondaryBuf* tempSecBuf = new SecondaryBuf ;

	tempSecBuf->bufName = _fileName ;
	memset(&bufferDesc,0,sizeof(DSBUFFERDESC)); 
	/// describe buffer required - i.e. Secondary Sound Buffer
	bufferDesc.dwSize = sizeof(DSBUFFERDESC); 
	bufferDesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2 ; 
	bufferDesc.dwBufferBytes = tempWav->DataChunkInfo.cksize ; // from open wave files header
	bufferDesc.lpwfxFormat = &(tempWav->waveFmt); // from the open wave files header

	res = pDS->CreateSoundBuffer( &bufferDesc, &tempSecBuf->pDSBuff, NULL) ;
	if (res != DS_OK ){
		cout << "IDirectSound::CreateSoundBuffer failed creating secondary buffer. Error " << SetErrorText(res)  << endl ;
//		return (res) ;
	}	
	_DSBVect.push_back(tempSecBuf) ;


	// lock buffer
	res = tempSecBuf->pDSBuff->Lock(0,tempWav->DataChunkInfo.cksize,&pMem1,&bufSize1,&pMem2,&bufSize2,0) ;
	if (res != DS_OK ){
		cout << "DirectSoundBuffer->Lock failed. Error " << SetErrorText(res) << endl ;
	}
	// copy data from WaveClass object to DSBuffer
	memmove(pMem1,tempWav->wavData,bufSize1) ;
	if (bufSize2 != 0)
			memmove(pMem2,tempWav->wavData+bufSize1,bufSize2) ;

	// Unlock buffer
	res = tempSecBuf->pDSBuff->Unlock(pMem1,bufSize1,pMem2,bufSize2) ;
	if (res != DS_OK ){
		cout << "DirectSoundBuffer->Unlock failed. Error " << SetErrorText(res) << endl ;
	}

	if (wavInput == NULL)
		delete tempWav ;

	return (res) ;
}



int WaveDev::GetDSDevOutputCaps( int devNum )
{
	res = DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, this) ;
	if (res != DS_OK ){
		cout << "DirectSoundEnumerate failed. Error " << SetErrorText(res) << endl ;
//		return (res) ;
	}	

	return (res) ;
}


BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID passedPointer )
{

	WaveDev * pWavDev = (class WaveDev *) passedPointer ;

	pWavDev->lpGuid[pWavDev->numDrivers++] = lpGUID ;
	
	if (pWavDev->printDevCapsFlag == 1)
	{
		if (lpGUID != NULL) printf("\nDriver ID = %p\n",lpGUID) ;
		cout << "Device Description = " <<  lpszDesc << endl ;
		cout << "Driver Name = "<< lpszDrvName << endl ;
	}

//	pWavDev->DSInitalise( pWavDev->numDrivers ) ;

	if (pWavDev->printDevCapsFlag == 1)
		pWavDev->PrintDevCaps() ;

	return( TRUE );
}


int WaveDev::DSInitalise( int selectedSndCardDrv )
{
	if (pDS != NULL) 
		pDS->Release() ;

	res = DirectSoundCreate( lpGuid[selectedSndCardDrv-1],  &(pDS), 0) ;
	if (res != DS_OK ){
		cout << "IDirectSound::DirectSoundCreate failed. Error " << SetErrorText(res)  << endl ;
//		return (res) ;
	}	

	HWND hWnd = GetForegroundWindow();
	if (hWnd == NULL)
		hWnd = GetDesktopWindow();

	res = pDS->SetCooperativeLevel(hWnd , DSSCL_NORMAL ) ;
	if (res != DS_OK ){
		cout << "IDirectSound::SetCooperativeLevel failed. Error " << SetErrorText(res)  << endl ;
//		return (res) ;
	}	

	memset(&deviceCaps, 0, sizeof(DSCAPS) ) ;
	deviceCaps.dwSize = sizeof(DSCAPS) ;

	res = pDS->GetCaps( &deviceCaps ) ;
	if (res != DS_OK ){
		cout << "IDirectSound::GetCaps failed. Error " << SetErrorText(res)  << endl ;
//		return (res) ;
	}	

	return res ;
}



int	WaveDev::CreatePrimaryBuf( void )
{
	memset(&bufferDesc,0,sizeof(bufferDesc)); 

	/// describe buffer required - i.e. Primary Sound Buffer
	bufferDesc.dwSize = sizeof(DSBUFFERDESC); 
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER; 
	bufferDesc.dwBufferBytes = 0; 
	bufferDesc.lpwfxFormat = NULL; 

	res = pDS->CreateSoundBuffer(&bufferDesc, &pPrimarySndBuf, NULL) ;
	if (res != DS_OK ){
		cout << "IDirectSound::CreateSoundBuffer failed creating primary buffer. Error " << SetErrorText(res)  << endl ;
//		return (res) ;
	}	

	pPrimarySndBuf->Play(0, 0, DSBPLAY_LOOPING);

	return res ;
}


int		WaveDev::GetBufferCaps( IDirectSoundBuffer*  buf )
{

	memset(&bufferCaps,0,sizeof(DSBCAPS));
	bufferCaps.dwSize = sizeof(DSBCAPS) ;

	res = buf->GetCaps(&bufferCaps) ;
	if (res != DS_OK ){
		cout << "IDirectSoundBuffer::GetCaps failed. Error " << SetErrorText(res)  << endl ;
//		return (res) ;
	}	

	return res ;
}


void WaveDev::PrintBufferCaps() 
{
	cout << "\nFlags set on buffer:" << endl ;
	if ( bufferCaps.dwFlags & DSBCAPS_CTRL3D  )
	 cout << "  DSBCAPS_CTRL3D" << endl ;
	if ( bufferCaps.dwFlags & DSBCAPS_CTRLFREQUENCY)
	 cout << "  DSBCAPS_CTRLFREQUENCY" << endl ;
	if ( bufferCaps.dwFlags & DSBCAPS_CTRLPAN ) 
	 cout << "  DSBCAPS_CTRLPAN " << endl ;
	if ( bufferCaps.dwFlags &  DSBCAPS_CTRLVOLUME  )
	 cout << "  DSBCAPS_CTRLVOLUME " << endl ; 
	if ( bufferCaps.dwFlags & DSBCAPS_GETCURRENTPOSITION2  )
	 cout << "  DSBCAPS_GETCURRENTPOSITION2" << endl ;
	if ( bufferCaps.dwFlags & DSBCAPS_GLOBALFOCUS)
	 cout << "  DSBCAPS_GLOBALFOCUS" << endl ;
	if ( bufferCaps.dwFlags & DSBCAPS_LOCHARDWARE ) 
	 cout << "  DSBCAPS_LOCHARDWARE" << endl ;
	if ( bufferCaps.dwFlags &  DSBCAPS_LOCSOFTWARE   )
	 cout << "  DSBCAPS_LOCSOFTWARE " << endl ; 
	if ( bufferCaps.dwFlags & DSBCAPS_MUTE3DATMAXDISTANCE   )
	 cout << "  DSBCAPS_MUTE3DATMAXDISTANCE " << endl ;
	if ( bufferCaps.dwFlags & DSBCAPS_PRIMARYBUFFER )
	 cout << "  DSBCAPS_PRIMARYBUFFER " << endl ;
	if ( bufferCaps.dwFlags & DSBCAPS_STATIC ) 
	 cout << "  DSBCAPS_STATIC" << endl ;
	if ( bufferCaps.dwFlags &  DSBCAPS_STICKYFOCUS   )
	 cout << "  DSBCAPS_STICKYFOCUS " << endl ; 

	
	cout << "  bufferCaps.dwBufferBytes " << bufferCaps.dwBufferBytes << endl ;
	cout << "  bufferCaps.dwUnlockTransferRate " << bufferCaps.dwUnlockTransferRate << endl ;
	cout << "  bufferCaps.dwPlayCpuOverhead = " << bufferCaps.dwPlayCpuOverhead <<  endl ;

}



int WaveDev::SetBufFormat(IDirectSoundBuffer* buf, int channels, int samplesPerSec, int bitsPerSample) 
{

	waveFmt.wFormatTag = WAVE_FORMAT_PCM ; 
	waveFmt.nChannels = channels ; 
	waveFmt.nSamplesPerSec = samplesPerSec ; 
	waveFmt.wBitsPerSample = bitsPerSample ; 
	waveFmt.nBlockAlign = (waveFmt.wBitsPerSample / 8) * waveFmt.nChannels ; 
	waveFmt.nAvgBytesPerSec = waveFmt.nSamplesPerSec * waveFmt.nBlockAlign ; 

	res = buf->SetFormat( &waveFmt ) ;
	if (res != DS_OK ){
		cout << "IDirectSoundBuffer::SetFormat failed. Error " << SetErrorText(res)  << endl ;
//		return (res) ;
	}	

	return res ;
}


void WaveDev::SelectDSDriver( int driverNum, int askUser)
{
	if (askUser == 1)
	{
		cout << "\nThere are " << numDrivers << " sound devices in system!\nPlease select driver: " << endl;

		for (int i = 0; i < numDrivers; i++) {
			cout << "Driver " << i+1 <<" = " << lpGuid[i]  << endl  ;
		}
		while ((selectedSndCardDrv > numDrivers) || (selectedSndCardDrv < 1) )
			cin >> selectedSndCardDrv ;
	}
	else
		selectedSndCardDrv = driverNum ;

	DSInitalise(selectedSndCardDrv) ;
	CreatePrimaryBuf() ;
}





void WaveDev::PrintDevCaps()
{

	printf("\ndeviceCaps.dwFlags = %d\n", deviceCaps.dwFlags ) ;
	if ( deviceCaps.dwFlags & DSCAPS_CERTIFIED )
	 printf("DSCAPS_CERTIFIED  \n" ) ;
	if ( deviceCaps.dwFlags & DSCAPS_CONTINUOUSRATE)
	 printf("DSCAPS_CONTINUOUSRATE... max = %d, min = %d \n",  deviceCaps.dwMaxSecondarySampleRate, deviceCaps.dwMinSecondarySampleRate ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_EMULDRIVER ) 
	 printf("DSCAPS_EMULDRIVER flag indicates no DirectSound driver for audio device\n" ) ; 
	if ( deviceCaps.dwFlags &  DSCAPS_PRIMARY16BIT  )
	 printf("DSCAPS_PRIMARY16BIT \n" ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_PRIMARY8BIT )
	 printf("DSCAPS_PRIMARY8BIT \n" ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_PRIMARYMONO )
	 printf("DSCAPS_PRIMARYMONO \n" ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_PRIMARYSTEREO )
	 printf("DSCAPS_PRIMARYSTEREO \n" ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_SECONDARY16BIT)
	 printf("DSCAPS_SECONDARY16BIT \n" ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_SECONDARY8BIT )
	 printf("DSCAPS_SECONDARY8BIT \n" ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_SECONDARYMONO)
	 printf("DSCAPS_SECONDARYMONO\n" ) ; 
	if ( deviceCaps.dwFlags & DSCAPS_SECONDARYSTEREO)
	 printf("DSCAPS_SECONDARYSTEREO\n" ) ; 

	printf("deviceCaps.dwMinSecondarySampleRate %d \n", deviceCaps.dwMinSecondarySampleRate ) ;
	printf("deviceCaps.dwMaxSecondarySampleRate %d \n", deviceCaps.dwMaxSecondarySampleRate ) ;
	printf("deviceCaps.dwPrimaryBuffers %d \n", deviceCaps.dwPrimaryBuffers ) ;
	printf("deviceCaps.dwMaxHwMixingAllBuffers %d \n", deviceCaps.dwMaxHwMixingAllBuffers ) ;
	printf("deviceCaps.dwMaxHwMixingStaticBuffers = %d \n", deviceCaps.dwMaxHwMixingStaticBuffers ) ;
	
	printf("deviceCaps.dwMaxHwMixingStreamingBuffers = %d\n", deviceCaps.dwMaxHwMixingStreamingBuffers ) ;
	printf("deviceCaps.dwFreeHwMixingAllBuffers %d \n", deviceCaps.dwFreeHwMixingAllBuffers ) ;
	printf("deviceCaps.dwFreeHwMixingStaticBuffers %d \n", deviceCaps.dwFreeHwMixingStaticBuffers ) ;
	printf("deviceCaps.dwFreeHwMixingStreamingBuffers %d \n", deviceCaps.dwFreeHwMixingStreamingBuffers ) ;
	printf("deviceCaps.dwMaxHw3DAllBuffers %d \n", deviceCaps.dwMaxHw3DAllBuffers ) ;
	printf("deviceCaps.dwMaxHw3DStaticBuffers = %d \n", deviceCaps.dwMaxHw3DStaticBuffers ) ;
	printf("deviceCaps.dwMaxHw3DStreamingBuffers = %d\n", deviceCaps.dwMaxHw3DStreamingBuffers ) ;
	printf("deviceCaps.dwFreeHw3DAllBuffers %d \n", deviceCaps.dwFreeHw3DAllBuffers ) ;
	printf("deviceCaps.dwFreeHw3DStaticBuffers %d \n", deviceCaps.dwFreeHw3DStaticBuffers ) ;
	printf("deviceCaps.dwFreeHw3DStreamingBuffers %d \n", deviceCaps.dwFreeHw3DStreamingBuffers ) ;
	printf("deviceCaps.dwTotalHwMemBytes %d \n", deviceCaps.dwTotalHwMemBytes ) ;
	printf("deviceCaps.dwFreeHwMemBytes = %d \n", deviceCaps.dwFreeHwMemBytes ) ;
	printf("deviceCaps.dwMaxContigFreeHwMemBytes %d \n", deviceCaps.dwMaxContigFreeHwMemBytes ) ;
	printf("deviceCaps.dwUnlockTransferRateHwBuffers %d \n", deviceCaps.dwUnlockTransferRateHwBuffers ) ;
	printf("deviceCaps.dwPlayCpuOverheadSwBuffers %d \n", deviceCaps.dwPlayCpuOverheadSwBuffers ) ;
	printf("deviceCaps.dwReserved1 %d \n", deviceCaps.dwReserved1 ) ;
	printf("deviceCaps.dwReserved2 = %d \n", deviceCaps.dwReserved2 ) ;


}



char * WaveDev::SetErrorText(int res)
{
	for (int i = 0 ; i == 255; i++)
	{
		errText[i] = 0 ;
	}
	if (res == DS_OK )
	 strcpy(errText, "The request completed successfully.\n" );
	if (res == DSERR_ALLOCATED   )
	 strcpy(errText, "The request failed because resources, such as a priority level, were already in use by another caller.\n" );  
	if (res == DSERR_ALREADYINITIALIZED  ) 
	 strcpy(errText, "The object is already initialized. \n" ); 
	if (res == DSERR_BADFORMAT )  
	 strcpy(errText, "The specified wave format is not supported.  \n" );
	if (res == DSERR_BUFFERLOST  ) 
	 strcpy(errText, "The buffer memory has been lost and must be restored.\n" );  
	if (res == DSERR_CONTROLUNAVAIL  ) 
	 strcpy(errText, "The control (volume, pan, and so forth) requested by the caller is not available.\n" );  
	if (res == DSERR_GENERIC  ) 
	 strcpy(errText, "An undetermined error occurred inside the DirectSound subsystem.\n" );  
	if (res == DSERR_INVALIDCALL   )
	 strcpy(errText, "This function is not valid for the current state of this object. \n" ); 
	if (res == DSERR_INVALIDPARAM  ) 
	 strcpy(errText, "An invalid parameter was passed to the returning function. \n" ); 
	if (res == DSERR_NOAGGREGATION   )
	 strcpy(errText, "The object does not support aggregation.\n" ); 
	if (res == DSERR_NODRIVER  ) 
	 strcpy(errText, "No sound driver is available for use. \n" ); 
	if (res == DSERR_NOINTERFACE  ) 
	 strcpy(errText, "The requested COM interface is not available.  \n" );
	if (res == DSERR_OTHERAPPHASPRIO   )
	 strcpy(errText, "Another application has a higher priority level, preventing this call from succeeding  \n" );
	if (res == DSERR_OUTOFMEMORY  ) 
	 strcpy(errText, "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request.  \n" );
	if (res == DSERR_PRIOLEVELNEEDED  ) 
	 strcpy(errText, "The caller does not have the priority level required for the function to succeed. \n" ); 
	if (res == DSERR_UNINITIALIZED  ) 
	 strcpy(errText, "The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called. \n" ); 
	if (res == DSERR_UNSUPPORTED  ) 
	 strcpy(errText, "The function called is not supported at this time. \n" ); 


	return errText ;
}