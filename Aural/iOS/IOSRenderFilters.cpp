//
//  IOSRenderFilters.cpp
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

#include "IOSRenderFilters.h"
#include <AudioToolbox/AudioToolbox.h>
#include "AudioUnitHelpers.h"
#include "Mutex.h"
#include "Macros.h"


static const unsigned int kLeftChannel = 0;
static const unsigned int kRightChannel = 1;


namespace aural
{
    static OSStatus IOS3DMixerRenderSink_renderCallback(void* inRefCon,
                                   AudioUnitRenderActionFlags* ioActionFlags,
                                   const AudioTimeStamp* inTimeStamp,
                                   UInt32 inElementNumber,
                                   UInt32 inNumberFrames,
                                   AudioBufferList* ioData)
    {
        IOS3DMixerRenderFilter* filter = (IOS3DMixerRenderFilter*)inRefCon;
        
        if(filter->muted())
        {
            filter->skipFrames(inNumberFrames);
            memset(ioData->mBuffers[kLeftChannel].mData, 0, inNumberFrames*filter->frameSize());
        }
        else
        {
            filter->readFrames(inNumberFrames, ioData->mBuffers[kLeftChannel].mData);
        }
	    
        return noErr;
    }

    IOS3DMixerRenderFilter::IOS3DMixerRenderFilter(AudioUnitAccessor& accessor, Mutex& mutex)
    : accessor_(accessor)
    , mutex_(mutex)
    {
    }

    void IOS3DMixerRenderFilter::setMuted(const bool muted)
    {
        muted_ = muted;
    }

    void IOS3DMixerRenderFilter::setEnabled(const bool enabled)
    {
        AURenderCallbackStruct renderCallbackStruct;
        if(enabled)
        {
            renderCallbackStruct.inputProc = &IOS3DMixerRenderSink_renderCallback;
            renderCallbackStruct.inputProcRefCon = this;
        }
        else
        {
            renderCallbackStruct.inputProc = NULL;
            renderCallbackStruct.inputProcRefCon = NULL;
        }

        // Nested scope to control mutex lifetime.
        {
            OPTIONAL_LOCK(mutex_);
            accessor_.setRenderCallback(renderCallbackStruct);
            enabled_ = enabled;
        }
    }

    void IOS3DMixerRenderFilter::readFrames(const unsigned long numFrames, void*const dst)
    {
        sourceReadFrames(numFrames, dst);
    }
}
