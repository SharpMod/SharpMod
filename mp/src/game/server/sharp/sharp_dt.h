#pragma once
#include "cbase.h"
#include "sharp/sharp.h"
#include "dt_common.h"
#include <string>
#include <vector>

SendProp SendPropSharp();

class SharpDTString {
public:
	char buf[DT_MAX_STRING_BUFFERSIZE];
};

class EntitySharpDT {
public:
	int intArraySize;
	int stringArraySize;
	int vectorArraySize;

	CUtlVector<SharpDTString> m_bufferedStr;
};

