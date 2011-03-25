//
//  AUCAudioContext.m
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

#import "AUCAudioContext.h"
#import "AUCMacros.h"
#import "AUCLogging.h"
#import "AUCAudioSource.h"

#define kDefaultMaxFramesPerSlice 4096


namespace aural {


AudioContext::AudioContext(AudioManager* manager)
: manager_(manager)
{
	memset(sources_, 0, sizeof(sources_));

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
//	nodeMixerDesc.componentSubType       = kAudioUnitSubType_MultiChannelMixer;
	nodeMixerDesc.componentSubType       = kAudioUnitSubType_AU3DMixerEmbedded;
	nodeMixerDesc.componentManufacturer  = kAudioUnitManufacturer_Apple;
	nodeMixerDesc.componentFlags         = 0;
	nodeMixerDesc.componentFlagsMask     = 0;
	
	// Create a new graph.
	if(noErr != (error = NewAUGraph(&auGraph_)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not create audio processing graph");
		goto done;
	}
	
	// Add the I/O node.
	if(noErr != (error = AUGraphAddNode(auGraph_, 
										&nodeIoDesc,
										&nodeIo)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not create I/O Node");
		goto done;
	}
	
	// Add the mixer node.
	if(noErr != (error = AUGraphAddNode(auGraph_,
										&nodeMixerDesc,
										&nodeMixer)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not create Mixer Node");
		goto done;
	}
	
	// Open the graph.
	if(noErr != (error = AUGraphOpen(auGraph_)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not open AUGraph");
		goto done;
	}
	
	// Get a handle to the mixer unit.
	if(noErr != (error = AUGraphNodeInfo(auGraph_,
										 nodeMixer,
										 NULL,
										 &mixerUnit_)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not obtain handle to mixer unit");
		goto done;
	}
	
	// Connect mixer output element 0 to I/O input element 0. 
	if(noErr != (error = AUGraphConnectNodeInput(auGraph_,
												 nodeMixer,
												 0,
												 nodeIo,
												 0)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not connect mixer node to I/O node");
		goto done;
	}
	
	printf("Graph state before init:");
	CAShow(auGraph_);
	
	setMaxFramesPerSlice(kDefaultMaxFramesPerSlice);
	
	
	setNumElements(kMaxSourcesPerContext);
	
	if(noErr != (error = AUGraphInitialize(auGraph_)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not initialize the audio processing graph");
		goto done;
	}
	
	printf("Graph state after init:");
	CAShow(auGraph_);
	
	setActive(TRUE);
	
done:
	if(noErr != error)
	{
		// TODO: exception?
	}
}


AudioContext::~AudioContext()
{
}

void AudioContext::setNumElements(UInt32 numElements)
{
	OSStatus error;

	{
		OPTIONAL_LOCK(mutex_);
		error = AudioUnitSetProperty(mixerUnit_,
									 kAudioUnitProperty_ElementCount,
									 kAudioUnitScope_Input,
									 0,
									 &numElements,
									 sizeof(numElements));
	}
		
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not open %d mixer elements", numElements);
	}
}

Float64 AudioContext::sampleRate()
{
	Float64 value = 0;
	UInt32 size = sizeof(value);
	OSStatus error;

	{
		OPTIONAL_LOCK(mutex_);
		error = AudioUnitGetProperty(mixerUnit_,
									 kAudioUnitProperty_SampleRate,
									 kAudioUnitScope_Output,
									 0,
									 &value,
									 &size);
	}

	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not get mixer sample rate");
	}
	
	return value;
}

void AudioContext::setSampleRate(Float64 sampleRate)
{
	OSStatus error;
	
	{
		OPTIONAL_LOCK(mutex_);
		error = AudioUnitSetProperty(mixerUnit_,
									 kAudioUnitProperty_SampleRate,
									 kAudioUnitScope_Output,
									 0,
									 &sampleRate,
									 sizeof(sampleRate));
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set the mixer sample rate to %f", sampleRate);
	}
}

UInt32 AudioContext::maxFramesPerSlice()
{
	UInt32 value = 0;
	UInt32 size = sizeof(value);
	OSStatus error;
	
	{
		OPTIONAL_LOCK(mutex_);
		error = AudioUnitGetProperty(mixerUnit_,
									 kAudioUnitProperty_SampleRate,
									 kAudioUnitScope_Output,
									 0,
									 &value,
									 &size);
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not get frames per slice");
	}
	
	return value;
}

void AudioContext::setMaxFramesPerSlice(UInt32 value)
{
	OSStatus error;
	
	{
		OPTIONAL_LOCK(mutex_);
		error = AudioUnitSetProperty(mixerUnit_,
									 kAudioUnitProperty_MaximumFramesPerSlice,
									 kAudioUnitScope_Global,
									 0,
									 &value,
									 sizeof(value));
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set max frames per slice to %d", value);
	}
}

void AudioContext::setActive(bool value)
{
	OSStatus error = noErr;
	
	Boolean isActive = FALSE;
	
	{
		OPTIONAL_LOCK(mutex_);
		error = AUGraphIsRunning(auGraph_, &isActive);
		
		if(noErr != error)
		{
			REPORT_AUGRAPH_ERROR(error, "Could not get processing graph running status");
			return;
		}
		
		active_ = isActive;
		
		if(value != isActive)
		{
			if(value)
			{
				if(noErr != (error = AUGraphStart(auGraph_)))
				{
					REPORT_AUGRAPH_ERROR(error, "Could not start the processing graph");
					return;
				}
			}
			else
			{
				if(noErr != (error = AUGraphStop(auGraph_)))
				{
					REPORT_AUGRAPH_ERROR(error, "Could not stop the processing graph");
					return;
				}
			}
			active_ = value;
		}
	}
}

UInt32 AudioContext::notifySourceInit(AudioSource& source)
{
	for(unsigned int i = 0; i < kMaxSourcesPerContext; i++)
	{
		if(NULL == sources_[i])
		{
			sources_[i] = &source;
			return i;
		}
	}
	return INVALID_ELEMENT;
}

void AudioContext::notifySourceDestroy(AudioSource& source)
{
	if(INVALID_ELEMENT != source.elementNumber())
	{
		sources_[source.elementNumber()] = NULL;
	}
}

} // namespace
