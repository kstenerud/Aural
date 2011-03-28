//
//  IOSAudioSource.cpp
//  Aural
//
//  Created by Karl Stenerud on 3/26/11.
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

#include "IOSAudioSource.h"
#include "IOSAudioContext.h"
#include "Macros.h"
#include "SoundTouchRenderFilter.h"
#include "Logging.h"


// Enables trace logging in this file. Trace logs go to the debug channel.
#define TRACE_LOGGING_ENABLED 0

#if TRACE_LOGGING_ENABLED
    #define LOG_TRACE(FMT, ...) AURAL_LOG_DEBUG(FMT, ##__VA_ARGS__)
#else
    #define LOG_TRACE(FMT, ...)
#endif


namespace aural
{
    IOSAudioSource::IOSAudioSource(IOS3DMixerAudioContext& context, UInt32 elementNumber)
    : audioContext_(context)
    , audioUnitAccessor_(context.mixerUnit(),
                         kAudioUnitScope_Input,
                         elementNumber)
    , renderSink_(audioUnitAccessor_, context.mutex())
    , gain_(1.0f)
    , pan_(0.5f)
    , currentFilter_(new SoundTouchRenderFilter())
    {
//        renderSink_.setSource(&renderSource_);
        renderSink_.setSource(currentFilter_);
        currentFilter_->setSource(&renderSource_);

        LOG_TRACE("Initialized source %d", audioUnitAccessor_.element());
    }
    
    IOSAudioSource::~IOSAudioSource()
    {
    }
    
    void IOSAudioSource::setBuffer(AudioBuffer* buffer)
    {
        OPTIONAL_LOCK(audioContext_.mutex());
        audioBuffer_ = buffer;
        renderSource_.setBuffer(buffer);
        audioUnitAccessor_.setStreamFormat(buffer->format());
        renderSink_.prepare();
    }
    
    AudioBuffer* IOSAudioSource::buffer()
    {
        return audioBuffer_;
    }
    
    bool IOSAudioSource::muted()
    {
        return muted_;
        
    }
    
    void IOSAudioSource::setMuted(bool muted)
    {
            OPTIONAL_LOCK(audioContext_.mutex());
            if(muted != muted_)
            {
                //NSLog(@"Set %d to muted %d", elementNumber_, muted);
                /*
                 error = AudioUnitSetParameter(self->context->mixerUnit,
                 kMultiChannelMixerParam_Enable,
                 kAudioUnitScope_Input,
                 self->elementNumber,
                 !muted,
                 0);
                 */
            }
    }
    
    bool IOSAudioSource::paused()
    {
        return paused_;
    }
    
    void IOSAudioSource::setPaused(bool paused)
    {
        if(paused != paused_)
        {
            paused = paused_;
            renderSink_.setEnabled(!paused_);
        }
    }
	
    void IOSAudioSource::play()
    {
        if(playing_)
        {
            stop();
        }
        
        renderSink_.setEnabled(TRUE);
        playing_ = TRUE;
        paused_ = FALSE;
    }
    
    void IOSAudioSource::stop() 
    {
        // TODO: Calling reset/prepare will reset SoundTouch to the CURRENT
        // pitch value. If you change pitch, it will still have preloaded data
        // from the OLD pitch value.
        renderSink_.setEnabled(FALSE);
        renderSink_.reset();
        renderSink_.prepare();
        playing_ = FALSE;
        paused_ = FALSE;
        currentBytePos_ = 0;
    }
    
    float IOSAudioSource::gain()
    {
        return gain_;
    }
    
    void IOSAudioSource::setGain(float gain)
    {
        OPTIONAL_LOCK(audioContext_.mutex());
        if(gain != gain_)
        {
            // Converting to sound pressure, so * 20
            Float32	db = fmaxf(log10f(gain) * 20.0f, -120.0f);
            audioUnitAccessor_.setParameter(k3DMixerParam_Gain, db);
            gain_ = gain;
        }
    }
    
    float IOSAudioSource::pitch()
    {
        return pitch_;
    }
	
    void IOSAudioSource::setPitch(float pitch)
    {
        float totalAmount = (pitch - 1.0f) * 2.0f;
        //	NSLog(@"%f = %f", pitch, totalAmount);
        
        ((SoundTouchRenderFilter*)currentFilter_)->setPitchSemiTones(totalAmount);
//        pitch_ = pitch;
    }
    
    void IOSAudioSource::setPlaybackRate(float pitch)
    {
        OPTIONAL_LOCK(audioContext_.mutex());
        if(pitch != pitch_)
        {
            audioUnitAccessor_.setParameter(k3DMixerParam_PlaybackRate, pitch);
            pitch_ = pitch;
        }
    }
    
    float IOSAudioSource::pan()
    {
        return pan_;
    }
    
    void IOSAudioSource::setPan(float pan)
    {
        OPTIONAL_LOCK(audioContext_.mutex());
        if(pan != pan_)
        {
            // Distance needs to be something other than 0 (inside your head)
            setDistance(1.0f);
            //kAudioUnitProperty_3DMixerAttenuationCurve
            Float32 azimuth = fminf(pan*90, 90);
            azimuth = fmaxf(azimuth, -90);
            audioUnitAccessor_.setParameter(k3DMixerParam_Azimuth, azimuth);
            pan_ = pan;
        }
    }
    
    void IOSAudioSource::setDistance(float distance)
    {
        OPTIONAL_LOCK(audioContext_.mutex());
        if(distance != distance_)
        {
            audioUnitAccessor_.setParameter(k3DMixerParam_Distance, distance);
            distance_ = distance;
        }
    }
    
    UInt32 IOSAudioSource::elementNumber()
    {
        return audioUnitAccessor_.element();
    }
}
