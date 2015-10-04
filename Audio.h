// Copyright 2004/2006 Marko Mihovilic

#pragma once

// Our structure for the peaks
struct Peak
{
	float position;		// The position of the peak
	float velocity;		// The direction of movement
	float life;			// The time the peak has sleeped
};

class Audio
{
public:
	Audio(void);
	~Audio(void);

	bool	CreateAudio(void);
	bool	GetCreated(void) const { return mCreated; }
	void	DestroyAudio(void);

	void	Process(void);

	bool	StartCapture(void);
	bool	StopCapture(void);

	bool	Capture(short* buffer, unsigned long bufferSize);

	bool	SetSource(unsigned long index);
	bool SetSource(const wchar_t* name) { for(unsigned long i = 0; i < mSources.GetSize(); ++i) { if(mSources[i] == name) return SetSource(i); } return false; }
	const String& GetSource(unsigned long index) { return mSources[index]; }
	unsigned long GetSourcesCount(void) const { return mSources.GetSize(); }

	void	GetSeperateChannels(double* left, double* right,const short* buffer,unsigned long bufferSize);
	void	GetMiddleChannel(double* out,const double* left,const double* right,unsigned long size);

	void	GetLazySpectrum(float* ls,const double* ch,unsigned long size);
	void	GetPowerSpectrum(float* ps,const double* ch,unsigned long size);
	void	GetEq(float* eq,const double* ch,unsigned long size);
	void	GetPeaks(Peak* peaks,const float* eq,unsigned long size);
	void	GetScope(float* scope,const float* left,const float* right,unsigned long size);
	void	GetVu(float* vu,const double* left,const double* right,unsigned long size);

	float	GetAverage(const float* buffer,unsigned long bufferSize) const;
	float	GetTotal(const float* buffer,unsigned long bufferSize) const;
	float	GetAverageAbs(const float* buffer,unsigned long bufferSize) const;
	float	GetTotalAbs(const float* buffer,unsigned long bufferSize) const;

	float	GetMagnitude(float level,float scale,float magnitude,float cutrange,float sharpness,unsigned long x,unsigned long size);
	double	GetBlackmanHarrisWindow(unsigned long x,unsigned long size);

	//double	GetAveragePowerSpectrum(void) const { return this->GetAverage(mPowerSpectrum[0],ANALYZER_FFTBUFFERSIZE/4 - 1); }
	//double	GetTotalPowerSpectrum(void) const { return this->GetTotal(mPowerSpectrum,ANALYZER_FFTBUFFERSIZE/4 - 1); }
	//double	GetAverageEq(void) const { return this->GetAverage(mEq,ANALYZER_NUMBANDS); }
	//double	GetTotalEq(void) const { return this->GetTotal(mEq,ANALYZER_NUMBANDS); }
	//double	GetAverageScope(void) const { return this->GetAverage(mScope,ANALYZER_FFTBUFFERSIZE/2); }
	//double	GetTotalScope(void) const { return this->GetTotal(mScope,ANALYZER_FFTBUFFERSIZE/2); }
	//double	GetAverageScopeAbs(void) const { return this->GetAverageAbs(mScope,ANALYZER_FFTBUFFERSIZE/2); }
	//double	GetTotalScopeAbs(void) const { return this->GetTotalAbs(mScope,ANALYZER_FFTBUFFERSIZE/2); }

	float	GetLazySpectrum(unsigned long index) const { return (mLazySpectrum[0][index] + mLazySpectrum[1][index])/2.0f; }
	float	GetLazySpectrum(unsigned long channel,unsigned long index) { return mLazySpectrum[channel][index]; }
	float	GetPowerSpectrum(unsigned long index) const { return (mPowerSpectrum[0][index] + mPowerSpectrum[1][index])/2.0f; }
	float	GetPowerSpectrum(unsigned long channel,unsigned long index) const { return mPowerSpectrum[channel][index]; }
	float	GetEq(unsigned long index) const { return (mEq[0][index] + mEq[1][index])/2.0f; }
	float	GetEq(unsigned long channel, unsigned long index) const { return mEq[channel][index]; }
	Peak	GetPeaks(unsigned long channel, unsigned long index) const { return mPeaks[channel][index]; }
	float	GetScope(unsigned long index) const { return mScope[index]; }
	float	GetVu(unsigned char channel) const { return mVu[channel]; }
	float	GetVu(void) const { return (mVu[0] + mVu[1]) / 2.0; }

	void	SetEqFalloff(long falloff);
	void	SetPeakFalloff(long falloff);
	void	SetPeakDelay(long delay);

	long	GetEqFalloff(void) const { if(mEqFalloff == 0.2f) return -1; else if(mEqFalloff == 0.5f) return 0; else if(mEqFalloff == 0.8f) return 1; else return 0; }
	long	GetPeakFalloff(void) const { if(mPeakFalloff == 0.2f) return -1; else if(mPeakFalloff == 0.5f) return 0; else if(mPeakFalloff == 0.8f) return 1; else return 0; }
	long	GetPeakDelay(void) const { if(mPeakDelay == 2.0f) return -1; else if(mPeakDelay == 6.0f) return 0; else if(mPeakDelay == 10.0f) return 1; else return 0; }

protected:
	bool	mCreated;

	IDirectSoundCapture8*		mSoundCapture;	// Our sound capture interface
	IDirectSoundCaptureBuffer*	mCaptureBuffer;	// Our sound capture buffer

	Array<String>		mSources;

	float				mAmplification;

	float				mEqFalloff;
	float				mPeakFalloff;
	float				mPeakDelay;

	// Our fft buffers
	float	mLazySpectrum[2][ANALYZER_FFTBUFFERSIZE/4];		// Our main buffer for the lazy power spectrum
	float	mPowerSpectrum[2][ANALYZER_FFTBUFFERSIZE/4];	// Our main buffer for the power spectrum
	float	mEq[2][ANALYZER_NUMBANDS];						// Our main eq bands that we render to the screen
	float	mScope[ANALYZER_FFTBUFFERSIZE/2];				// Our scope buffer
	float	mVu[2];											// Our vu meter levels
	Peak	mPeaks[2][ANALYZER_NUMBANDS];					// Our main peak levels
};

__forceinline unsigned long GetFrequencySubspectrum(unsigned long band,unsigned long bands,unsigned long spectrum,unsigned long offset,unsigned long skip)
{
	return skip + (spectrum-skip) * powf((float)(band+offset)/(float)(bands+offset),3.0f);
}

__forceinline double iterp(double p1, double p2, double s)
{
	return 2*lerp(p1,p2,s) - cosp(p1,p2,pow(s,0.9));
}

__forceinline float iterpf(float p1, float p2, float s)
{
	return 2*lerpf(p1,p2,s) - cospf(p1,p2,powf(s,0.9f));
}