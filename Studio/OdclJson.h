#pragma once

// Studio-only .odcl.json. nlohmann must not enter Common / Runtime TUs.
// Format 1: named properties as sparse overlays on AddDefaultProperties.

class CProject;

bool StudioPathIsJson( const CString& path );
bool StudioProjectFromJsonFile( const CString& jsonPath, CProject& project, CString& error );
bool StudioProjectToJsonFile( const CProject& project, const CString& jsonPath, CString& error );

bool StudioTryGetConvertJob( CString& inPath, CString& outPath );
bool StudioConvertProject( const CString& inPath, CString& outPath, CString& error );
void StudioWriteConsoleMessage( const CString& msg, bool err );
