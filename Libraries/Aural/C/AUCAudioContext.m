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



#pragma mark -
#pragma mark Forward Declarations

static void self_destroy(AUCAudioContext* self);
static AUCAudioSource* self_createSource(AUCAudioContext* self);

static void self_setNumElements(AUCAudioContext* self, UInt32 numElements);

static Float64 self_getSampleRate(AUCAudioContext* self);
static void self_setSampleRate(AUCAudioContext* self, Float64 sampleRate);

static UInt32 self_getMaxFramesPerSlice(AUCAudioContext* self);
static void self_setMaxFramesPerSlice(AUCAudioContext* self, UInt32 maxFramesPerSlice);

static BOOL self_getActive(AUCAudioContext* self);
static void self_setActive(AUCAudioContext* self, BOOL active);

static unsigned int self_notifySourceInit(AUCAudioContext* self, AUCAudioSource* source);
static void self_notifySourceDestroy(AUCAudioContext* self, AUCAudioSource* source);



#pragma mark -
#pragma mark Methods

AUCAudioContext* AUCAudioContext_create(AUCAudioManager* manager)
{
	// Creation
	AUCAudioContext* self = ALLOC_OBJECT(AUCAudioContext);
	if(NULL == self)
	{
		return NULL;
	}
	
	// Method Assignment
	self->destroy = self_destroy;
	self->createSource = self_createSource;
	self->setNumElements = self_setNumElements;
	self->getSampleRate = self_getSampleRate;
	self->setSampleRate = self_setSampleRate;
	self->getMaxFramesPerSlice = self_getMaxFramesPerSlice;
	self->setMaxFramesPerSlice = self_setMaxFramesPerSlice;
	self->getActive = self_getActive;
	self->setActive = self_setActive;
	self->notifySourceInit = self_notifySourceInit;
	self->notifySourceDestroy = self_notifySourceDestroy;
	
	// Other Initialization
	AUC_MUTEX_INIT(&self->mutex);

	self->manager = manager;

	
	
	OSStatus error;
	AUNode nodeIo;
	AUNode nodeMixer;
	
	// Node descriptions.
	AudioComponentDescription nodeIoDesc =
	{
		.componentType          = kAudioUnitType_Output,
		.componentSubType       = kAudioUnitSubType_RemoteIO,
		.componentManufacturer  = kAudioUnitManufacturer_Apple,
		.componentFlags         = 0,
		.componentFlagsMask     = 0,
	};
	
	AudioComponentDescription nodeMixerDesc =
	{
		.componentType          = kAudioUnitType_Mixer,
//		.componentSubType       = kAudioUnitSubType_MultiChannelMixer,
		.componentSubType       = kAudioUnitSubType_AU3DMixerEmbedded,
		.componentManufacturer  = kAudioUnitManufacturer_Apple,
		.componentFlags         = 0,
		.componentFlagsMask     = 0,
	};
	
	// Create a new graph.
	if(noErr != (error = NewAUGraph(&self->auGraph)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not create audio processing graph");
		goto done;
	}
	
	// Add the I/O node.
	if(noErr != (error = AUGraphAddNode(self->auGraph, 
										&nodeIoDesc,
										&nodeIo)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not create I/O Node");
		goto done;
	}
	
	// Add the mixer node.
	if(noErr != (error = AUGraphAddNode(self->auGraph,
										&nodeMixerDesc,
										&nodeMixer)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not create Mixer Node");
		goto done;
	}
	
	// Open the graph.
	if(noErr != (error = AUGraphOpen(self->auGraph)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not open AUGraph");
		goto done;
	}
	
	// Get a handle to the mixer unit.
	if(noErr != (error = AUGraphNodeInfo(self->auGraph,
										 nodeMixer,
										 NULL,
										 &self->mixerUnit)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not obtain handle to mixer unit");
		goto done;
	}
	
	// Connect mixer output element 0 to I/O input element 0. 
	if(noErr != (error = AUGraphConnectNodeInput(self->auGraph,
												 nodeMixer,
												 0,
												 nodeIo,
												 0)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not connect mixer node to I/O node");
		goto done;
	}
	
	NSLog(@"Graph state before init:");
	CAShow(self->auGraph);
	
	self_setMaxFramesPerSlice(self, kDefaultMaxFramesPerSlice);
	
	
	self_setNumElements(self, kMaxSourcesPerContext);
	
	if(noErr != (error = AUGraphInitialize(self->auGraph)))
	{
		REPORT_AUGRAPH_ERROR(error, "Could not initialize the audio processing graph");
		goto done;
	}
	
	NSLog(@"Graph state after init:");
	CAShow(self->auGraph);
	
	self_setActive(self, TRUE);
	
done:
	if(noErr != error)
	{
		self_destroy(self);
		self = nil;
	}
	return self;
}

static void self_destroy(AUCAudioContext* self)
{
	AUC_MUTEX_DESTROY(&self->mutex);

	FREE_OBJECT(self);
}

static AUCAudioSource* self_createSource(AUCAudioContext* self)
{
	return AUCAudioSource_create(self);
}

static void self_setNumElements(AUCAudioContext* self, UInt32 value)
{
	OSStatus error;

	OPT_LOCK();
	error = AudioUnitSetProperty(self->mixerUnit,
								 kAudioUnitProperty_ElementCount,
								 kAudioUnitScope_Input,
								 0,
								 &value,
								 sizeof(value));
	OPT_UNLOCK();

	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not open %d mixer elements", value);
	}
}

static Float64 self_getSampleRate(AUCAudioContext* self)
{
	Float64 value = 0;
	UInt32 size = sizeof(value);
	OSStatus error;

	OPT_LOCK();
	error = AudioUnitGetProperty(self->mixerUnit,
								 kAudioUnitProperty_SampleRate,
								 kAudioUnitScope_Output,
								 0,
								 &value,
								 &size);
	OPT_UNLOCK();

	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not get mixer sample rate");
	}
	
	return value;
}

static void self_setSampleRate(AUCAudioContext* self, Float64 value)
{
	OSStatus error;
	
	OPT_LOCK();
	error = AudioUnitSetProperty(self->mixerUnit,
								 kAudioUnitProperty_SampleRate,
								 kAudioUnitScope_Output,
								 0,
								 &value,
								 sizeof(value));
	OPT_UNLOCK();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set the mixer sample rate to %f", value);
	}
}

static UInt32 self_getMaxFramesPerSlice(AUCAudioContext* self)
{
	UInt32 value = 0;
	UInt32 size = sizeof(value);
	OSStatus error;
	
	OPT_LOCK();
	error = AudioUnitGetProperty(self->mixerUnit,
								 kAudioUnitProperty_SampleRate,
								 kAudioUnitScope_Output,
								 0,
								 &value,
								 &size);
	OPT_UNLOCK();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not get frames per slice");
	}
	
	return value;
}

static void self_setMaxFramesPerSlice(AUCAudioContext* self, UInt32 value)
{
	OSStatus error;
	
	OPT_LOCK();
	error = AudioUnitSetProperty(self->mixerUnit,
								 kAudioUnitProperty_MaximumFramesPerSlice,
								 kAudioUnitScope_Global,
								 0,
								 &value,
								 sizeof(value));
	OPT_UNLOCK();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set max frames per slice to %d", value);
	}
}

static BOOL self_getActive(AUCAudioContext* self)
{
	return self->active;
}

static void self_setActive(AUCAudioContext* self, BOOL value)
{
	OSStatus error = noErr;
	
	Boolean isActive = FALSE;
	
	OPT_LOCK();
	error = AUGraphIsRunning(self->auGraph, &isActive);
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not get processing graph running status");
		OPT_UNLOCK_AND_RETURN();
	}
	
	self->active = isActive;
	
	if(value != isActive)
	{
		if(value)
		{
			NSLog(@"Starting graph");
			if(noErr != (error = AUGraphStart(self->auGraph)))
			{
				REPORT_AUGRAPH_ERROR(error, "Could not start the processing graph");
				OPT_UNLOCK_AND_RETURN();
			}
		}
		else
		{
			if(noErr != (error = AUGraphStop(self->auGraph)))
			{
				REPORT_AUGRAPH_ERROR(error, "Could not stop the processing graph");
				OPT_UNLOCK_AND_RETURN();
			}
		}
		self->active = value;
	}
	OPT_UNLOCK();
}

static unsigned int self_notifySourceInit(AUCAudioContext* self, AUCAudioSource* source)
{
	for(unsigned int i = 0; i < kMaxSourcesPerContext; i++)
	{
		if(NULL == self->sources[i])
		{
			self->sources[i] = source;
			return i;
		}
	}
	return INVALID_ELEMENT;
}

static void self_notifySourceDestroy(AUCAudioContext* self, AUCAudioSource* source)
{
	if(INVALID_ELEMENT != source->elementNumber)
	{
		self->sources[source->elementNumber] = NULL;
	}
}
