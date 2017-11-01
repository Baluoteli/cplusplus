#ifndef __DEF_H__
#define __DEF_H__

#include <iostream>

#ifndef EXPORTS_UTILC
#define EXPORTS_UTILC __declspec(dllexport)
#else
#define EXPORTS_UTILC __declspec(dllimport)
#endif

#endif
