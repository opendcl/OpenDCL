// UpdateCheck.h : header file
//

#pragma once

bool UpdateCheck( LPCTSTR pszProductName, LPCTSTR pszInstalledVersion /*N.N.N.N format*/ ); //returns false on initialization error
void UpdateCheckShutdown(); // join worker threads; call before unload