//
//  SoundTouchRenderFilter.h
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

#ifndef AURAL_SOUNDTOUCHRENDERFILTER_H
#define AURAL_SOUNDTOUCHRENDERFILTER_H


#include "AudioRenderFilter.h"
#include <SoundTouch/SoundTouch.h>


namespace aural
{
    class SoundTouchRenderFilter: public BasicAudioRenderFilter
    {
    public:
        SoundTouchRenderFilter();
        ~SoundTouchRenderFilter();

        void reset();
        void prepare();
        void readFrames(const unsigned long numFrames, void*const dst);
        void skipFrames(const unsigned long numFrames);

        bool enabled();
        void setEnabled(const bool enabled);

		void setPitchSemiTones(const float newPitch);

    private:
        void preload();

		soundtouch::SoundTouch soundTouch_;
        bool enabled_;
    };

    inline bool SoundTouchRenderFilter::enabled()
    {
        return enabled_;
    }
    
}

#endif // AURAL_SOUNDTOUCHRENDERFILTER_H
