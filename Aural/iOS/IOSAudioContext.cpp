//
//  IOSAudioContext.cpp
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

#include "IOSAudioContext.h"
#include "Macros.h"
#include "IOSAudioManager.h"
#include "IOSAudioSource.h"


namespace aural
{
    IOS3DMixerAudioContext::IOS3DMixerAudioContext(IOSAudioManager*const manager,
                                                   const unsigned int maxSources)
    : manager_(manager)
    , maxSources_(maxSources)
    , graph_(AudioUnitGraph(kAudioUnitSubType_AU3DMixerEmbedded, maxSources))
    , sources_(new IOSAudioSource*[maxSources])
    , inputAccessor_(graph_.mixerUnit(), kAudioUnitScope_Input, 0)
    , outputAccessor_(graph_.mixerUnit(), kAudioUnitScope_Output, 0)
    , globalAccessor_(graph_.mixerUnit(), kAudioUnitScope_Global, 0)
    {
        memset(sources_, 0, sizeof(sources_) * maxSources);
        setActive(TRUE);
    }
    
    
    IOS3DMixerAudioContext::~IOS3DMixerAudioContext()
    {
        for(UInt32 element = 0; element < maxSources_; element++)
        {
            if(NULL != sources_[element])
            {
                delete sources_[element];
            }
        }
        delete [] sources_;
    }

    AudioSource* IOS3DMixerAudioContext::newSource()
    {
        IOSAudioSource* source = NULL;
        
        // Nested scope to control mutex lifetime.
        {
            OPTIONAL_LOCK(mutex_);
            
            for(UInt32 element = 0; element < maxSources_; element++)
            {
                if(NULL == sources_[element])
                {
                    sources_[element] = source = new IOSAudioSource(*this, element);
                    break;
                }
            }
        }
        
        if(NULL == source)
        {
            AURAL_LOG_ERROR("Cannot allocate any more sources on this context");
        }

        return source;
    }

    void IOS3DMixerAudioContext::deleteSource(AudioSource*const audioSource)
    {
        IOSAudioSource* realSource = static_cast<IOSAudioSource*>(audioSource);
        OPTIONAL_LOCK(mutex_);
        sources_[realSource->elementNumber()] = NULL;
        delete audioSource;
    }

    void IOS3DMixerAudioContext::setNumElements(const UInt32 value)
    {
        OPTIONAL_LOCK(mutex_);
        inputAccessor_.setUInt32Property(kAudioUnitProperty_ElementCount, value);
    }
    
    Float64 IOS3DMixerAudioContext::sampleRate()
    {
        OPTIONAL_LOCK(mutex_);
        return outputAccessor_.getFloat64Property(kAudioUnitProperty_SampleRate);
    }
    
    void IOS3DMixerAudioContext::setSampleRate(const Float64 value)
    {
        OPTIONAL_LOCK(mutex_);
        outputAccessor_.setFloat64Property(kAudioUnitProperty_SampleRate, value);
    }
    
    UInt32 IOS3DMixerAudioContext::maxFramesPerSlice()
    {
        OPTIONAL_LOCK(mutex_);
        return globalAccessor_.getUInt32Property(kAudioUnitProperty_MaximumFramesPerSlice);
    }
    
    void IOS3DMixerAudioContext::setMaxFramesPerSlice(const UInt32 value)
    {
        OPTIONAL_LOCK(mutex_);
        globalAccessor_.setUInt32Property(kAudioUnitProperty_MaximumFramesPerSlice, value);
    }
    
    inline bool IOS3DMixerAudioContext::active()
    {
        return graph_.active();
    }
    
    void IOS3DMixerAudioContext::setActive(const bool value)
    {
        graph_.setActive(value);
    }
}
