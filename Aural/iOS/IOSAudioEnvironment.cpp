//
//  IOSAudioEnvironment.cpp
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

#include "IOSAudioEnvironment.h"
#include "Macros.h"
#include "IOSAudioManager.h"
#include "IOSAudioEmitter.h"


namespace aural
{
    IOS3DMixerAudioEnvironment::IOS3DMixerAudioEnvironment(IOSAudioManager*const manager,
                                                           const unsigned int maxEmitters)
    : manager_(manager)
    , maxEmitters_(maxEmitters)
    , graph_(AudioUnitGraph(kAudioUnitSubType_AU3DMixerEmbedded, maxEmitters))
    , emitters_(new IOSAudioEmitter*[maxEmitters])
    , inputAccessor_(graph_.mixerUnit(), kAudioUnitScope_Input, 0)
    , outputAccessor_(graph_.mixerUnit(), kAudioUnitScope_Output, 0)
    , globalAccessor_(graph_.mixerUnit(), kAudioUnitScope_Global, 0)
    {
        memset(emitters_, 0, sizeof(emitters_) * maxEmitters);
        setActive(TRUE);
    }
    
    
    IOS3DMixerAudioEnvironment::~IOS3DMixerAudioEnvironment()
    {
        for(UInt32 element = 0; element < maxEmitters_; element++)
        {
            if(NULL != emitters_[element])
            {
                delete emitters_[element];
            }
        }
        delete [] emitters_;
    }

    AudioEmitter* IOS3DMixerAudioEnvironment::newEmitter()
    {
        IOSAudioEmitter* emitter = NULL;
        
        // Nested scope to control mutex lifetime.
        {
            OPTIONAL_LOCK(mutex_);
            
            for(UInt32 element = 0; element < maxEmitters_; element++)
            {
                if(NULL == emitters_[element])
                {
                    emitters_[element] = emitter = new IOSAudioEmitter(*this, element);
                    break;
                }
            }
        }
        
        if(NULL == emitter)
        {
            AURAL_LOG_ERROR("Cannot allocate any more emitters in this environment");
        }

        return emitter;
    }

    void IOS3DMixerAudioEnvironment::deleteEmitter(AudioEmitter*const audioEmitter)
    {
        IOSAudioEmitter* realEmitter = static_cast<IOSAudioEmitter*>(audioEmitter);
        OPTIONAL_LOCK(mutex_);
        emitters_[realEmitter->elementNumber()] = NULL;
        delete audioEmitter;
    }

    void IOS3DMixerAudioEnvironment::setNumElements(const UInt32 value)
    {
        OPTIONAL_LOCK(mutex_);
        inputAccessor_.setUInt32Property(kAudioUnitProperty_ElementCount, value);
    }
    
    Float64 IOS3DMixerAudioEnvironment::sampleRate()
    {
        OPTIONAL_LOCK(mutex_);
        return outputAccessor_.getFloat64Property(kAudioUnitProperty_SampleRate);
    }
    
    void IOS3DMixerAudioEnvironment::setSampleRate(const Float64 value)
    {
        OPTIONAL_LOCK(mutex_);
        outputAccessor_.setFloat64Property(kAudioUnitProperty_SampleRate, value);
    }
    
    UInt32 IOS3DMixerAudioEnvironment::maxFramesPerSlice()
    {
        OPTIONAL_LOCK(mutex_);
        return globalAccessor_.getUInt32Property(kAudioUnitProperty_MaximumFramesPerSlice);
    }
    
    void IOS3DMixerAudioEnvironment::setMaxFramesPerSlice(const UInt32 value)
    {
        OPTIONAL_LOCK(mutex_);
        globalAccessor_.setUInt32Property(kAudioUnitProperty_MaximumFramesPerSlice, value);
    }
    
    inline bool IOS3DMixerAudioEnvironment::active()
    {
        return graph_.active();
    }
    
    void IOS3DMixerAudioEnvironment::setActive(const bool value)
    {
        graph_.setActive(value);
    }
}
