#include "audiostealer.h"
//
SampleIter::SampleIter()
{
    m_curChannel    = WRKCHANNEL_STEREO;
    m_pSamples      = NULL;
    m_sampleLen     = 0;
    m_byteWidth     = 0;
    m_interRWIdx    = 0;
    m_stereoSamples = FALSE;
};
//
VOID
SampleIter::SetWorkChannel(WorkChannel workChhannel)
{
    if (this == NULL)
        return;
    m_curChannel = workChhannel;
};
//
BOOL
SampleIter::SetSampleWorkSet(PCHAR pSamples, INT sampleLen, INT byteWidth, BOOL stereo)
{
    if (this == NULL)
        return FALSE;
    if (pSamples == NULL)
        return FALSE;

    m_pSamples = pSamples;

    if (byteWidth <= 0)
        return FALSE;

    m_sampleLen = sampleLen;
    m_byteWidth = byteWidth;

    if ((m_stereoSamples = stereo) == FALSE)
        m_curChannel = WRKCHANNEL_MONO;
    else
        m_curChannel = WRKCHANNEL_STEREO;
    SetPos (0);
    return TRUE;
};
//
INT
SampleIter::Count(VOID)
{
    if (this == NULL)
        return 0;
    if (m_pSamples == NULL)
        return 0;

    if (m_sampleLen <= 0)
        return 0;

    INT cnt = m_sampleLen / m_byteWidth;

    if (m_stereoSamples != FALSE)
        cnt /= 2;
    return cnt;
};
//
VOID
SampleIter::SetPos(INT idx)
{
    if (this == NULL)
        return;
    __try
    {
        m_interRWIdx = (idx * m_byteWidth);
        if (m_stereoSamples != FALSE)
            m_interRWIdx *= 2;
    }
    __except(1)
    {
    };
};
//
INT
SampleIter::GetPos(VOID)
{
    if (this == NULL)
        return 0;
    INT pos = m_interRWIdx / m_byteWidth;
    if (m_stereoSamples != FALSE)
        pos /= 2;
    return pos;
};
//
VOID
SampleIter::Advance(VOID)
{
    if (this == NULL)
        return;
    m_interRWIdx += m_byteWidth;
    if (m_stereoSamples != FALSE)
        m_interRWIdx += m_byteWidth;
};
//
SHORT
SampleIter::GetSample(VOID)
{
    if (this == NULL)
        return -1;

    if (m_pSamples == NULL)
        return -1;

    switch (m_byteWidth)
    {
        case 0:
            return -1;

        case 1:
            if (m_curChannel != WRKCHANNEL_RIGHT)
            {
                return m_pSamples[m_interRWIdx];
            };
            return m_pSamples[m_interRWIdx + 1];

        case 2:
            if (m_curChannel != WRKCHANNEL_RIGHT)
            {
                return *((PSHORT)(m_pSamples + m_interRWIdx));
            } ;
            return *((PSHORT)(m_pSamples + m_interRWIdx + 2));

        default:
            return *((PSHORT)(m_pSamples + m_interRWIdx));
    };
    return -1;
};
//
FFTransform::FFTransform(INT sampleRate, INT sampleCount)
{
    if (this == NULL)
        return;

    InitFftTables();

    this->sampleRate  = sampleRate;
    this->sampleCount = sampleCount;
    pSpecMag          = new FLOAT [sampleCount];
    pXReal            = new FLOAT [sampleCount];
    pXImaginary       = new FLOAT [sampleCount];
};
//
FFTransform::~FFTransform()
{
    if (pSpecMag != NULL)
        delete[] pSpecMag;

    if (pXReal != NULL)
        delete[] pXReal;

    if (pXImaginary != NULL)
        delete[] pXImaginary;

    pXReal = pXImaginary = pSpecMag = NULL;
};
//
INT
FFTransform::bitrev(INT j, INT nu)
{
    if (this == NULL)
        return 0;

    INT j2;
    INT j1 = j;
    INT k = 0;
    for (INT i = 1; i <= nu; i++)
    {
        j2 = j1 / 2;
        k  = ((2 * k) + j1) - (2 * j2);
        j1 = j2;
    };
    return k;
};
//
VOID
FFTransform::InitFftTables(VOID)
{
    if (this == NULL)
        return;

    DOUBLE step = HALFPI / (DOUBLE)sinCosRng;
    DOUBLE rad  = 0.0;

    for (INT idx = 0; idx < sinCosRng; idx++)
    {
        sINTab[idx] = (FLOAT)sin(rad);
        cosTab[idx] = (FLOAT)cos(rad);
        rad        += step;
    };
};
//
INT
FFTransform::HzToPoINT(INT freq) const
{
    if (this == NULL)
        return 0;
    return (sampleCount * freq) / sampleRate;
};
//
FLOAT*
FFTransform::fftGetMagArray(VOID)
{
    if (this == NULL)
        return NULL;
    return pSpecMag;
};
//
FLOAT
FFTransform::fftGetMagFromHertz(INT freq)
{
    if (this == NULL)
        return 0.0;
    return pSpecMag[HzToPoINT(freq)];
};
//
BOOL
FFTransform::fftTransformMag(SampleIter& samples, BOOL powerSpectrum)
{
    if (this == NULL)
        return FALSE;
    if ((pSpecMag == NULL) || (pXImaginary == NULL) || (pXReal == NULL))
        return FALSE;
    if (samples.Count() > sampleCount)
        return FALSE;

    INT nu            = (INT)(log(sampleCount)/log(2));
    INT halfSampCount = sampleCount/2;
    INT nu1           = nu - 1;

    FLOAT tr, ti, p, arg, c, s;

    INT idx;
    for (idx = 0; idx < sampleCount; idx++)
    {
        pXReal[idx]  = samples.GetSample();

        if (powerSpectrum != FALSE)
            pSpecMag[idx] = pXReal[idx];

        pXImaginary[idx] = 0.0f;
        samples.Advance();
    };

    INT k = 0;
    INT scidx;

    for (INT l = 1; l <= nu; l++)
    {
        while (k < sampleCount)
        {
            for (INT i = 1; i <= halfSampCount; i++)
            {
                BITREV_F(k >> nu1, nu, &p)
                arg = 2 * (FLOAT)PI * p / sampleCount;

                while (arg >= HALFPI)
                    arg = arg - HALFPI;

                scidx = (INT)((arg / HALFPI) * (FLOAT)sinCosRng);
                c     = cosTab [scidx];
                s     = sINTab [scidx];

                tr  = pXReal[k + halfSampCount] * c + pXImaginary[k + halfSampCount] * s;
                ti  = pXReal[k + halfSampCount] * c - pXReal[k + halfSampCount] * s;

                pXReal[k+halfSampCount]      = pXReal[k] - tr;
                pXImaginary[k+halfSampCount] = pXImaginary[k] - ti;
                pXReal[k]                   += tr;
                pXImaginary[k]              += ti;
                k++;
            };
            k += halfSampCount;
        };

        k = 0;
        nu1--;
        halfSampCount = halfSampCount / 2;
    };

    k = 0;
    INT r;
    while (k < sampleCount)
    {
        BITREV_I(k, nu, &r)

        if (r > k)
        {
            tr             = pXReal[k];
            ti             = pXImaginary[k];
            pXReal[k]      = pXReal[r];
            pXImaginary[k] = pXImaginary[r];
            pXReal[r]      = tr;
            pXImaginary[r] = ti;
        };
        k++;
    };

    halfSampCount = sampleCount / 2;
    if (powerSpectrum != FALSE)
    {
        for (idx = 0; idx < halfSampCount; idx++)
            pSpecMag[idx] = 2.0f * ((FLOAT)pXReal[idx] / (FLOAT)sampleCount);
    }
    else
    {
        pSpecMag[0]   = (FLOAT)(FastSqrt(pXReal[0] * pXReal[0] + pXImaginary[0] * pXImaginary[0])) / (FLOAT)sampleCount;
        for (idx = 1; idx < halfSampCount; idx++)
              pSpecMag[idx] = 2.0f * (FLOAT)(FastSqrt((pXReal[idx] * pXReal[idx]) + (pXImaginary[idx] * pXImaginary[idx]))) / (FLOAT)sampleCount;
    };
    return TRUE;
};
