// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Audio.h"

#include "fftsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Audio::Audio(void) :
mCreated(false),
mSoundCapture(NULL),
mCaptureBuffer(NULL),
mEqFalloff(0.5f),	// Default
mPeakFalloff(0.5f),	// Default
mPeakDelay(6.0f),	// Default
mAmplification(1.0f)
{
	// Zero the arrays
	ZeroMemory(mPowerSpectrum,sizeof(mPowerSpectrum));
	ZeroMemory(mLazySpectrum,sizeof(mLazySpectrum));
	ZeroMemory(mEq,sizeof(mEq));
    ZeroMemory(mScope,sizeof(mScope));
	ZeroMemory(mPeaks,sizeof(mPeaks));
	ZeroMemory(mVu,sizeof(mVu));
}

Audio::~Audio(void)
{
	DestroyAudio();
}

bool Audio::CreateAudio(void)
{
	ASSERT(!mCreated);

	// Create our main capture object
	HRESULT hr = DirectSoundCaptureCreate8(NULL, &mSoundCapture, NULL);
	if(FAILED(hr))
	{
		GetKernel()->SetLastError(L"Failed to create direct sound capture.");
		TRACE(L"Failed to create direct sound capture.");
		return false;
	}

	// Create the wave format description
	WAVEFORMATEX waveform;
	ZeroMemory(&waveform,sizeof(waveform));
	waveform.cbSize = sizeof(waveform);
	waveform.nChannels = ANALYZER_CHANNELS;
	waveform.nSamplesPerSec = ANALYZER_FREQUENCY;
	waveform.wBitsPerSample = ANALYZER_BITS;
	waveform.nBlockAlign = (waveform.nChannels * waveform.wBitsPerSample) / 8;
	waveform.nAvgBytesPerSec = waveform.nSamplesPerSec * waveform.nBlockAlign;
	waveform.wFormatTag = WAVE_FORMAT_PCM;

	// Create the record buffer format description
	DSCBUFFERDESC bufferdesc;
	ZeroMemory(&bufferdesc,sizeof(bufferdesc));
	bufferdesc.dwSize = sizeof(bufferdesc);
	bufferdesc.dwFlags = 0;
	bufferdesc.dwBufferBytes = ANALYZER_BUFFERSIZE;
	bufferdesc.dwFXCount = 0;
	bufferdesc.lpDSCFXDesc = 0;
	bufferdesc.lpwfxFormat = &waveform;

	// Create the capture buffer
	hr = mSoundCapture->CreateCaptureBuffer(&bufferdesc, &mCaptureBuffer, NULL);
	if(FAILED(hr))
	{
		GetKernel()->SetLastError(L"Failed to create direct sound capture buffer.");
		TRACE(L"Failed to create direct sound capture buffer.");
		return false;
	}

	// Enumerate recoding sources
	SetSource(0xFFFFFFFF);

	// Get magnification level
	if(GetCommandLineData()->GetVar(L"/amplification"))
		mAmplification = wcstod(GetCommandLineData()->GetVar(L"/amplification",String(L"1.0")),NULL);

	mCreated = true;

	// If we got to here that means everything is OK
	return true;
}

void Audio::DestroyAudio(void)
{
	mCreated = false;

	SAFERELEASE(mCaptureBuffer);
	SAFERELEASE(mSoundCapture);
}

#pragma region Mixer Source

bool Audio::SetSource(unsigned long index)
{
	if(mSources.GetSize())
		mSources.Erase();

	HMIXER mixer;
	if(mixerOpen(&mixer,NULL,NULL,(DWORD_PTR)GetModuleHandle(NULL),MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
	{
		TRACE(L"Failed to open mixer device 0.\n");
		return false;
	}

	MIXERCAPS caps;
	if(mixerGetDevCaps((UINT_PTR)mixer,&caps,sizeof(MIXERCAPS)) != MMSYSERR_NOERROR)
	{
		mixerClose(mixer);
		TRACE(L"Failed to get device caps for mixer device %S.\n",caps.szPname);
		return false;
	}

	MIXERLINE line;
	line.cbStruct = sizeof(MIXERLINE);

	for(unsigned int j = 0; j < caps.cDestinations; ++j)
	{
		line.dwDestination = j;
		if(mixerGetLineInfo((HMIXEROBJ)mixer,&line,MIXER_GETLINEINFOF_DESTINATION) != MMSYSERR_NOERROR)
		{
			TRACE(L"Failed to get line info for line %u on mixer device %S.\n",line.dwDestination,caps.szPname);
			continue;
		}
			
		if(line.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_WAVEIN)
		{
			TRACE(L"Getting line info for %s.\n",line.szName);
			
			MIXERLINECONTROLS controls;
			controls.cbStruct = sizeof(MIXERLINECONTROLS);
			controls.dwLineID = line.dwLineID;  // from MIXERLINE
			controls.cControls = line.cControls;// from MIXERLINE
			controls.cbmxctrl = sizeof(MIXERCONTROL);

			LPMIXERCONTROL control = (LPMIXERCONTROL)LocalAlloc(LPTR,sizeof(MIXERCONTROL) * line.cControls);
			if(!control)
			{
				TRACE(L"Failed to allocate %u bytes for line mixer control enumeration.\n",sizeof(MIXERCONTROL) * line.cControls);
				continue;
			}

			controls.pamxctrl = control;
			controls.dwControlType = 0;

			if(mixerGetLineControls((HMIXEROBJ)mixer, &controls, MIXER_GETLINECONTROLSF_ALL) != MMSYSERR_NOERROR)
			{
				LocalFree(control);
				TRACE(L"Failed to get mixer line controls.\n");
				continue;
			}

			for(unsigned int c = 0; c < line.cControls; ++c)
			{
				if((MIXERCONTROL_CONTROLTYPE_MUX & control[c].dwControlType) != MIXERCONTROL_CONTROLTYPE_MUX)
					continue;

				MIXERCONTROLDETAILS cd;
				cd.cbStruct = sizeof(MIXERCONTROLDETAILS);
				cd.dwControlID = control[c].dwControlID;
				cd.cChannels = line.cChannels;
				cd.cMultipleItems = control[c].cMultipleItems;
				cd.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);

				LPMIXERCONTROLDETAILS_LISTTEXT lt = (LPMIXERCONTROLDETAILS_LISTTEXT)LocalAlloc(LPTR,cd.cChannels * cd.cMultipleItems * cd.cbDetails);
				if(!lt)
				{
					TRACE(L"Failed to allocate %u bytes for control text details.\n",cd.cChannels * cd.cMultipleItems * cd.cbDetails);
					continue;
				}

				cd.paDetails = lt;
				if(mixerGetControlDetails((HMIXEROBJ)mixer, &cd, MIXER_GETCONTROLDETAILSF_LISTTEXT) != MMSYSERR_NOERROR)
				{
					LocalFree(lt);
					TRACE(L"Failed to get control details.\n");
					continue;
				}
				
				cd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
				
				LPMIXERCONTROLDETAILS_BOOLEAN lv = (LPMIXERCONTROLDETAILS_BOOLEAN)LocalAlloc(LPTR,cd.cChannels * cd.cMultipleItems * cd.cbDetails);
				if(!lv)
				{
					LocalFree(lt);
					TRACE(L"Failed to allocate %u bytes for control details boolean.\n",cd.cChannels * cd.cMultipleItems * cd.cbDetails);
					continue;
				}
				
				cd.paDetails = lv;

				if(mixerGetControlDetails((HMIXEROBJ)mixer, &cd, MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
				{
					LocalFree(lv);
					LocalFree(lt);
					TRACE(L"Failed to get control details.\n");
					continue;
				}
				
				for(unsigned int i = 0; i < cd.cMultipleItems; ++i)
				{
					TRACE(L"%20s [%s]\n",lt[i].szName,lv[i].fValue ? L"on" : L"off");
					mSources.InsertBack(lt[i].szName);

					if(index != -1)
						lv[i].fValue = FALSE;
				}

				if(index < cd.cMultipleItems)
					lv[index].fValue = TRUE;

				if(index != -1)
				{
					if(mixerSetControlDetails((HMIXEROBJ)mixer, &cd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
					{
						LocalFree(lv);
						LocalFree(lt);
						TRACE(L"Failed to set control details.\n");
						continue;
					}
				}

				LocalFree(lv);
				LocalFree(lt);
			}

			LocalFree(control);
		}
	}

	mixerClose(mixer);

    return true;
}

#pragma endregion

void Audio::Process(void)
{
	// Our buffers
	static short fft_buffer[ANALYZER_FFTBUFFERSIZE] = {0};

	static double fft_in[2][ANALYZER_FFTBUFFERSIZE/2] = {0};
	static double fft_out[2][ANALYZER_FFTBUFFERSIZE] = {0};
	static double fft_work[2][ANALYZER_FFTBUFFERSIZE/2] = {0};

	static int ip[2][64] = {{0},{0}};

	ZeroMemory(fft_buffer,sizeof(fft_buffer));

	// Capture the audio data into the main buffer
	if(!Capture(fft_buffer,sizeof(fft_buffer)))
		return;

	// Volume amplifier
	if(mAmplification != 1.0f)
		for(unsigned long i = 0; i < ANALYZER_FFTBUFFERSIZE; ++i)
			fft_buffer[i] = (unsigned short)(fft_buffer[i] * mAmplification);

	// Seperate the main buffer into the left and right chanels
	GetSeperateChannels(fft_in[0],fft_in[1],fft_buffer,ANALYZER_FFTBUFFERSIZE);

	// Copy the input data to the input-output buffer
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
	{
		fft_out[0][x*2] = fft_in[0][x];
		fft_out[0][x*2+1] = 0;
		fft_out[1][x*2] = fft_in[1][x];
		fft_out[1][x*2+1] = 0;
	}

	// Use the Blackman-Harris window
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
	{
		double mag = GetBlackmanHarrisWindow(x,ANALYZER_FFTBUFFERSIZE/2);

		fft_out[0][x*2] *= mag;
		fft_out[0][x*2+1] *= mag;
		fft_out[1][x*2] *= mag;
		fft_out[1][x*2+1] *= mag;
	}

	// Calculate the Real Discrete Fourier Transform
	rdft(ANALYZER_FFTBUFFERSIZE, 1, fft_out[0], ip[0], fft_work[0]);
	rdft(ANALYZER_FFTBUFFERSIZE, 1, fft_out[1], ip[1], fft_work[1]);

	// Calculate the Power Spectrum by suming the square of the real and the square of the imaginary part
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
	{
		fft_out[0][x*2] = sqrt(fft_out[0][x*2] * fft_out[0][x*2] + fft_out[0][x*2+1] * fft_out[0][x*2+1]);
		fft_out[0][x*2+1] = 0;
		fft_out[1][x*2] = sqrt(fft_out[1][x*2] * fft_out[1][x*2] + fft_out[1][x*2+1] * fft_out[1][x*2+1]);
		fft_out[1][x*2+1] = 0;
	}

	// Calc the power spectrum array
	GetPowerSpectrum(mPowerSpectrum[0],fft_out[0],ANALYZER_FFTBUFFERSIZE/4);
	GetPowerSpectrum(mPowerSpectrum[1],fft_out[1],ANALYZER_FFTBUFFERSIZE/4);

	// Calc the bars that will be displayed
	GetEq(mEq[0],fft_out[0],ANALYZER_FFTBUFFERSIZE/4);
	GetEq(mEq[1],fft_out[1],ANALYZER_FFTBUFFERSIZE/4);

	// Calc the lazy spectrum
	GetLazySpectrum(mLazySpectrum[0],fft_out[0],ANALYZER_FFTBUFFERSIZE/4);
	GetLazySpectrum(mLazySpectrum[1],fft_out[1],ANALYZER_FFTBUFFERSIZE/4);

	// Calc the audio peak levels
	GetPeaks(mPeaks[0],mEq[0],ANALYZER_NUMBANDS);
	GetPeaks(mPeaks[1],mEq[1],ANALYZER_NUMBANDS);

	// Calc the average of the left and right channel for our scope
	//GetScope(mScope,fft_in[0],fft_in[1],ANALYZER_FFTBUFFERSIZE/2);
	
	// Calc the vu meter level
	//GetVu(mVu,fft_in[0],fft_in[1],ANALYZER_FFTBUFFERSIZE/2);
	
	{
		static short scope_buffer[ANALYZER_BUFFERSIZE/4];
		static double scope_buffer_channels[2][ANALYZER_BUFFERSIZE/8];

		// Capture the audio data into the main buffer
		if(!Capture(scope_buffer,sizeof(scope_buffer)))
			return;

		// Volume amplifier
		if(mAmplification != 1.0)
			for(unsigned long i = 0; i < ANALYZER_BUFFERSIZE/4; ++i)
				scope_buffer[i] = (unsigned short)(scope_buffer[i] * mAmplification);
	
		// Seperate the main buffer into the left and right chanels
		GetSeperateChannels(scope_buffer_channels[0],scope_buffer_channels[1],scope_buffer,ANALYZER_BUFFERSIZE/4);

		static const unsigned long factor = ANALYZER_BUFFERSIZE/8/(ANALYZER_FFTBUFFERSIZE/2);

		// Calc the level of the scope line
		for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
		{
			float level = (scope_buffer_channels[0][x*factor] + scope_buffer_channels[1][x*factor]) / 2.0f / float((1 << ANALYZER_BITS) / 2);

			// We calc the average for the final level
			//double level = 0.0;
			
			//for(unsigned long k = 0; k < factor; ++k)
			//	level = (scope_buffer_channels[0][x*factor+k] + scope_buffer_channels[1][x*factor+k]) / 2.0f / double((1 << ANALYZER_BITS) / 2);

			//level /= factor;

			// Do a linear interpolation for smoothnes
			mScope[x] = lerpf(mScope[x],level,min(1.0,mEqFalloff * GetKernel()->GetElapsedTime()));
		}

		GetVu(mVu,scope_buffer_channels[0],scope_buffer_channels[1],ANALYZER_BUFFERSIZE/8);
	}
}

bool Audio::Capture(short* buffer, unsigned long bufferSize)
{
	unsigned long readCursor, lockOffset;
	void* firstBuf, *secondBuf;
	unsigned long firstLen, secondLen;

	// Get the position of the read cursor in the capture buffer
	// We can read everything up to this point (and not including it)
	mCaptureBuffer->GetCurrentPosition(NULL, &readCursor);

	// Calculate a starting point in the buffer behind the read cursor to lock at
	if(readCursor < bufferSize)
		lockOffset = ANALYZER_BUFFERSIZE - readCursor - bufferSize;
	else
		lockOffset = readCursor - bufferSize;

	// Lock the capture buffer and grab some data, ah yeah!
	if(FAILED(mCaptureBuffer->Lock(lockOffset, bufferSize, &firstBuf, &firstLen, &secondBuf, &secondLen, 0)))
	{
		GetKernel()->SetLastError(L"Failed to lock audio buffer.");
		TRACE(L"IDirectSoundCaptureBuffer8::Lock() failed! lockOffset=%d readCursor=%d\n",lockOffset,readCursor);
		return false;
	}

	// Copy the data out of the first buffer
	if(firstLen)
		CopyMemory(buffer, firstBuf, firstLen);

	// Is there more (wrapped) data to copy?  If so, append it to the first chunk of copied data
	if(secondLen)
		CopyMemory(buffer + firstLen, secondBuf, secondLen);

	// Unlock the buffer
	if(FAILED(mCaptureBuffer->Unlock(firstBuf, firstLen, secondBuf, secondLen)))
	{
		GetKernel()->SetLastError(L"Failed to unlock audio buffer.");
		TRACE(L"IDirectSoundCaptureBuffer8::Unlock() failed!\n");
		return false;
	}

	return true;
}

void Audio::GetPowerSpectrum(float* ps,const double* ch,unsigned long size)
{
	// Just lerp-copy the array
	for(unsigned long x = 0; x < size; ++x)
	{
		// Get magnitude of this band
		//float mag = GetMagnitude(ch[x*2],260.0f,4.0f,1.22f,2.2f,x,size);
		float mag = GetMagnitude(ch[x*2],200.0f,4.0f,0.5f,1.5f,x,size);
		//float mag = 20*log10f(ch[x*2]);

		// Preform a linear interpolation between the last eq state and the new eq state for smooth display
		//ps[x] = iterpf(ps[x],-max(mag,0.0),min(1.0,mEqFalloff * GetKernel()->GetElapsedTime()));

		if(-ps[x] < mag)
			ps[x] = -mag;
		else
		{
			ps[x] += 35.0f*mEqFalloff*GetKernel()->GetElapsedTime();

			if(-ps[x] < 0.0f)
				ps[x] = 0.0f;
		}
	}
}

void Audio::GetLazySpectrum(float* ls,const double* ch,unsigned long size)
{
	// Just lerp-copy the array
	for(unsigned long x = 0; x < size; ++x)
	{
		// Get magnitude of this band
		//float mag = GetMagnitude(ch[x*2],260.0f,4.0f,1.22f,2.2f,x,size);
		float mag = GetMagnitude(ch[x*2],200.0f,4.0f,0.5f,1.5f,x,size);
		//float mag = 20*log10f(ch[x*2]);

		if(ls[x] < mag)
			ls[x] = mag;
		else
		{
			ls[x] -= 35.0f*mEqFalloff*GetKernel()->GetElapsedTime();

			if(ls[x] < 0.0f)
				ls[x] = 0.0f;
		}
	}
}

void Audio::GetEq(float* eq,const double* ch,unsigned long size)
{
	for(unsigned long x = 0; x < ANALYZER_NUMBANDS; ++x)
	{
		// We calc the average of levels for the final band
		double level = 0.0;

		unsigned long lower = GetFrequencySubspectrum(x,ANALYZER_NUMBANDS,size,4,1);
		unsigned long upper = GetFrequencySubspectrum(x+1,ANALYZER_NUMBANDS,size,4,1);

		for(unsigned long i = lower; i < upper; ++i)
			level += ch[i*2];
			//level += GetMagnitude(ch[i*2],260.0f,4.0f,1.22f,2.2f,i,size);
		
		level /= upper - lower;

		// Get magnitude of this band
		level = GetMagnitude(level,260.0f,4.1f,1.06f,2.0f,x,ANALYZER_NUMBANDS);

		// Preform a linear interpolation between the last eq state and the new eq state for smooth display
		eq[x] = iterp(eq[x],-max(level,0.0),min(1.0,mEqFalloff * GetKernel()->GetElapsedTime()));
	}
}

void Audio::GetSeperateChannels(double* left, double* right,const short* buffer,unsigned long bufferSize)
{
	// Seperate the left chanel from the main buffer
	for(unsigned long i = 0, j = 0; i < bufferSize; i += 2, ++j)
		left[j] = (double)buffer[i];

	// Seperate the right chanel from the main buffer
	for(unsigned long i = 1, j = 0; i < bufferSize; i += 2, ++j)
		right[j] = (double)buffer[i];
}

void Audio::GetPeaks(Peak* peaks,const float* eq,unsigned long size)
{
	// Calc the height of the peaks
	for(unsigned long x = 0; x < size; ++x)
	{
		if(peaks[x].position < -eq[x])	// Eq is negative so must convert to positive first
		{
#ifdef ANALYZER_BOUNCINGPEAKS
			peaks[x].velocity = (eq[x] - peaks[x].position)/200.0;
			peaks[x].position = -eq[x];
			peaks[x].life = 0.0f;
#else
			peaks[x].position = -eq[x];
			peaks[x].life = mPeakDelay;
			peaks[x].velocity = 0.0f;
#endif
		}
		else
		{
			if(peaks[x].life > 0.0f)
				peaks[x].life -= 0.5f * GetKernel()->GetElapsedTime();
			else if(peaks[x].position > 0.0f)
			{
				peaks[x].position -= peaks[x].velocity * GetKernel()->GetElapsedTime();
				
				//peaks[x].velocity = lerp(peaks[x].velocity,20.0f,min(1.0f,0.1f * GetKernel()->GetElapsedTime()));
				peaks[x].velocity += mPeakFalloff * GetKernel()->GetElapsedTime();
			}
		}
	}
}

void Audio::GetScope(float* scope,const float* left,const float* right,unsigned long size)
{
	// Calc the level of the scope line
	for(unsigned long x = 0; x < size; ++x)
	{
		// We calc the average for the final level
		float level = (left[x] + right[x]) / 2.0f / float((1 << ANALYZER_BITS) / 2);

		// Do a linear interpolation for smoothnes
		scope[x] = lerpf(scope[x],level,min(1.0,mEqFalloff * GetKernel()->GetElapsedTime()));
	}
}

void Audio::GetVu(float* vu,const double* left,const double* right,unsigned long size)
{
	double vu1 = 0.0;
	double vu2 = 0.0;

	// Calc average level
	for(unsigned long x = 0; x < size; ++x)
	{
		vu1 += fabs(left[x] / double((1 << ANALYZER_BITS) / 2));
		vu2 += fabs(right[x] / double((1 << ANALYZER_BITS) / 2));
	}

	vu1 /= (double)size;
	vu2 /= (double)size;

	// Magnitude
	vu1 *= 1.4;
	vu2 *= 1.4;

	vu[0] = iterpf(vu[0],vu1,min(1.0f,mEqFalloff * GetKernel()->GetElapsedTime()));
	vu[1] = iterpf(vu[1],vu2,min(1.0f,mEqFalloff * GetKernel()->GetElapsedTime()));
}

void Audio::GetMiddleChannel(double* out,const double* left,const double* right,unsigned long size)
{
	// Calc the level of the scope line
	for(unsigned long x = 0; x < size; ++x)
	{
		// We calc the average for the final level
		out[x] = (left[x] + right[x]) / 2.0;
	}
}

double Audio::GetBlackmanHarrisWindow(unsigned long x,unsigned long size)
{
	static const double a0 = 0.35875;
	static const double a1 = 0.48829;
	static const double a2 = 0.14128;
	static const double a3 = 0.01168;
	static const double twopi = M_PI * 2.0;
	static const double fourpi = M_PI * 4.0;
	static const double sixpi = M_PI * 6.0;

	return a0 - a1 * cos(twopi * (double)(x + 0.5) / (double)(size)) +
			a2 * cos(fourpi * (double)(x + 0.5) / (double)(size)) - 
			a3 * cos(sixpi * (double)(x + 0.5) / (double)(size));
}

float Audio::GetMagnitude(float level,float scale,float magnitude,float cutrange,float sharpness,unsigned long x,unsigned long size)
{
	float power = powf(10.0f, log10f(1.0f + cutrange) / powf((float)(size - 1), sharpness));

	return scale * (log10f(level) - magnitude - powf(power, powf((float)(size - 1) - (float)x, sharpness)) + 1.0f);
}

float Audio::GetAverage(const float* buffer,unsigned long bufferSize) const
{
	float average = 0.0;

	for(unsigned long x = 0; x < bufferSize; ++x)
		average += buffer[x];

	return average / bufferSize;
}

float Audio::GetTotal(const float* buffer,unsigned long bufferSize) const
{
	float total = 0.0;

	for(unsigned long x = 0; x < bufferSize; ++x)
		total += buffer[x];

	return total;
}

float Audio::GetAverageAbs(const float* buffer,unsigned long bufferSize) const
{
	float average = 0.0;

	for(unsigned long x = 0; x < bufferSize; ++x)
		average += fabs(buffer[x]);

	return average / bufferSize;
}

float Audio::GetTotalAbs(const float* buffer,unsigned long bufferSize) const
{
	float total = 0.0;

	for(unsigned long x = 0; x < bufferSize; ++x)
		total += fabs(buffer[x]);

	return total;
}

bool Audio::StartCapture(void)
{
	if(FAILED(mCaptureBuffer->Start(DSCBSTART_LOOPING)))
	{
		GetKernel()->SetLastError(L"Failed to start sound capture.");
		TRACE(L"Failed to start sound capture.\n");
		return false;
	}

	return true;
}

bool Audio::StopCapture(void)
{
	if(FAILED(mCaptureBuffer->Stop()))
	{
		GetKernel()->SetLastError(L"Failed to stop sound capture.");
		TRACE(L"Failed to stop sound capture.\n");
		return false;
	}

	return true;
}

void Audio::SetEqFalloff(long falloff)
{
	falloff = min(1,max(falloff,-1));	// Camp value to filter out bogus settings
	
	if(falloff == -1)
	{
		if(mEqFalloff == 0.2f)
			return;

		mEqFalloff = 0.2f;

		TRACE(L"Eq falloff set to slow.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"eq falloff: \002slow\001");
	}
	else if(falloff == 0)
	{
		if(mEqFalloff == 0.5f)
			return;

		mEqFalloff = 0.5f;

		TRACE(L"Eq falloff set to normal.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"eq falloff: \002normal\001");
	}
	else if(falloff == 1)
	{
		if(mEqFalloff == 0.8f)
			return;

		mEqFalloff = 0.8f;

		TRACE(L"Eq falloff set to fast.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"eq falloff: \002fast\001");
	}
}

void Audio::SetPeakFalloff(long falloff)
{
	falloff = min(1,max(falloff,-1));	// Camp value to filter out bogus settings
	
	if(falloff == -1)
	{
		if(mPeakFalloff == 0.2f)
			return;

		mPeakFalloff = 0.2f;

		TRACE(L"Peak falloff set to slow.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"peak falloff: \002slow\001");
	}
	else if(falloff == 0)
	{
		if(mPeakFalloff == 0.5f)
			return;

		mPeakFalloff = 0.5f;

		TRACE(L"Peak falloff set to normal.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"peak falloff: \002normal\001");
	}
	else if(falloff == 1)
	{
		if(mPeakFalloff == 0.8f)
			return;

		mPeakFalloff = 0.8f;

		TRACE(L"Peak falloff set to fast.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"peak falloff: \002fast\001");
	}
}

void Audio::SetPeakDelay(long delay)
{
	delay = min(1,max(delay,-1));	// Camp value to filter out bogus settings
	
	if(delay == -1)
	{
		if(mPeakDelay == 2.0f)
			return;

		mPeakDelay = 2.0f;

		TRACE(L"Peak delay set to low.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"peak delay: \002low\001");
	}
	else if(delay == 0)
	{
		if(mPeakDelay == 6.0f)
			return;

		mPeakDelay = 6.0f;

		TRACE(L"Peak delay set to normal.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"peak delay: \002normal\001");
	}
	else if(delay == 1)
	{
		if(mPeakDelay == 10.0f)
			return;

		mPeakDelay = 10.0f;

		TRACE(L"Peak delay set to high.\n");
		GetKernel()->GetGraphics()->PrintOutputString(L"peak delay: \002high\001");
	}
}