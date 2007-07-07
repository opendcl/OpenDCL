// ArxProjectCollection.h : header file
//

#pragma once

#include "ProjectCollection.h"


class CArxProjectCollection : public CProjectCollection
{
public:
	CArxProjectCollection();
	virtual ~CArxProjectCollection();

public:		
// Attributes
	CStringList	m_FileNameList;
};
