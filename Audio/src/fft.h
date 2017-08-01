#ifndef __FFT_ALGORITHM_H_
#define __FFT_ALGORITHM_H_
//
#include <math.h>
#include <vector>
//
extern
FLOAT FastSqrt(FLOAT x);
//
enum WorkChannel
{
    WRKCHANNEL_LEFT   = 0,
    WRKCHANNEL_RIGHT  = 1,
    WRKCHANNEL_MONO   = 2,
    WRKCHANNEL_STEREO = 3
};
//
class SampleIter
{
protected:
    WorkChannel m_curChannel;
    PCHAR       m_pSamples;
    INT         m_sampleLen;
    INT         m_byteWidth;
    BOOL        m_stereoSamples;
    INT         m_interRWIdx;

public:
    SampleIter();
    BOOL SetSampleWorkSet(PCHAR pSamples, INT sampleLen, INT byteWidth, BOOL stereo);
//    
    INT Count(VOID);
    INT GetPos(VOID);
//
    VOID SetPos(INT idx);
    VOID Advance(VOID);
	VOID SetWorkChannel(WorkChannel workChhannel);
//
    SHORT GetSample(VOID);
};
//
class FFTransform
{
private:
    INT    sampleRate;
    INT    sampleCount;
    FLOAT* pSpecMag;
    FLOAT* pXReal;
    FLOAT* pXImaginary;
    FLOAT* pSamples;
//
#define sinCosRng 2048
    FLOAT  sINTab[sinCosRng];
    FLOAT  cosTab[sinCosRng]; 
//
    INT  bitrev(INT j, INT nu);
    VOID InitFftTables(VOID);
//
public:
//
    FFTransform(INT sampleRate, INT sampleCount);
    ~FFTransform();
//
    INT HzToPoINT(INT freq) const;
    BOOL fftTransformMag(SampleIter& samples, BOOL powerSpectrum);
//
    FLOAT* fftGetMagArray(VOID);
    FLOAT  fftGetMagFromHertz(INT freq);
};
//
#endif