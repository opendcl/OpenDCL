#pragma once

#include <AtlImage.h> //for CImage


void SerializeCLSID(CArchive& ar, CLSID &clsid);


void InitFilerGlobals(void); //this needs to be removed ASAP [ORW]

void writeInt(FILE* pFile, int i);
void writeLong(FILE* pFile, long i);
void writeDISPID(FILE* pFile, DISPID i);
void writeULONG(FILE* pFile, ULONG i);
void writeShort(FILE* pFile, short i);
void writeVARTYPE(FILE* pFile, VARTYPE i);
void writeWORD(FILE* pFile, WORD i);
void writeDWORD(FILE* pFile, DWORD i);
void writeBYTE(FILE* pFile, BYTE i);
void writeDouble(FILE* pFile, double d);
void writeBool(FILE* pFile, bool b);
void writeBOOL(FILE* pFile, BOOL b);
void writeString(FILE* pFile, CString s);
void writeBits(FILE* pFile, const BYTE* pBytes, DWORD byteCount);
void writeCLSID(FILE* pFile, const CLSID &clsid);
void writeOleVariant(FILE* pFile, COleVariant var);
void writeImage(FILE* pFile, const CString &fileName, const CImage &img);
void writeImageList(FILE* pFile, const CString &fileName, const CImageList &l);
CString readLine(std::ifstream &sFile);
bool readLine(std::ifstream &sFile, CString checkType1, CString checkType2);
bool readInt(std::ifstream &sFile, int& i);
bool readLong(std::ifstream &sFile, long& i);
bool readDISPIDAsLong(std::ifstream &sFile, DISPID& i);
bool readDISPID(std::ifstream &sFile, DISPID& i);
bool readULONG(std::ifstream &sFile, ULONG& i);
bool readShort(std::ifstream &sFile, short& i);
bool readVARTYPE(std::ifstream &sFile, VARTYPE& i);
bool readWORD(std::ifstream &sFile, WORD& i);
bool readDWORD(std::ifstream &sFile, DWORD& i);
bool readBYTE(std::ifstream &sFile, BYTE& b);
bool readDouble(std::ifstream &sFile, double& d);
bool readBool(std::ifstream &sFile, bool& b);
bool readBOOL(std::ifstream &sFile, BOOL& b);
bool readString(std::ifstream &sFile, CString& s);
bool readBits(std::ifstream &sFile, std::string& bits);
bool readCLSID(std::ifstream &sFile, CLSID& clsid);
bool readOleVariant(std::ifstream &sFile, COleVariant& var);
bool readImage(std::ifstream &sFile, const CString &fileName, CImage& img);
bool readImageList(std::ifstream &sFile, const CString &fileName, CImageList &l);
