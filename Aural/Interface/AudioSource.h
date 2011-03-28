//
//  AUCAudioSource.h
//  Aural
//
//  Created by Karl Stenerud on 2/19/11.
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

#ifndef AURAL_AUDIOSOURCE_H
#define AURAL_AUDIOSOURCE_H


namespace aural
{
    class AudioBuffer;
    
    class AudioSource
    {
    public:
        virtual ~AudioSource() {};
        
        virtual AudioBuffer* buffer() = 0;

        virtual void setBuffer(AudioBuffer* buffer) = 0;
        
        virtual bool paused() = 0;
        virtual void setPaused(bool paused) = 0;
        
        virtual bool muted() = 0;
        virtual void setMuted(bool muted) = 0;
        
        virtual float gain() = 0;
        virtual void setGain(float gain) = 0;
        
        virtual float pan() = 0;
        virtual void setPan(float pan) = 0;
        
        virtual void setPitch(float pitch) = 0;
        
        virtual void setPlaybackRate(float playbackRate) = 0;
        
        virtual void play() = 0;
        virtual void stop() = 0;
    };
}

#endif // AURAL_AUDIOSOURCE_H

