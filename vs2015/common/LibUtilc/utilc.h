#ifndef __UTILC_H__
#define __UTILC_H__

#ifdef exportUTILC
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

#include <stdio.h>
#include <iostream>
#include <atlstr.h>
#include <vector>
#include <map>
#pragma warning(disable:4190)
#pragma warning(disable:4273)
#pragma warning(disable:4293)
#pragma warning(disable:4251)
#pragma warning(disable:4800)

#include "../../../Include/MacDef.h"
#include "../../../Include/ConstantDef.h"


#endif
