//
//  SoundTouchRenderFilter.cpp
//  Aural
//
//  Created by Karl Stenerud on 3/27/11.
//
// Copyright 2011 Karl Stenerud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Note: You are NOT required to make the license available from within your
// iOS application. Including it in your project is sufficient.
//
// Attribution is not required, but appreciated :)
//

#include "SoundTouchRenderFilter.h"
#include "Util.h"
#include "Logging.h"


// Enables trace logging in this file. Trace logs go to the debug channel.
#define TRACE_LOGGING_ENABLED 0

#if TRACE_LOGGING_ENABLED
    #define LOG_TRACE(FMT, ...) AURAL_LOG_DEBUG(FMT, ##__VA_ARGS__)
#else
    #define LOG_TRACE(FMT, ...)
#endif


#define kSoundTouchLatencyFrames 8192
#define kInternalBufferSize 1024

namespace aural
{
    void SoundTouchRenderFilter::setPitchSemiTones(float newPitch)
    {
        soundTouch_.setPitchSemiTones(newPitch);
    }

	using namespace soundtouch;
    
	SoundTouchRenderFilter::SoundTouchRenderFilter()
	{
		int sampleRate = 44100;
		int channels = 1;
		
		soundTouch_.setSampleRate(sampleRate);
		soundTouch_.setChannels(channels);
		
		soundTouch_.setTempoChange(0.0f);
		soundTouch_.setPitchSemiTones(0.0f);
		soundTouch_.setRateChange(0.0f);
		
		soundTouch_.setSetting(SETTING_USE_QUICKSEEK, TRUE);
		soundTouch_.setSetting(SETTING_USE_AA_FILTER, FALSE);
		
		if (0)
		{
			// use settings for speech processing
			soundTouch_.setSetting(SETTING_SEQUENCE_MS, 40);
			soundTouch_.setSetting(SETTING_SEEKWINDOW_MS, 15);
			soundTouch_.setSetting(SETTING_OVERLAP_MS, 8);
		}
	}
	
	SoundTouchRenderFilter::~SoundTouchRenderFilter()
	{
	}
    
    void SoundTouchRenderFilter::setEnabled(bool enabled)
    {
        // TODO
    }
    
    
	void SoundTouchRenderFilter::preload()
	{
        SAMPLETYPE temp[kSoundTouchLatencyFrames];
        
		sourceReadFrames(kSoundTouchLatencyFrames, temp);
        soundTouch_.putSamples(temp, kSoundTouchLatencyFrames);
        LOG_TRACE("Preloaded %d frames", kSoundTouchLatencyFrames);
    }

    void SoundTouchRenderFilter::reset()
	{
        BasicAudioRenderFilter::reset();
        soundTouch_.clear();
    }

	void SoundTouchRenderFilter::prepare()
	{
        BasicAudioRenderFilter::prepare();
        preload();
	}
    
    inline void SoundTouchRenderFilter::readFrames(unsigned long numFrames, void* buffer)
    {
        SAMPLETYPE temp[kInternalBufferSize];
        
        unsigned long framesLeft = numFrames;
        unsigned long framesToWrite = 0;
        while(framesLeft > 0)
        {
            unsigned long framesToRead = framesLeft > kInternalBufferSize ? kInternalBufferSize : framesLeft;
            
            sourceReadFrames(framesToRead, temp);
            soundTouch_.putSamples(temp, framesToRead);
            framesToWrite += framesToRead;
            unsigned long framesWritten = soundTouch_.receiveSamples(static_cast<SAMPLETYPE*>(buffer), framesToWrite);
            LOG_TRACE("Read %d and wrote %d", framesToRead, framesWritten);
            framesToWrite -= framesWritten;
            framesLeft -= framesToRead;
        }
        
        while(framesToWrite > 0)
        {
            unsigned long framesWritten = soundTouch_.receiveSamples(static_cast<SAMPLETYPE*>(buffer), framesToWrite);
            if(0 == framesWritten)
            {
                AURAL_LOG_ERROR("Could not read %d frames from SoundTouch", framesToWrite);
                break;
            }
            LOG_TRACE("Wrote %d", framesWritten);
            framesToWrite -= framesWritten;
        }
    }
    
    inline void SoundTouchRenderFilter::skipFrames(unsigned long numFrames)
    {
        // TODO
    }
}
