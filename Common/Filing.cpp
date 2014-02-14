#include "stdafx.h"
#include "Filing.h"
#include <fstream>


void SerializeCLSID(CArchive& ar, CLSID &clsid)
{
	if (ar.IsStoring())
	{		
		ar << (DWORD) clsid.Data1;
		ar << (WORD) clsid.Data2;
		ar << (WORD) clsid.Data3;
		for (int i = 0; i < 8; i++)
		{
			ar << (BYTE) clsid.Data4[i];
		}		
	}
	else
	{
		DWORD dw; WORD w; BYTE b;
		ar >> dw; clsid.Data1 = dw;
		ar >> w; clsid.Data2 = w;
		ar >> w; clsid.Data3 = w;
		for (int i = 0; i < 8; i++)
		{
			ar >> b; clsid.Data4[i] = b;
		}				
	}
}



//text based file IO
#include "Base64.h"


LPCSTR labelDebugInt = ":INT:";
LPCSTR labelDebugLong = ":LONG:";
LPCSTR labelDebugDISPID = ":DISPID:";
LPCSTR labelDebugULONG = ":ULONG:";
LPCSTR labelDebugShort = ":SHORT:";
LPCSTR labelDebugVARTYPE = ":VARTYPE:";
LPCSTR labelDebugWORD = ":WORD:";
LPCSTR labelDebugDWORD = ":DWORD:";
LPCSTR labelDebugBYTE = ":BYTE:";
LPCSTR labelDebugDouble = ":DOUBLE:";
LPCSTR labelDebugBool = ":BOOL:";
LPCSTR labelDebugString = ":STRING:";
LPCSTR labelDebugBits = ":BITS:";
LPCSTR labelDebugCLSID = ":CLSID:";
LPCSTR labelDebugOleVariant = ":OLEVARIANT:";

LPCSTR labelReleaseBool = "B";
LPCSTR labelReleaseCLSID = "C";
LPCSTR labelReleaseDouble = "D";
LPCSTR labelReleaseShort = "H";
LPCSTR labelReleaseInt = "I";
LPCSTR labelReleaseLong = "L";
LPCSTR labelReleaseOleVariant = "O";
LPCSTR labelReleaseDWORD = "R";
LPCSTR labelReleaseString = "S";
LPCSTR labelReleaseBits = "T";
LPCSTR labelReleaseULONG = "U";
LPCSTR labelReleaseVARTYPE = "V";
LPCSTR labelReleaseWORD = "W";
LPCSTR labelReleaseDISPID = "X";
LPCSTR labelReleaseBYTE = "Y";

/*#ifdef _DEBUG
LPCSTR labelInt = labelDebugInt;
LPCSTR labelLong = labelDebugLong;
LPCSTR labelDISPID = labelDebugDISPID;
LPCSTR labelULONG = labelDebugULONG;
LPCSTR labelShort = labelDebugShort;
LPCSTR labelVARTYPE = labelDebugVARTYPE;
LPCSTR labelWORD = labelDebugWORD;
LPCSTR labelDWORD = labelDebugDWORD;
LPCSTR labelBYTE = labelDebugBYTE;
LPCSTR labelDouble = labelDebugDouble;
LPCSTR labelBool = labelDebugBool;
LPCSTR labelString = labelDebugString;
LPCSTR labelBits = labelDebugBits;
LPCSTR labelCLSID = labelDebugCLSID;
LPCSTR labelOleVariant = labelDebugOleVariant;
#else*/
LPCSTR labelBool = labelReleaseBool;
LPCSTR labelCLSID = labelReleaseCLSID;
LPCSTR labelDouble = labelReleaseDouble;
LPCSTR labelShort = labelReleaseShort;
LPCSTR labelInt = labelReleaseInt;
LPCSTR labelLong = labelReleaseLong;
LPCSTR labelOleVariant = labelReleaseOleVariant;
LPCSTR labelDWORD = labelReleaseDWORD;
LPCSTR labelString = labelReleaseString;
LPCSTR labelBits = labelReleaseBits;
LPCSTR labelULONG = labelReleaseULONG;
LPCSTR labelVARTYPE = labelReleaseVARTYPE;
LPCSTR labelWORD = labelReleaseWORD;
LPCSTR labelDISPID = labelReleaseDISPID;
LPCSTR labelBYTE = labelReleaseBYTE;
//#endif

//void writeInt(FILE* pFile, int i)
//{
//  _ftprintf(pFile, "\n%s%d", labelInt, i);
//}
//
//void writeLong(FILE* pFile, long i)
//{
//  _ftprintf(pFile, "\n%s%d", labelLong, i);
//}
//
//void writeDISPID(FILE* pFile, DISPID i)
//{
//  _ftprintf(pFile, "\n%s%d", labelDISPID, i);
//}
//
//void writeULONG(FILE* pFile, ULONG i)
//{
//  _ftprintf(pFile, "\n%s%d", labelULONG, i);
//}
//
//void writeShort(FILE* pFile, short i)
//{
//  _ftprintf(pFile, "\n%s%d", labelShort, i);
//}
//
//void writeVARTYPE(FILE* pFile, VARTYPE i)
//{
//  _ftprintf(pFile, "\n%s%d", labelVARTYPE, i);
//}
//
//void writeWORD(FILE* pFile, WORD i)
//{
//  _ftprintf(pFile, "\n%s%d", labelWORD, i);
//}
//
//void writeDWORD(FILE* pFile, DWORD i)
//{
//  _ftprintf(pFile, "\n%s%d", labelDWORD, i);
//}
//
//void writeBYTE(FILE* pFile, BYTE i)
//{
//  _ftprintf(pFile, "\n%s%d", labelBYTE, i);
//}
//
//void writeDouble(FILE* pFile, double d) {
//  _ftprintf(pFile, "\n%s%f", labelDouble, d);
//}
//
//void writeBool(FILE* pFile, bool b)
//{
//	_ftprintf(pFile, "\n%s%s"), labelBool, (b? _T("T") : _T("F"));
//}
//
//void writeBOOL(FILE* pFile, BOOL b)
//{
//  _ftprintf(pFile, "\n%s%s"), labelBool, (b? _T("T") : _T("F"));
//}
//
//void writeString(FILE* pFile, CString s)
//{
//  s.Replace("\n"), _T("\\n");
//  _ftprintf(pFile, "\n%s%s", labelString, (LPCSTR)s);
//}
//
//void writeBits(FILE* pFile, const BYTE* pBytes, DWORD byteCount)
//{
//  _ftprintf(pFile, "\n%s%s", labelBits, base64_encode(pBytes, byteCount, -1).c_str());
//}
//
//void writeCLSID(FILE* pFile, const CLSID &clsid)
//{
//  writeDWORD(pFile, clsid.Data1);
//  writeWORD(pFile, clsid.Data2);
//  writeWORD(pFile, clsid.Data3);
//  writeBits(pFile, clsid.Data4, 8);
//}
//
//void writeOleVariant(FILE* pFile, COleVariant var)
//{
//  _ftprintf(pFile, "\n%s%d", labelOleVariant, var.vt);
//  switch(var.vt) {
//    case VT_EMPTY : 
//      break;
//    case VT_I1 : writeInt(pFile, var.intVal); break;
//    case VT_I2 : writeInt(pFile, var.intVal); break;
//    case VT_I4 : writeInt(pFile, var.intVal); break;
//    default :
//      MessageBox(NULL, "Error: Writing OLE Variant!"), _T("OpenDCL Error", MB_OK);
//      _ftprintf(pFile, ""); break;
//  }
//}

/*
void GetImageFromList(const CImageList *lstImages, int nImage, CBitmap* destBitmap) {    
	//From http://www.codeproject.com/bitmap/getimagefromlist.asp

	//First we want to create a temporary image list we can manipulate
	CImageList tmpList;
	tmpList.Create(const_cast<CImageList*>(lstImages));

	//Then swap the requested image to the first spot in the list 
	tmpList.Copy( 0, nImage, ILCF_SWAP );

	//Now we need to get some information about the image 
	IMAGEINFO lastImage;
	tmpList.GetImageInfo(0,&lastImage);

	//Heres where it gets fun
	//Create a Compatible Device Context using 
	//the valid DC of your calling window
	CDC dcMem; 
	dcMem.CreateCompatibleDC(AfxGetMainWnd()->GetWindowDC()); 

	//This rect stores the size of the image we need
	CRect rect (lastImage.rcImage);

	//Using the bitmap passed in, create a bitmap compatible with the window DC
	//We also know that the bitmap needs to be a certain size.
	destBitmap->CreateCompatibleBitmap (
		AfxGetMainWnd()->GetWindowDC(), 
		rect.Width (), 
		rect.Height ());

	//Select the new destination bitmap into the DC we created above
	CBitmap* pBmpOld = dcMem.SelectObject (destBitmap);

	//This call "draws" the bitmap from the list to the new destination bitmap
	tmpList.DrawIndirect (&dcMem, 0, CPoint (0, 0), 
		CSize (rect.Width (), rect.Height ()), CPoint (0, 0));

	//cleanup by reselecting the old bitmap object into the DC
	dcMem.SelectObject (pBmpOld);
}
*/

//int iCurrentImage = 1; //yikes! [ORW]

//void writeImage(FILE* pFile, const CString &fileName, const CImage &img)
//{
//  //Get the file name without the extension or dot
//  CString imageNameBase = fileName.Left(fileName.GetLength() - 4);
//
//  /*See if that file plus "-1.png" exists. If it does not, we are saving the first
//  image to go with the file. If it does, we have already saved images and the
//  iCurrentImage value is correct.*/
//  WIN32_FIND_DATA findData;
//  HANDLE h = FindFirstFile(imageNameBase + "-1.png", &findData);
//  if (h == INVALID_HANDLE_VALUE) {
//    iCurrentImage = 1;    
//  }
//  FindClose(h);
//
//  /*Remember the number of this image so that the file can be easily found
//  when reading the image.*/
//  writeInt(pFile, iCurrentImage);
//
//  //Save the image to disk and increment for the next file we save
//	CString imageName;
//	imageName.Format("%s-%d.png", imageNameBase, iCurrentImage);
//  img.Save(imageName);
//  iCurrentImage++;
//}
//
//void writeImageList(FILE* pFile, const CString &fileName, const CImageList &l)
//{
//  writeInt(pFile, l.GetImageCount());
//  for (int i = 0; i < l.GetImageCount(); i++) {
//    CBitmap bmp;
//    GetImageFromList(&l, i, &bmp);
//    CImage img;
//    img.Attach(bmp);
//    writeImage(pFile, fileName, img);
//  }
//}

/////////////////////////////////////////////////////////////////////////////
//Functions to write data to our file.
#define CHUNKSIZE 1048576
CHAR currentChunk[CHUNKSIZE];
CStringA currentString;
int chunkPosition = CHUNKSIZE;
CStringA lastLine;
CStringA lineType;
CStringA lineValue;

void consumeLine() {
	lastLine = "";
	lineType = "";
	lineValue = "";
}

void InitFilerGlobals(void)
{
	//Init the globals before reading anything from the file.
	currentString = "";
	consumeLine();
}

bool parseLine(CStringA in, CStringA& outType, CStringA& outValue)
{
	if (in.Left(1) == ":") {
		in = in.Mid(1);
		int iColon = in.Find(':');
		if (iColon != -1) {
			outType = ":";
			outType += in.Left(iColon);
			outType += ":";
			if (in.GetLength() > iColon) {
				outValue = in.Mid(iColon + 1);
			} else {
				outValue = "";
			}
			/*outValue = in.MakeReverse();
			iColon = in.ReverseFind(':');
			if (iColon != -1) {
				outValue = in.Left(iColon);
				outValue = outValue.MakeReverse();
				return true;
			}*/
			return true;
		}
		return false;
	} else {
		outType = in.Left(1);
		outValue = in.Mid(1);
		return true;
	}
}

CStringA readLine(std::ifstream &sFile)
{
	CStringA line;

	bool readNextLine = true;
	while (readNextLine) {
		if (currentString.IsEmpty()) {
			if (sFile.eof()) {
				break;
			}
			sFile.get(currentChunk, CHUNKSIZE, '\0');
			chunkPosition = 0;
			currentString = CStringA(currentChunk);
		} else {
			chunkPosition++;
		}

		int startingPosition = chunkPosition;
		int nextNewLine = currentString.Find('\n', startingPosition);
		if (nextNewLine == -1) {
			line = line + currentString.Mid(startingPosition);
			currentString = "";
			readNextLine = true;
		} else {
			chunkPosition = nextNewLine;
			line = line + currentString.Mid(startingPosition, nextNewLine - startingPosition);
			readNextLine = false;
		}
		/*while (chunkPosition < (CHUNKSIZE - 1)
					 && currentChunk[chunkPosition] != '\n') {
				chunkPosition++;
		}

		line = line 
					 + currentString.Mid(startingPosition, chunkPosition - startingPosition);

		if (chunkPosition >= (CHUNKSIZE - 1)) {
			readNextLine = true;
		} else {
			readNextLine = false;
		}*/
	}

	line = line.TrimLeft(" ");
	return line;
}

bool readLine(std::ifstream &sFile, CStringA checkType1, CStringA checkType2)
{
	if (lastLine == "") {
		lastLine = readLine(sFile);
		if (parseLine(lastLine, lineType, lineValue)) {
			return (checkType1 == lineType || checkType2 == lineType);
		} else {
			return false;
		}
	} else {
		return (checkType1 == lineType || checkType2 == lineType);
	}
}

bool readGenericInt(std::ifstream &sFile, CStringA type1, CStringA type2, long& i)
{
	if (readLine(sFile, type1, type2)) {
		i = atol(lineValue);
		consumeLine();
		return true;
	} else {
		return false;
	}
}

bool readInt(std::ifstream &sFile, int& i)
{
	return readGenericInt(sFile, labelDebugInt, labelReleaseInt, (long&)i);
}

bool readLong(std::ifstream &sFile, long& i)
{
	return readGenericInt(sFile, labelDebugLong, labelReleaseLong, i);
}

bool readDISPIDAsLong(std::ifstream &sFile, DISPID& i)
{
	LONG l;
	if (readLong(sFile, l)) {
		i = l;
		return true;
	} else {
		return false;
	}
}

bool readDISPID(std::ifstream &sFile, DISPID& i)
{
	bool rVal = readGenericInt(sFile, labelDebugDISPID, labelReleaseDISPID, (long&)i);
	return rVal;
}

bool readULONG(std::ifstream &sFile, ULONG& i)
{
	return readGenericInt(sFile, labelDebugULONG, labelReleaseULONG, (long&)i);
}

bool readShort(std::ifstream &sFile, short& i)
{
	long l;
	if( !readGenericInt(sFile, labelDebugShort, labelReleaseShort, l) )
		return false;
	i = (short)l;
	return true;
}

bool readVARTYPE(std::ifstream &sFile, VARTYPE& i)
{
	return readGenericInt(sFile, labelDebugVARTYPE, labelReleaseVARTYPE, (long&)i);
}

bool readWORD(std::ifstream &sFile, WORD& i)
{
	return readGenericInt(sFile, labelDebugWORD, labelReleaseWORD, (long&)i);
}

bool readDWORD(std::ifstream &sFile, DWORD& i)
{
	return readGenericInt(sFile, labelDebugDWORD, labelReleaseDWORD, (long&)i);
}

bool readBYTE(std::ifstream &sFile, BYTE& b)
{
	long i;
	if (readGenericInt(sFile, labelDebugBYTE, labelReleaseBYTE, i)) {
		b = (BYTE)i;
		return true;
	} else {
		return false;
	}
}

bool readDouble(std::ifstream &sFile, double& d)
{
	if (readLine(sFile, labelDebugDouble, labelReleaseDouble)) {
		d = atof(lineValue);
		consumeLine();
		return true;
	} else {
		return false;
	}
}

bool readBool(std::ifstream &sFile, bool& b)
{
	if (readLine(sFile, labelDebugBool, labelReleaseBool)) {
		b = (lineValue == "True") || lineValue == _T("T");
		consumeLine();
		return true;
	} else {
		return false;
	}
}

bool readBOOL(std::ifstream &sFile, BOOL& b)
{
	if (readLine(sFile, labelDebugBool, labelReleaseBool)) {
		b = (lineValue == "True") || lineValue == _T("T");
		consumeLine();
		return true;
	} else {
		return false;
	}
}

bool readString(std::ifstream &sFile, CStringA& s)
{
	if (readLine(sFile, labelDebugString, labelReleaseString)) {
		s = lineValue;
		s.Replace("\\n", "\n");
		consumeLine();
		return true;
	} else {
		return false;
	}
}

bool readBits(std::ifstream &sFile, std::string& bits)
{
	if (readLine(sFile, labelDebugBits, labelReleaseBits)) {
		bits = base64_decode(std::string(CStringA(lineValue)));
		consumeLine();
		return true;
	}
	return false;
}

bool readCLSID(std::ifstream &sFile, CLSID& clsid)
{
	bool rVal =
		readDWORD(sFile, clsid.Data1) 
		&& readWORD(sFile, clsid.Data2)
		&& readWORD(sFile, clsid.Data3);
	if (rVal) {
		std::string bits;
		if (!readBits(sFile, bits) || bits.size() < 8) {
			return false;
		}
		for (int i = 0; i < 8; i++) {
			clsid.Data4[i] = bits[i];
		}
	}
	return rVal;
}

bool readOleVariant(std::ifstream &sFile, COleVariant& var)
{
	long i;
	if (!readGenericInt(sFile, labelDebugOleVariant, labelReleaseOleVariant, i)) return false;
	var.vt = (VARTYPE)i;

	switch (var.vt) {
		case VT_EMPTY : 
			return true;
		case VT_I1 : 
			return readInt(sFile, var.intVal);
		case VT_I2 : 
			return readInt(sFile, var.intVal);
		case VT_I4 : 
			return readInt(sFile, var.intVal);
		default :
			MessageBoxA(NULL, "Error: Reading OLE Variant!", "OpenDCL Error", MB_OK);
			return false;
	}
	return false;
}

//Saving these using CArchives, not my text stream.
//bool readImage(std::ifstream &sFile, const CString &fileName, CImage& img)
//{
//  int iImageNumber;
//  if (!readInt(sFile, iImageNumber)) return false;
//
//  //Get the file name without the extension or dot
//	CStringA imageName;
//	imageName.Format("%s-%d.png", fileName.Left(fileName.GetLength() - 4), iImageNumber);
//  img.Load(imageName);
//  return true;
//}

bool readImageList(std::ifstream &sFile, const CString &fileName, CImageList &l)
{
	int iCount;
	if (!readInt(sFile, iCount)) return false;

	//bool bFirst = true;
	//for (int i = 0; i < iCount; i++) {
	//  CImage img;
	//  if (readImage(sFile, fileName, img)) {
	//    if (bFirst) {
	//      l.Create(img.GetWidth(), img.GetHeight(), ILC_COLOR4 | ILC_MASK, 1, 1);
	//      bFirst = false;
	//    }
	//    HBITMAP hBitmap = img.Detach();
	//    l.Add(CBitmap::FromHandle(hBitmap), RGB(0,0,0));
	//  } else {
	//    return false;
	//  }
	//}

	return true;
}
