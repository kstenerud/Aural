//
//  AudioUnitAccess.cpp
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

#include "AudioUnitHelpers.h"
#include "Logging.h"
#include "Macros.h"
#include <stdexcept>


// Enables trace logging in this file. Trace logs go to the debug channel.
#define TRACE_LOGGING_ENABLED 1

#if TRACE_LOGGING_ENABLED
    #define LOG_TRACE(FMT, ...) AURAL_LOG_DEBUG(FMT, ##__VA_ARGS__)
    #define LOG_TRACE_GRAPH(GRAPH) CAShow(GRAPH)
#else
    #define LOG_TRACE(FMT, ...)
    #define LOG_TRACE_GRAPH(GRAPH)
#endif


namespace aural
{
    inline static void reportAndThrow(const OSStatus error, const char*const message)
    {
        REPORT_AUDIOUNIT_ERROR(error, message);
        OPTIONAL_THROW(std::runtime_error(message));
    }
    
    
    AudioUnitGraph::AudioUnitGraph(const OSType mixerSubtype,
                                   const unsigned long numElements)
    {
        OSStatus error;
        AUNode nodeIo;
        AUNode nodeMixer;
        
        // Node descriptions.
        AudioComponentDescription nodeIoDesc;
        nodeIoDesc.componentType          = kAudioUnitType_Output;
        nodeIoDesc.componentSubType       = kAudioUnitSubType_RemoteIO;
        nodeIoDesc.componentManufacturer  = kAudioUnitManufacturer_Apple;
        nodeIoDesc.componentFlags         = 0;
        nodeIoDesc.componentFlagsMask     = 0;
        
        AudioComponentDescription nodeMixerDesc;
        nodeMixerDesc.componentType          = kAudioUnitType_Mixer;
        nodeMixerDesc.componentSubType       = mixerSubtype;
        nodeMixerDesc.componentManufacturer  = kAudioUnitManufacturer_Apple;
        nodeMixerDesc.componentFlags         = 0;
        nodeMixerDesc.componentFlagsMask     = 0;
        
        // Create a new graph.
        if(noErr != (error = NewAUGraph(&auGraph_)))
        {
            reportAndThrow(error, "Could not create audio processing graph");
        }
        
        // Add the I/O node.
        if(noErr != (error = AUGraphAddNode(auGraph_, 
                                            &nodeIoDesc,
                                            &nodeIo)))
        {
            reportAndThrow(error, "Could not create I/O Node");
        }
        
        // Add the mixer node.
        if(noErr != (error = AUGraphAddNode(auGraph_,
                                            &nodeMixerDesc,
                                            &nodeMixer)))
        {
            reportAndThrow(error, "Could not create Mixer Node");
        }
        
        // Open the graph.
        if(noErr != (error = AUGraphOpen(auGraph_)))
        {
            reportAndThrow(error, "Could not open AUGraph");
        }
        
        // Get a handle to the mixer unit.
        if(noErr != (error = AUGraphNodeInfo(auGraph_,
                                             nodeMixer,
                                             NULL,
                                             &mixerUnit_)))
        {
            reportAndThrow(error, "Could not obtain handle to mixer unit");
        }
        
        // Connect mixer output element 0 to I/O input element 0. 
        if(noErr != (error = AUGraphConnectNodeInput(auGraph_,
                                                     nodeMixer,
                                                     0,
                                                     nodeIo,
                                                     0)))
        {
            reportAndThrow(error, "Could not connect mixer node to I/O node");
        }
        
        if(numElements > 0)
        {
            AudioUnitAccessor accessor = AudioUnitAccessor(mixerUnit_,
                                                           kAudioUnitScope_Input,
                                                           0);
            
            accessor.setUInt32Property(kAudioUnitProperty_ElementCount, numElements);
        }
        
        if(noErr != (error = AUGraphInitialize(auGraph_)))
        {
            reportAndThrow(error, "Could not initialize the audio processing graph");
        }
        
        LOG_TRACE_GRAPH(auGraph_);
    }

    bool AudioUnitGraph::active()
    {
        Boolean isActive;
        OSStatus error;
        
        if(noErr != (error = AUGraphIsRunning(auGraph_, &isActive)))
        {
            REPORT_AUDIOUNIT_ERROR(error, "Could not get processing graph running status");
            OPTIONAL_THROW(std::runtime_error("Could not get processing graph running status"));
        }
        return isActive;
    }
    
    void AudioUnitGraph::setActive(const bool value)
    {
        OSStatus error;
        
        if(value)
        {
            if(noErr != (error = AUGraphStart(auGraph_)))
            {
                REPORT_AUDIOUNIT_ERROR(error, "Could not start the processing graph");
                OPTIONAL_THROW(std::runtime_error("Could not start the processing graph"));
            }
        }
        else
        {
            if(noErr != (error = AUGraphStop(auGraph_)))
            {
                REPORT_AUDIOUNIT_ERROR(error, "Could not stop the processing graph");
                OPTIONAL_THROW(std::runtime_error("Could not stop the processing graph"));
            }
        }
    }
}
