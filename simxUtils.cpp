#include "simxUtils.h"

short littleEndianShortConversion(short v,bool otherSideIsBigEndian)
{
    short retV=v;
    if (otherSideIsBigEndian)
    {
        ((BYTE*)&retV)[0]=((BYTE*)&v)[1];
        ((BYTE*)&retV)[1]=((BYTE*)&v)[0];
    }
    return(retV);
}

WORD littleEndianWordConversion(WORD v,bool otherSideIsBigEndian)
{
    WORD retV=v;
    if (otherSideIsBigEndian)
    {
        ((BYTE*)&retV)[0]=((BYTE*)&v)[1];
        ((BYTE*)&retV)[1]=((BYTE*)&v)[0];
    }
    return(retV);
}

int littleEndianIntConversion(int v,bool otherSideIsBigEndian)
{
    int retV=v;
    if (otherSideIsBigEndian)
    {
        ((BYTE*)&retV)[0]=((BYTE*)&v)[3];
        ((BYTE*)&retV)[1]=((BYTE*)&v)[2];
        ((BYTE*)&retV)[2]=((BYTE*)&v)[1];
        ((BYTE*)&retV)[3]=((BYTE*)&v)[0];
    }
    return(retV);
}

float littleEndianFloatConversion(float v,bool otherSideIsBigEndian)
{
    float retV=v;
    if (otherSideIsBigEndian)
    {
        ((BYTE*)&retV)[0]=((BYTE*)&v)[3];
        ((BYTE*)&retV)[1]=((BYTE*)&v)[2];
        ((BYTE*)&retV)[2]=((BYTE*)&v)[1];
        ((BYTE*)&retV)[3]=((BYTE*)&v)[0];
    }
    return(retV);
}

double littleEndianDoubleConversion(double v,bool otherSideIsBigEndian)
{
    double retV=v;
    if (otherSideIsBigEndian)
    {
        ((BYTE*)&retV)[0]=((BYTE*)&v)[7];
        ((BYTE*)&retV)[1]=((BYTE*)&v)[6];
        ((BYTE*)&retV)[2]=((BYTE*)&v)[5];
        ((BYTE*)&retV)[3]=((BYTE*)&v)[4];
        ((BYTE*)&retV)[4]=((BYTE*)&v)[3];
        ((BYTE*)&retV)[5]=((BYTE*)&v)[2];
        ((BYTE*)&retV)[6]=((BYTE*)&v)[1];
        ((BYTE*)&retV)[7]=((BYTE*)&v)[0];
    }
    return(retV);
}

WORD getCRC(const char* data,int length)
{
    WORD crc=0;
    int p=0;
    for (int i=0;i<length;i++)
    {
        crc=crc^(((WORD)data[p])<<8);
        for (int j=0;j<8;j++)
        {
            if (crc&WORD(0x8000))
                crc=(crc<<1)^WORD(0x1021);
            else
                crc<<=1;
        }
        p++;
    }
    return(crc);
}
