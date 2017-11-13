#pragma once

#include "porting.h"

short littleEndianShortConversion(short v,bool otherSideIsBigEndian);
WORD littleEndianWordConversion(WORD v,bool otherSideIsBigEndian);
int littleEndianIntConversion(int v,bool otherSideIsBigEndian);
float littleEndianFloatConversion(float v,bool otherSideIsBigEndian);
double littleEndianDoubleConversion(double v,bool otherSideIsBigEndian);
WORD getCRC(const char* data,int length);
