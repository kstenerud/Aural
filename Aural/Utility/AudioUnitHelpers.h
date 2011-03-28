//
//  AudioUnitHelpers.h
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

#ifndef AURAL_AUDIOUNITHELPERS_H
#define AURAL_AUDIOUNITHELPERS_H


#include <AudioToolbox/AudioToolbox.h>
#include <stdexcept>
#include "Logging.h"
#include "Macros.h"
#include "Information.h"


// Enables trace logging in this file. Trace logs go to the debug channel.
#define TRACE_LOGGING_ENABLED 0

#if TRACE_LOGGING_ENABLED
    #define LOG_TRACE(FMT, ...) AURAL_LOG_DEBUG(FMT, ##__VA_ARGS__)
#else
    #define LOG_TRACE(FMT, ...)
#endif


namespace aural
{
    class AudioUnitGraph
    {
    public:
        AudioUnitGraph(OSType mixerSubtype,
                       unsigned long numElements);
        
        bool active();
        void setActive(bool value);
        
        AUGraph auGraph();
        AudioUnit mixerUnit();
        
    private:
        AUGraph auGraph_;
        AudioUnit mixerUnit_;
    };
    
    class AudioUnitAccessor
    {
    public:
        AudioUnitAccessor(AudioUnit audioUnit_,
                          AudioUnitScope audioUnitScope_,
                          AudioUnitElement element_);

        AudioUnitParameterValue getParameter(const AudioUnitParameterID parameter);
        void setParameter(const AudioUnitParameterID parameter, const AudioUnitParameterValue value);

        UInt32 getUInt32Property(const AudioUnitPropertyID property);
        Float32 getFloat32Property(const AudioUnitPropertyID property);
        Float64 getFloat64Property(const AudioUnitPropertyID property);

        void setUInt32Property(const AudioUnitPropertyID property, const UInt32 value);
        void setFloat32Property(const AudioUnitPropertyID property, const Float32 value);
        void setFloat64Property(const AudioUnitPropertyID property, const Float64 value);

        void setRenderCallback(const AURenderCallbackStruct& renderCallbackStruct);
        void setStreamFormat(const AudioStreamBasicDescription& format);

        AudioUnit audioUnit();
        AudioUnitScope audioUnitScope();
        AudioUnitElement element();

    private:
        AudioUnit audioUnit_;
        AudioUnitScope audioUnitScope_;
        AudioUnitElement element_;
    };
    
    
    
    
    
    inline AUGraph AudioUnitGraph::auGraph()
    {
        return auGraph_;
    }
    
    inline AudioUnit AudioUnitGraph::mixerUnit()
    {
        return mixerUnit_;
    }
    
    
    inline AudioUnitAccessor::AudioUnitAccessor(AudioUnit audioUnit,
                                                AudioUnitScope audioUnitScope,
                                                AudioUnitElement element)
    : audioUnit_(audioUnit)
    , audioUnitScope_(audioUnitScope)
    , element_(element)
    {
    }
    
    inline AudioUnit AudioUnitAccessor::audioUnit()
    {
        return audioUnit_;
    }
    
    inline AudioUnitScope AudioUnitAccessor::audioUnitScope()
    {
        return audioUnitScope_;
    }
    
    inline AudioUnitElement AudioUnitAccessor::element()
    {
        return element_;
    }
    
    inline AudioUnitParameterValue AudioUnitAccessor::getParameter(const AudioUnitParameterID parameter)
    {
        AudioUnitParameterValue value;
        OSStatus error;
        if(noErr != (error = AudioUnitGetParameter(audioUnit_,
                                                   parameter,
                                                   audioUnitScope_,
                                                   element_,
                                                   &value)))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not get parameter %s, %s scope, element %d on unit %08x",
                                   AudioUnitInfo::parameterName(parameter), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_);
            OPTIONAL_THROW(std::invalid_argument("Could not get parameter"));
        }
        
        LOG_TRACE("Get parameter %s, %s scope, element %d on unit %08x: %f",
                  AudioUnitInfo::parameterName(parameter), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
        return value;
    }
    
    inline void AudioUnitAccessor::setParameter(const AudioUnitParameterID parameter, const AudioUnitParameterValue value)
    {
        OSStatus error;
        
        if(noErr != (error = AudioUnitSetParameter(audioUnit_,
                                                   parameter,
                                                   audioUnitScope_,
                                                   element_,
                                                   value,
                                                   0)))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not set parameter %s, %s scope, element %d on unit %08x to %f",
                                   AudioUnitInfo::parameterName(parameter), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_, value);
            OPTIONAL_THROW(std::invalid_argument("Could not set parameter"));
        }
        
        LOG_TRACE("Set parameter %s, %s scope, element %d on unit %08x to %f",
                  AudioUnitInfo::parameterName(parameter), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
    }
    
    inline UInt32 AudioUnitAccessor::getUInt32Property(const AudioUnitPropertyID property)
    {
        UInt32 value;
        UInt32 size = sizeof(value);
        OSStatus error;
        if(noErr != (error = AudioUnitGetProperty(audioUnit_,
                                                  property,
                                                  audioUnitScope_,
                                                  element_,
                                                  &value,
                                                  &size)))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not get UInt32 property %s, %s scope, element %d on unit %08x",
                                   AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_);
            OPTIONAL_THROW(std::invalid_argument("Could not get UInt32 property"));
        }
        
        LOG_TRACE("Get UInt32 property %s, %s scope, element %d on unit %08x: %d",
                  AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
        return value;
    }
    
    inline Float32 AudioUnitAccessor::getFloat32Property(const AudioUnitPropertyID property)
    {
        Float32 value;
        UInt32 size = sizeof(value);
        OSStatus error;
        if(noErr != (error = AudioUnitGetProperty(audioUnit_,
                                                  property,
                                                  audioUnitScope_,
                                                  element_,
                                                  &value,
                                                  &size)))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not get Float32 property %s, %s scope, element %d on unit %08x",
                                   AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_);
            OPTIONAL_THROW(std::invalid_argument("Could not get Float32 property"));
        }
        
        LOG_TRACE("Get Float32 property %s, %s scope, element %d on unit %08x: %f",
                  AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
        return value;
    }
    
    inline Float64 AudioUnitAccessor::getFloat64Property(const AudioUnitPropertyID property)
    {
        Float64 value;
        UInt32 size = sizeof(value);
        OSStatus error;
        if(noErr != (error = AudioUnitGetProperty(audioUnit_,
                                                  property,
                                                  audioUnitScope_,
                                                  element_,
                                                  &value,
                                                  &size)))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not get Float64 property %s, %s scope, element %d on unit %08x",
                                   AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_);
            OPTIONAL_THROW(std::invalid_argument("Could not get Float64 property"));
        }
        
        LOG_TRACE("Get Float64 property %s, %s scope, element %d on unit %08x: %f",
                  AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
        return value;
    }
    
    
    inline void AudioUnitAccessor::setUInt32Property(const AudioUnitPropertyID property,
                                                     const UInt32 value)
    {
        OSStatus error;
        if(noErr != (error = AudioUnitSetProperty(audioUnit_,
                                                  property,
                                                  audioUnitScope_,
                                                  element_,
                                                  &value,
                                                  sizeof(value))))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not set UInt32 property %s, %s scope, element %d on unit %08x to %d",
                                   AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_, value);
            OPTIONAL_THROW(std::invalid_argument("Could not set UInt32 property"));
        }
        
        LOG_TRACE("Set UInt32 property %s, %s scope, element %d on unit %08x to %d",
                  AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
    }
    
    inline void AudioUnitAccessor::setFloat32Property(const AudioUnitPropertyID property,
                                                      const Float32 value)
    {
        OSStatus error;
        if(noErr != (error = AudioUnitSetProperty(audioUnit_,
                                                  property,
                                                  audioUnitScope_,
                                                  element_,
                                                  &value,
                                                  sizeof(value))))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not set Float32 property %s, %s scope, element %d on unit %08x to %f",
                                   AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_, value);
            OPTIONAL_THROW(std::invalid_argument("Could not set Float32 property"));
        }
        
        LOG_TRACE("Set Float32 property %s, %s scope, element %d on unit %08x to %f",
                  AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
    }
    
    inline void AudioUnitAccessor::setFloat64Property(const AudioUnitPropertyID property,
                                                      const Float64 value)
    {
        OSStatus error;
        if(noErr != (error = AudioUnitSetProperty(audioUnit_,
                                                  property,
                                                  audioUnitScope_,
                                                  element_,
                                                  &value,
                                                  sizeof(value))))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not set Float64 property %s, %s scope, element %d on unit %08x to %f",
                                   AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                                   element_, audioUnit_, value);
            OPTIONAL_THROW(std::invalid_argument("Could not set Float64 property"));
        }
        
        LOG_TRACE("Set Float64 property %s, %s scope, element %d on unit %08x to %f",
                  AudioUnitInfo::propertyName(property), AudioUnitInfo::scopeName(audioUnitScope_),
                  element_, audioUnit_, value);
    }
    
    inline void AudioUnitAccessor::setRenderCallback(const AURenderCallbackStruct& renderCallbackStruct)
    {
        OSStatus error;
        if(noErr != (error = AudioUnitSetProperty(audioUnit_,
                                                  kAudioUnitProperty_SetRenderCallback,
                                                  kAudioUnitScope_Input,
                                                  element_,
                                                  &renderCallbackStruct,
                                                  sizeof(renderCallbackStruct))))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not set render callback for element %d on unit %08x",
                                   element_, audioUnit_);
            OPTIONAL_THROW(std::invalid_argument("Could not set render callback"));
        }
        
        LOG_TRACE("Set render callback for element %d on unit %08x",
                  element_, audioUnit_);
    }
    
    inline void AudioUnitAccessor::setStreamFormat(const AudioStreamBasicDescription& format)
    {
        OSStatus error;
        if(noErr != (error = AudioUnitSetProperty(audioUnit_,
                                                  kAudioUnitProperty_StreamFormat,
                                                  kAudioUnitScope_Input,
                                                  element_,
                                                  &format,
                                                  sizeof(format))))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not set stream format for element %d on unit %08x",
                                   element_, audioUnit_);
            OPTIONAL_THROW(std::invalid_argument("Could not set stream format"));
        }
        
        LOG_TRACE("Set stream format for element %d on unit %08x",
                  element_, audioUnit_);
    }
}

#undef TRACE_LOGGING_ENABLED
#undef LOG_TRACE

#endif // AURAL_AUDIOUNITHELPERS_H
