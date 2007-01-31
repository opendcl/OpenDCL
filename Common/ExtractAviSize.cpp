// ExtractAviSize.cpp : implementation file
//

#include "stdafx.h"
#include "ExtractAviSize.h"

#include <vfw.h>


/////////////////////////////////////////////////////////////////////////////
// ExtractAviSize

CSize ExtractAviSize(CString sFileName)
{
	CSize sz(-1,-1);
	PAVIFILE aviFile = NULL;
	PAVISTREAM aviStream;

    AVIFileInit(); //Intitialize AVI library
    
    //Open the file
    int result = AVIFileOpen(
        &aviFile,
        sFileName,
        OF_SHARE_DENY_WRITE, 0);
    
	if (result != 0)
		return sz;

    //Get the video stream
    result = AVIFileGetStream(
        aviFile,
        &aviStream,
        streamtypeVIDEO, 0);

	//get start position and count of frames
	long firstFrame = AVIStreamStart(aviStream);
	long countFrames = AVIStreamLength(aviStream);

	//get header information            
	AVISTREAMINFO streamInfo;// = new AVISTREAMINFO();
	result = AVIStreamInfo(aviStream, &streamInfo,
		sizeof(streamInfo));

	//construct the expected bitmap header
	BITMAPINFOHEADER bih;
	bih.biBitCount = 24;
	bih.biCompression = 0; //BI_RGB;
	bih.biHeight = streamInfo.rcFrame.bottom;
	bih.biWidth = streamInfo.rcFrame.right;
	bih.biPlanes = 1;
	bih.biSize = sizeof(bih);

	sz.cx = streamInfo.rcFrame.right;
	sz.cy = streamInfo.rcFrame.bottom;

	/*
	//prepare to decompress DIBs (device independend bitmaps)
	int getFrameObject = AVIStreamGetFrameOpen(aviStream, bih);

	if (getFrameObject != 0)
	{
		AVIStreamGetFrameClose(getFrameObject);
		getFrameObject = 0;
	}
	*/
	if (aviStream != NULL)
	{
		AVIStreamRelease(aviStream);
		aviStream = NULL;
	}
	if (aviFile != NULL)
	{
		AVIFileRelease(aviFile);
		aviFile = NULL;
	}
	AVIFileExit();

	return sz;
}
