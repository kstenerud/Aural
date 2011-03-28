//
//  AUCAudioContext.h
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

#ifndef AURAL_AUDIOCONTEXT_H
#define AURAL_AUDIOCONTEXT_H


#import "Mutex.h"


namespace aural
{
    class AudioSource;
    
    class AudioContext
    {
    public:
        virtual ~AudioContext() {};
        
        virtual AudioSource* newSource() = 0;
        virtual void deleteSource(AudioSource*const source) = 0;

        virtual double sampleRate() = 0;
        virtual void setSampleRate(const double sampleRate) = 0;

        virtual bool active() = 0;
        virtual void setActive(const bool value) = 0;
    };
    
}

#endif // AURAL_AUDIOCONTEXT_H
