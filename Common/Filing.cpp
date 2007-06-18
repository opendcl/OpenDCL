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


LPCTSTR labelDebugInt = _T(":INT:");
LPCTSTR labelDebugLong = _T(":LONG:");
LPCTSTR labelDebugDISPID = _T(":DISPID:");
LPCTSTR labelDebugULONG = _T(":ULONG:");
LPCTSTR labelDebugShort = _T(":SHORT:");
LPCTSTR labelDebugVARTYPE = _T(":VARTYPE:");
LPCTSTR labelDebugWORD = _T(":WORD:");
LPCTSTR labelDebugDWORD = _T(":DWORD:");
LPCTSTR labelDebugBYTE = _T(":BYTE:");
LPCTSTR labelDebugDouble = _T(":DOUBLE:");
LPCTSTR labelDebugBool = _T(":BOOL:");
LPCTSTR labelDebugString = _T(":STRING:");
LPCTSTR labelDebugBits = _T(":BITS:");
LPCTSTR labelDebugCLSID = _T(":CLSID:");
LPCTSTR labelDebugOleVariant = _T(":OLEVARIANT:");

LPCTSTR labelReleaseBool = _T("B");
LPCTSTR labelReleaseCLSID = _T("C");
LPCTSTR labelReleaseDouble = _T("D");
LPCTSTR labelReleaseShort = _T("H");
LPCTSTR labelReleaseInt = _T("I");
LPCTSTR labelReleaseLong = _T("L");
LPCTSTR labelReleaseOleVariant = _T("O");
LPCTSTR labelReleaseDWORD = _T("R");
LPCTSTR labelReleaseString = _T("S");
LPCTSTR labelReleaseBits = _T("T");
LPCTSTR labelReleaseULONG = _T("U");
LPCTSTR labelReleaseVARTYPE = _T("V");
LPCTSTR labelReleaseWORD = _T("W");
LPCTSTR labelReleaseDISPID = _T("X");
LPCTSTR labelReleaseBYTE = _T("Y");

/*#ifdef _DEBUG
LPCTSTR labelInt = labelDebugInt;
LPCTSTR labelLong = labelDebugLong;
LPCTSTR labelDISPID = labelDebugDISPID;
LPCTSTR labelULONG = labelDebugULONG;
LPCTSTR labelShort = labelDebugShort;
LPCTSTR labelVARTYPE = labelDebugVARTYPE;
LPCTSTR labelWORD = labelDebugWORD;
LPCTSTR labelDWORD = labelDebugDWORD;
LPCTSTR labelBYTE = labelDebugBYTE;
LPCTSTR labelDouble = labelDebugDouble;
LPCTSTR labelBool = labelDebugBool;
LPCTSTR labelString = labelDebugString;
LPCTSTR labelBits = labelDebugBits;
LPCTSTR labelCLSID = labelDebugCLSID;
LPCTSTR labelOleVariant = labelDebugOleVariant;
#else*/
LPCTSTR labelBool = labelReleaseBool;
LPCTSTR labelCLSID = labelReleaseCLSID;
LPCTSTR labelDouble = labelReleaseDouble;
LPCTSTR labelShort = labelReleaseShort;
LPCTSTR labelInt = labelReleaseInt;
LPCTSTR labelLong = labelReleaseLong;
LPCTSTR labelOleVariant = labelReleaseOleVariant;
LPCTSTR labelDWORD = labelReleaseDWORD;
LPCTSTR labelString = labelReleaseString;
LPCTSTR labelBits = labelReleaseBits;
LPCTSTR labelULONG = labelReleaseULONG;
LPCTSTR labelVARTYPE = labelReleaseVARTYPE;
LPCTSTR labelWORD = labelReleaseWORD;
LPCTSTR labelDISPID = labelReleaseDISPID;
LPCTSTR labelBYTE = labelReleaseBYTE;
//#endif

//void writeInt(FILE* pFile, int i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelInt, i);
//}
//
//void writeLong(FILE* pFile, long i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelLong, i);
//}
//
//void writeDISPID(FILE* pFile, DISPID i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelDISPID, i);
//}
//
//void writeULONG(FILE* pFile, ULONG i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelULONG, i);
//}
//
//void writeShort(FILE* pFile, short i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelShort, i);
//}
//
//void writeVARTYPE(FILE* pFile, VARTYPE i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelVARTYPE, i);
//}
//
//void writeWORD(FILE* pFile, WORD i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelWORD, i);
//}
//
//void writeDWORD(FILE* pFile, DWORD i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelDWORD, i);
//}
//
//void writeBYTE(FILE* pFile, BYTE i)
//{
//  _ftprintf(pFile, _T("\n%s%d"), labelBYTE, i);
//}
//
//void writeDouble(FILE* pFile, double d) {
//  _ftprintf(pFile, _T("\n%s%f"), labelDouble, d);
//}
//
//void writeBool(FILE* pFile, bool b)
//{
//	_ftprintf(pFile, _T("\n%s%s"), labelBool, (b? _T("T") : _T("F")));
//}
//
//void writeBOOL(FILE* pFile, BOOL b)
//{
//  _ftprintf(pFile, _T("\n%s%s"), labelBool, (b? _T("T") : _T("F")));
//}
//
//void writeString(FILE* pFile, CString s)
//{
//  s.Replace(_T("\n"), _T("\\n"));
//  _ftprintf(pFile, _T("\n%s%s"), labelString, (LPCTSTR)s);
//}
//
//void writeBits(FILE* pFile, const BYTE* pBytes, DWORD byteCount)
//{
//  _ftprintf(pFile, _T("\n%s%s"), labelBits, base64_encode(pBytes, byteCount, -1).c_str());
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
//  _ftprintf(pFile, _T("\n%s%d"), labelOleVariant, var.vt);
//  switch(var.vt) {
//    case VT_EMPTY : 
//      break;
//    case VT_I1 : writeInt(pFile, var.intVal); break;
//    case VT_I2 : writeInt(pFile, var.intVal); break;
//    case VT_I4 : writeInt(pFile, var.intVal); break;
//    default :
//      MessageBox(NULL, _T("Error: Writing OLE Variant!"), _T("OpenDCL Error"), MB_OK);
//      _ftprintf(pFile, _T("")); break;
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
//  HANDLE h = FindFirstFile(imageNameBase + _T("-1.png"), &findData);
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
//	imageName.Format(_T("%s-%d.png"), imageNameBase, iCurrentImage);
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
LPSTR currentChunk = new CHAR[CHUNKSIZE];
CString currentString;
int chunkPosition = CHUNKSIZE;
CString lastLine;
CString lineType;
CString lineValue;

void consumeLine() {
  lastLine = _T("");
  lineType = _T("");
  lineValue = _T("");
}

void InitFilerGlobals(void)
{
  //Init the globals before reading anything from the file.
  currentString = _T("");
  consumeLine();
}

bool parseLine(CString in, CString& outType, CString& outValue)
{
  if (in.Left(1) == _T(":")) {
    in = in.Mid(1);
    int iColon = in.Find(_T(':'));
    if (iColon != -1) {
      outType = _T(":");
			outType += (in.Left(iColon) + _T(":"));
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

CString readLine(std::ifstream &sFile)
{
  CString line;

  bool readNextLine = true;
  while (readNextLine) {
    if (currentString.IsEmpty()) {
      if (sFile.eof()) {
        break;
      }
      sFile.get(currentChunk, CHUNKSIZE, '\0');
      chunkPosition = 0;
      currentString = CString(currentChunk);
    } else {
      chunkPosition++;
    }

    int startingPosition = chunkPosition;
    int nextNewLine = currentString.Find(_T('\n'), startingPosition);
    if (nextNewLine == -1) {
      line = line + currentString.Mid(startingPosition);
      currentString = _T("");
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

  line = line.TrimLeft(_T(" "));
  return line;
}

bool readLine(std::ifstream &sFile, CString checkType1, CString checkType2)
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

bool readGenericInt(std::ifstream &sFile, CString type1, CString type2, long& i)
{
  if (readLine(sFile, type1, type2)) {
    i = _ttol(lineValue);
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
	if (!rVal) {
		int p = 0;
	}
  return rVal;
}

bool readULONG(std::ifstream &sFile, ULONG& i)
{
  return readGenericInt(sFile, labelDebugULONG, labelReleaseULONG, (long&)i);
}

bool readShort(std::ifstream &sFile, short& i)
{
  return readGenericInt(sFile, labelDebugShort, labelReleaseShort, (long&)i);
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
    d = _tstof(lineValue);
    consumeLine();
    return true;
  } else {
    return false;
  }
}

bool readBool(std::ifstream &sFile, bool& b)
{
  if (readLine(sFile, labelDebugBool, labelReleaseBool)) {
    b = (lineValue == _T("True") || lineValue == _T("T"));
    consumeLine();
    return true;
  } else {
    return false;
  }
}

bool readBOOL(std::ifstream &sFile, BOOL& b)
{
  if (readLine(sFile, labelDebugBool, labelReleaseBool)) {
    b = (lineValue == _T("True") || lineValue == _T("T"));
    consumeLine();
    return true;
  } else {
    return false;
  }
}

bool readString(std::ifstream &sFile, CString& s)
{
  if (readLine(sFile, labelDebugString, labelReleaseString)) {
    s = lineValue;
    s.Replace(_T("\\n"), _T("\n"));
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
      MessageBox(NULL, _T("Error: Reading OLE Variant!"), _T("OpenDCL Error"), MB_OK);
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
//	CString imageName;
//	imageName.Format(_T("%s-%d.png"), fileName.Left(fileName.GetLength() - 4), iImageNumber);
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
