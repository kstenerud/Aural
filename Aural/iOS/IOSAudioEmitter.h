//
//  IOSAudioEmitter.h
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

#ifndef AURAL_IOSAUDIOEMITTER_H
#define AURAL_IOSAUDIOEMITTER_H


#import <AudioToolbox/AudioToolbox.h>
#include "AudioEmitter.h"
#include "IOSAudioData.h"
#include "IOSRenderFilters.h"
#include "AudioUnitHelpers.h"

// TODO: hardware capable emitter, software emitter
// stream emitter, buffer emitter?

namespace aural
{
    class IOS3DMixerAudioEnvironment;
    
    class IOSAudioEmitter: public AudioEmitter
    {
    public:
        IOSAudioEmitter(IOS3DMixerAudioEnvironment& environment,
                        const UInt32 elementNumber);
        ~IOSAudioEmitter();
        
        AudioData* audioData();
        
        void setAudioData(AudioData*const audioData);
        
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
        
        float playbackRate();
        void setPlaybackRate(const float playbackRate);
        
        void play();
        void stop();
        
        void setDistance(const float distance);
        
        UInt32 elementNumber();
        
    private:
        IOSAudioEmitter(const IOSAudioEmitter&);
        IOSAudioEmitter& operator=(const IOSAudioEmitter&);
        
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
        
        IOS3DMixerAudioEnvironment& audioEnvironment_;
        AudioUnitAccessor audioUnitAccessor_;
        MonoBufferRenderSource renderSource_;
        IOS3DMixerRenderFilter renderSink_;
        
        AudioRenderFilter* currentFilter_;
        
        IOSAudioData* audioData_;
        
        float pitch_;
        float playbackRate_;
    };
}

#endif // AURAL_IOSAUDIOEMITTER_H
