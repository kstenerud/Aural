//
//  IOSAudioSource.h
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

#ifndef AURAL_IOSAUDIOSOURCE_H
#define AURAL_IOSAUDIOSOURCE_H


#import <AudioToolbox/AudioToolbox.h>
#include "AudioSource.h"
#include "CommonRenderFilters.h"
#include "IOSRenderFilters.h"
#include "AudioUnitHelpers.h"


namespace aural
{
    class IOS3DMixerAudioContext;
    
    class IOSAudioSource: public AudioSource
    {
    public:
        IOSAudioSource(IOS3DMixerAudioContext& context, const UInt32 elementNumber);
        ~IOSAudioSource();

        AudioBuffer* buffer();
        
        void setBuffer(AudioBuffer*const buffer);
        
        bool paused();
        void setPaused(const bool paused);
        
        bool muted();
        void setMuted(const bool muted);
        
        float gain();
        void setGain(const float gain);
        
        float pan();
        void setPan(const float pan);
        
        float pitch();
        void setPitch(const float pitch);
        
        void setPlaybackRate(const float playbackRate);
        
        void play();
        void stop();

        void setDistance(const float distance);
        
        UInt32 elementNumber();

    private:
        IOSAudioSource(const IOSAudioSource&);
        IOSAudioSource& operator=(const IOSAudioSource&);

    private:	
        float gain_;
        bool paused_;
        bool playing_;
        float pan_;
        
        // Internal
        float distance_;
        // position or pan...
        // playback finished callback...
        
        unsigned int currentBytePos_;
        
        IOS3DMixerAudioContext& audioContext_;
        AudioUnitAccessor audioUnitAccessor_;
        MonoBufferRenderSource renderSource_;
        IOS3DMixerRenderFilter renderSink_;
        
        AudioRenderFilter* currentFilter_;
        
        AudioBuffer* audioBuffer_;

        float pitch_;
        float playbackRate_;
    };
}

#endif // AURAL_IOSAUDIOSOURCE_H
