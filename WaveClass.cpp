// WaveClass.cpp: implementation of the WaveClass class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "WaveClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WaveClass::WaveClass()
{
	memset(&RIFFChunkInfo, 0, sizeof(MMCKINFO) ) ;
	memset(&FormatChunkInfo, 0, sizeof(MMCKINFO) ) ;
	memset(&waveFmt, 0, sizeof(WAVEFORMATEX) ) ;
	memset(&DataChunkInfo, 0, sizeof(MMCKINFO) ) ;
	wavData = NULL ;

}

WaveClass::~WaveClass()
{
	if (wavData != NULL)
		delete wavData ;
}


/*	sizeInBytes takes into account 1 or 2 channels	*/
int WaveClass::CreateWavDataBuf(int sizeInBytes)
{
	// create data buffer to store in read data
	if (wavData == NULL) {
		ClearDataBuffer() ;
	}

	wavData = new unsigned char[sizeInBytes];
	if (!wavData) {
		MessageBox(0, "Not enough memory for wave file data!", "Error", 0) ;
		RIFFChunkInfo.cksize = sizeInBytes + 36 ;// - 8 + 44 ;
		DataChunkInfo.cksize = sizeInBytes - 44 ;
		return (0) ;
		//~WaveClass() ;
	}

	RIFFChunkInfo.cksize = sizeInBytes + 36 ;// - 8 + 44 ;
	DataChunkInfo.cksize = sizeInBytes - 44 ;

	return (sizeInBytes) ;
}


int WaveClass::ClearDataBuffer() 
{
	if (wavData != NULL)
	{
		delete wavData ;
		return (1) ;
	}
	else
		return (0) ;
}



int WaveClass::GetWaveDataFromFile(char * filename,  int printWavInfo) 
{
	HMMIO handle ;
//	WaveClass* waveIn ;

//	waveIn = new WaveClass() ;

// Step 1: Open the wave file
	fprintf(stdout,"Opening file %s \n", filename) ;
	handle =  mmioOpen(filename, 0,  MMIO_READ) ;
	if (!handle) {
		fprintf(stdout,"handle = %s \n", handle) ;
		MessageBox(0, "Error opening file.", "Error Message", 0) ;
		exit(-1) ;
	}

	
// Step 2: Read the RIFF chunk
//	memset(&RIFFChunkInfo, 0, sizeof(MMCKINFO) ) ;

	// returns MMSYSERR_NOERROR (= 0) if no error
	int Res = mmioDescend(handle, &(RIFFChunkInfo), 0, MMIO_FINDRIFF);
	if (Res) {
		MessageBox(0, "Error reading RIFF chunk", "Error", 0) ;
		mmioClose(handle, 0);
		exit(-1) ;
	}

	if (printWavInfo) {
		fprintf(stdout, "printWavInfo is set!!!!\n") ;
		fprintf(stdout,"\ndwDataOffset = %d \n", RIFFChunkInfo.dwDataOffset) ;
		fprintf(stdout,"cksize = %d \n", RIFFChunkInfo.cksize) ;
		fprintf(stdout,"dwFlags = %d \n", RIFFChunkInfo.dwFlags) ;
		fprintf(stdout,"fccType = %d \n", RIFFChunkInfo.fccType) ;
		fprintf(stdout,"ckid = %d \n", RIFFChunkInfo.ckid) ;
	}

// Step 3: Read the wave format header chunk
	
	FormatChunkInfo.ckid =  mmioStringToFOURCC("fmt ", 0) ;

	Res = mmioDescend(handle, &(FormatChunkInfo),&(RIFFChunkInfo), MMIO_FINDCHUNK) ;
	if (Res) {
		MessageBox(0, "Error reading wave format header chunk", "Error", 0) ;
		mmioClose(handle, 0);
		exit(-1) ;
	}
	if (printWavInfo) {
		fprintf(stdout,"\ndwDataOffset = %d \n", FormatChunkInfo.dwDataOffset) ;
		fprintf(stdout,"cksize = %d \n", FormatChunkInfo.cksize) ;
		fprintf(stdout,"dwFlags = %d \n", FormatChunkInfo.dwFlags) ;
		fprintf(stdout,"fccType = %d \n", FormatChunkInfo.fccType) ;
		fprintf(stdout,"ckid = %d \n", FormatChunkInfo.ckid) ;
	}

	
	// read the wave header into the waveFmt structure
	mmioRead(handle, (char*)&(waveFmt),	FormatChunkInfo.cksize) ;
	if (printWavInfo) {
		fprintf(stdout,"\nnAvgBytesPerSec = %d \n", waveFmt.nAvgBytesPerSec) ;
		fprintf(stdout,"nChannels = %d \n", waveFmt.nChannels) ;
		fprintf(stdout,"nSamplesPerSec = %d \n", waveFmt.nSamplesPerSec) ;
		fprintf(stdout,"wBitsPerSample = %d \n", waveFmt.wBitsPerSample) ;
		fprintf(stdout,"wFormatTag = %d \n", waveFmt.wFormatTag) ;
		fprintf(stdout,"nBlockAlign = %d \n", waveFmt.nBlockAlign) ;
	}
	

// Step 4: Read the data chunk
// reads file data into buffer named RIFFdata
	mmioAscend(handle, &(FormatChunkInfo), 0) ;  // move out of format chunk
	DataChunkInfo.ckid = mmioStringToFOURCC("data", 0) ;
	mmioDescend(handle, &(DataChunkInfo), &(RIFFChunkInfo), MMIO_FINDCHUNK) ;  // move into data chunk & fill data MMCKINFO structure

// determine data buffer length
//	unsigned int dataSize =	DataChunkInfo.cksize;
	fprintf(stdout,"data is %d bytes in length\n", DataChunkInfo.cksize) ;
		if (printWavInfo) {
		fprintf(stdout,"\ndwDataOffset = %d \n", DataChunkInfo.dwDataOffset) ;
		fprintf(stdout,"cksize = %d \n", DataChunkInfo.cksize) ;
		fprintf(stdout,"dwFlags = %d \n", DataChunkInfo.dwFlags) ;
		fprintf(stdout,"fccType = %d \n", DataChunkInfo.fccType) ;
		fprintf(stdout,"ckid = %d \n", DataChunkInfo.ckid) ;
	}
// create data buffer to store in read date
//	char* RIFFdata = new char[waveIn->DataChunkInfo.cksize];
	CreateWavDataBuf(DataChunkInfo.cksize) ;
//	if (!RIFFdata) {
//		MessageBox(0, "Not enough memory for RIFF file data to be read into", "Error", 0) ;
//		exit(-1) ;
//	}
	mmioRead(handle,(char *)wavData, DataChunkInfo.cksize);
	mmioClose(handle, 0);
	// end step 4

	fprintf(stdout,"mmioFOURCC RIFF = %d \n",  mmioFOURCC('R', 'I', 'F', 'F')) ;
	fprintf(stdout,"mmioFOURCC WAVE = %d \n",  mmioFOURCC('W', 'A', 'V', 'E')) ;
	fprintf(stdout,"mmioFOURCC fmt  = %d \n",  mmioFOURCC('f', 'm', 't', ' ')) ;

	return 1 ;
}



int  WaveClass::WriteWaveDataToFile(char * filename )
{

		int ret ;
		HMMIO handle ;


// Step 1 - Open a new file.
	handle = mmioOpen(filename , 0, MMIO_CREATE | MMIO_WRITE);
	if (!handle) {
		fprintf(stdout,"handle = %s \n", handle) ;
		MessageBox(0, "Step 2 - Error opening file.", "Error Message", 0) ;
		return (0) ;
	}

// Step 2 - Create the RIFF chunk.
	// This function automatically decends into the file to the format chunk
	ret = mmioCreateChunk(handle, &(RIFFChunkInfo), MMIO_CREATERIFF);
	if ( ret != MMSYSERR_NOERROR   ) {
		MessageBox(0, "Step 3 - mmioCreateChunk failed creating RIFF chunk", "Error", 0) ;
		mmioClose(handle, 0);
	  return (0) ;
	}

// Step 3 - Create the fmt chunk.
	ret = mmioCreateChunk(handle, &(FormatChunkInfo), 0);
	if ( ret != MMSYSERR_NOERROR  ) {
		MessageBox(0, "Step 4 - mmioCreateChunk failed creating the FormatChunkInfo chunk", "Error", 0) ;
		mmioClose(handle, 0);
	  return (0) ;
	}

// Step 4 - Write the fmt chunk data. 
	ret = mmioWrite(handle, (char*)&(waveFmt), sizeof(WAVEFORMATEX)-2 );
	if ( ret == -1 ) {
		MessageBox(0, "Step 5 - mmioWrite failed", "Error", 0) ;
		mmioClose(handle, 0);
	  return (0) ;
	}

//	mmioSeek(handle, 2, SEEK_END) ;

// Step 5 -  Ascend out of the format chunk.
	ret = mmioAscend(handle, &(FormatChunkInfo), 0) ;
	if ( ret  != MMSYSERR_NOERROR ) {
		MessageBox(0, "Step 6 - mmioAscend failed to Ascend from format chunk", "Error", 0) ;
		mmioClose(handle, 0);
	  return (0) ; 
	}

// Step 6 - Create the data chunk.
	ret = mmioCreateChunk(handle, &(DataChunkInfo), 0);
	if ( ret  != MMSYSERR_NOERROR ) {
		MessageBox(0, "Step 7 - mmioCreateChunk failed to create data DataChunkInfo chunk", "Error", 0) ;
		mmioClose(handle, 0);
	  return (0) ; 
	}
// Step 7 - Write the data chunk.
	ret = mmioWrite(handle, (char *)wavData, DataChunkInfo.cksize);
	if ( ret  == -1 ) {
		MessageBox(0, "Step 8 - mmioWrite failed to write data to the data chunk", "Error", 0) ;
		mmioClose(handle, 0);
	  return (0)  ; 
	}

// Step 8 - Close the file.
	mmioClose(handle, 0);

	fprintf(stdout,"wave file successfully written to file \n Yay!\n") ;

	return (1) ;
}


char * WaveClass::SetErrorText(int res)
{
	for (int i = 0 ; i == 255; i++)
	{
		errText[i] = 0 ;
	}
	waveOutGetErrorText(res, errText, 256);

	return errText ;
}
