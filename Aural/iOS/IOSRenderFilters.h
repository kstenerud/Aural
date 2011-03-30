//
//  IOSRenderFilters.h
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

#ifndef AURAL_IOSRENDERFILTERS_H
#define AURAL_IOSRENDERFILTERS_H


#include "AudioRenderFilter.h"
#include "IOSAudioData.h"


namespace aural
{
    class AudioUnitAccessor;
    class Mutex;
    
    class IOS3DMixerRenderFilter: public BasicAudioRenderFilter
    {
    public:
        IOS3DMixerRenderFilter(AudioUnitAccessor& accessor, Mutex& mutex);
        
        void readFrames(const unsigned long numFrames, void*const dst);
        
        bool enabled();
        void setEnabled(const bool value);
        bool muted();
        void setMuted(const bool value);
    private:
        AudioUnitAccessor& accessor_;
        Mutex& mutex_;
        bool enabled_;
        bool muted_;
    };
    
    
    inline bool IOS3DMixerRenderFilter::enabled()
    {
        return enabled_;
    }
    
    inline bool IOS3DMixerRenderFilter::muted()
    {
        return muted_;
    }
    
    class MonoBufferRenderSource: public AudioRenderSource
    {
    public:
        void setBuffer(AudioData*const buffer);
        unsigned long position();
        void setPosition(const unsigned long frameNumber);
        
        void reset();
        void prepare();
        const unsigned long frameSize();
        void readFrames(const unsigned long numFrames, void*const dst);
        void skipFrames(const unsigned long numFrames);
        
    private:
        IOSAudioData* audioData_;
        unsigned long frameSize_;
        unsigned long numFrames_;
        unsigned long position_;
    };
    
    
    inline void MonoBufferRenderSource::setBuffer(AudioData*const buffer)
    {
        audioData_ = static_cast<IOSAudioData*>(buffer);
        frameSize_ = 2;
        numFrames_ = audioData_->numFrames();
        
        setPosition(0);
    }
    
    inline unsigned long MonoBufferRenderSource::position()
    {
        return position_;
    }
    
    inline void MonoBufferRenderSource::setPosition(const unsigned long frameNumber)
    {
        position_ = frameNumber;
    }
    
    inline void MonoBufferRenderSource::reset()
    {
        setPosition(0);
    }
    
    inline void MonoBufferRenderSource::prepare()
    {
        // Nothing to do
    }
    
    inline const unsigned long MonoBufferRenderSource::frameSize()
    {
        return frameSize_;
    }
    
    inline void MonoBufferRenderSource::readFrames(const unsigned long numFrames, void*const dst)
    {
        size_t newPosition = copy_circular(dst,
                                           numFrames * frameSize_,
                                           audioData_->leftChannelData(),
                                           position_ * frameSize_,
                                           audioData_->numBytes());
        
        position_ = newPosition / frameSize_;
    }
    
    inline void MonoBufferRenderSource::skipFrames(const unsigned long numFrames)
    {
        size_t newPosition = advance_circular(numFrames * frameSize_,
                                              position_ * frameSize_,
                                              audioData_->numBytes());
        
        position_ = newPosition / frameSize_;
    }
}

#endif // AURAL_IOSRENDERFILTERS_H
