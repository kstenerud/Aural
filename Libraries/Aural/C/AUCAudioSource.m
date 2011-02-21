//
//  AUCAudioSource.m
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

#import "AUCAudioSource.h"
#import <AudioToolbox/AudioToolbox.h>
#import "AUCMacros.h"
#import "AUCLogging.h"
#import "AUCAudioBuffer.h"
#import "AUCAudioContext.h"


#define OPT_LOCK_MIXER() OPTIONAL_LOCK(&self->context->mutex)
#define OPT_UNLOCK_MIXER() OPTIONAL_UNLOCK(&self->context->mutex)


#pragma mark -
#pragma mark Forward Declarations

static OSStatus renderCallback(void* inRefCon,
							   AudioUnitRenderActionFlags* ioActionFlags,
							   const AudioTimeStamp* inTimeStamp,
							   UInt32 inElementNumber,
							   UInt32 inNumberFrames,
							   AudioBufferList* ioData);

static void self_destroy(AUCAudioSource* self);

static inline OSStatus self_setFloat32Property(AUCAudioSource* self,
											   UInt32 property,
											   Float32 value);

static void self_enableRenderProc(AUCAudioSource* self);
static void self_disableRenderProc(AUCAudioSource* self);

static void self_play(AUCAudioSource* self);
static void self_stop(AUCAudioSource* self);

DECLARE_GETTER_SETTER(AUCAudioBuffer*, Buffer);
DECLARE_GETTER_SETTER(BOOL, Paused);
DECLARE_GETTER_SETTER(BOOL, Muted);
DECLARE_GETTER_SETTER(float, Gain);
DECLARE_GETTER_SETTER(float, Pitch);
DECLARE_GETTER_SETTER(float, Pan);

static void self_setDistance(AUCAudioSource* self, float distance);



#pragma mark -
#pragma mark Methods

AUCAudioSource* AUCAudioSource_create(AUCAudioContext* context)
{
	// Creation
	AUCAudioSource* self = ALLOC_OBJECT(AUCAudioSource);
	if(NULL == self)
	{
		return NULL;
	}
	
	// Method Assignment
	self->destroy = self_destroy;
	self->play = self_play;
	self->stop = self_stop;
	ASSIGN_GETTER_SETTER(Buffer);
	ASSIGN_GETTER_SETTER(Paused);
	ASSIGN_GETTER_SETTER(Muted);
	ASSIGN_GETTER_SETTER(Gain);
	ASSIGN_GETTER_SETTER(Pitch);
	ASSIGN_GETTER_SETTER(Pan);
	
	// Other Initialization
	AUC_MUTEX_INIT(&self->mutex);
	self->context = context;
	self->elementNumber = context->notifySourceInit(context, self);
	if(INVALID_ELEMENT == self->elementNumber)
	{
		AUC_LOG_ERROR("Cannot allocate more than %d sources per context", kMaxSourcesPerContext);
		self_destroy(self);
		return NULL;
	}
	
	self->gain = 1.0f;
	self->pitch = 1.0f;

	AUC_LOG_DEBUG("Initialized source %d", self->elementNumber);
	
	return self;
}

static void self_destroy(AUCAudioSource* self)
{
	self_disableRenderProc(self);
	self->context->notifySourceDestroy(self->context, self);

	AUC_MUTEX_DESTROY(&self->mutex);

	FREE_OBJECT(self);
}

static inline OSStatus self_setFloat32Property(AUCAudioSource* self,
											   UInt32 property,
											   Float32 value)
{
	return AudioUnitSetParameter(self->context->mixerUnit,
								 property,
								 kAudioUnitScope_Input,
								 self->elementNumber,
								 value,
								 sizeof(value));
}

static void self_enableRenderProc(AUCAudioSource* self)
{
	OSStatus error = noErr;
	
	AURenderCallbackStruct renderCallbackStruct =
	{
		.inputProc = &renderCallback,
		.inputProcRefCon = self,
	};
	
	OPT_LOCK_MIXER();
	error = AudioUnitSetProperty(self->context->mixerUnit,
								 kAudioUnitProperty_SetRenderCallback,
								 kAudioUnitScope_Input, 
								 self->elementNumber,
								 &renderCallbackStruct,
								 sizeof(renderCallbackStruct));
	OPT_UNLOCK_MIXER();

	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set mixer input callback for element %d", self->elementNumber);
	}
}

static void self_disableRenderProc(AUCAudioSource* self)
{
	OSStatus error = noErr;
	
	AURenderCallbackStruct renderCallbackStruct =
	{
		.inputProc = NULL,
		.inputProcRefCon = NULL,
	};
	
	OPT_LOCK_MIXER();
	error = AudioUnitSetProperty(self->context->mixerUnit,
								 kAudioUnitProperty_SetRenderCallback,
								 kAudioUnitScope_Input, 
								 self->elementNumber,
								 &renderCallbackStruct,
								 sizeof(renderCallbackStruct));
	OPT_UNLOCK_MIXER();

	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not clear mixer input callback for element %d", self->elementNumber);
	}
}

static AUCAudioBuffer* self_getBuffer(AUCAudioSource* self)
{
	return self->buffer;
}

static void self_setBuffer(AUCAudioSource* self, AUCAudioBuffer* buffer)
{
	OSStatus error = noErr;

	OPT_LOCK_MIXER();
	if(buffer != self->buffer)
	{
		self->buffer = buffer;
		
		error = AudioUnitSetProperty(self->context->mixerUnit,
									 kAudioUnitProperty_StreamFormat,
									 kAudioUnitScope_Input,
									 self->elementNumber,
									 &self->buffer->format,
									 sizeof(self->buffer->format));
	}		
	OPT_UNLOCK_MIXER();
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set mixer input format for element %d", self->elementNumber);
	}
}

static BOOL self_getMuted(AUCAudioSource* self)
{
	return self->muted;
}

static void self_setMuted(AUCAudioSource* self, BOOL muted)
{
	OSStatus error = noErr;
	
	OPT_LOCK_MIXER();
	if(muted != self->muted)
	{
		NSLog(@"Set %d to muted %d", self->elementNumber, muted);
		/*
		error = AudioUnitSetParameter(self->context->mixerUnit,
									  kMultiChannelMixerParam_Enable,
									  kAudioUnitScope_Input,
									  self->elementNumber,
									  !muted,
									  0);
		 */
		if(noErr == error)
		{
			self->muted = muted;
		}
	}
	OPT_UNLOCK_MIXER();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d mute to %d", self->elementNumber, muted);
	}
}

static BOOL self_getPaused(AUCAudioSource* self)
{
	return self->paused;
}

static void self_setPaused(AUCAudioSource* self, BOOL paused)
{
	if(paused != self->paused)
	{
		self->paused = paused;
		if(paused)
		{
			self_disableRenderProc(self);
		}
		else
		{
			self_enableRenderProc(self);
		}
	}
}
	
static void self_play(AUCAudioSource* self)
{
	if(self->playing)
	{
		self_stop(self);
	}
	
	self_enableRenderProc(self);
	self->playing = TRUE;
	self->paused = FALSE;
}

static void self_stop(AUCAudioSource* self)
{
	self_disableRenderProc(self);
	self->playing = FALSE;
	self->paused = FALSE;
	self->currentBytePos = 0;
}

static float self_getGain(AUCAudioSource* self)
{
	return self->gain;
}

static void self_setGain(AUCAudioSource* self, float gain)
{
	OSStatus error = noErr;
	
	OPT_LOCK_MIXER();
	if(gain != self->gain)
	{
		// Converting to sound pressure, so * 20
		Float32	db = fmaxf(log10f(gain) * 20.0f, -120.0f);
		error = self_setFloat32Property(self, k3DMixerParam_Gain, db);
		if(noErr == error)
		{
			self->gain = gain;
		}
	}
	OPT_UNLOCK_MIXER();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d gain to %f", self->elementNumber, gain);
	}
}

static float self_getPitch(AUCAudioSource* self)
{
	return self->pitch;
}

static void self_setPitch(AUCAudioSource* self, float pitch)
{
	OSStatus error = noErr;
	
	OPT_LOCK_MIXER();
	if(pitch != self->pitch)
	{
		error = self_setFloat32Property(self, k3DMixerParam_PlaybackRate, pitch);
		if(noErr == error)
		{
			self->pitch = pitch;
		}
	}
	OPT_UNLOCK_MIXER();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d pitch to %f", self->elementNumber, pitch);
	}
}

static float self_getPan(AUCAudioSource* self)
{
	return self->pan;
}

static void self_setPan(AUCAudioSource* self, float pan)
{
	OSStatus error = noErr;
		
	OPT_LOCK_MIXER();
	if(pan != self->pan)
	{
		// Distance needs to be something other than 0 (inside your head)
		self_setDistance(self, 1.0f);
//kAudioUnitProperty_3DMixerAttenuationCurve
		Float32 azimuth = fminf(pan*90, 90);
		azimuth = fmaxf(azimuth, -90);
		error = self_setFloat32Property(self, k3DMixerParam_Azimuth, azimuth);
		if(noErr == error)
		{
			self->pan = pan;
		}
	}
	OPT_UNLOCK_MIXER();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d pan to %f", self->elementNumber, pan);
	}
}

static void self_setDistance(AUCAudioSource* self, float distance)
{
	OSStatus error = noErr;
	
	OPT_LOCK_MIXER();
	if(distance != self->distance)
	{
		error = self_setFloat32Property(self, k3DMixerParam_Distance, distance);
		if(noErr == error)
		{
			self->distance = distance;
		}
	}
	OPT_UNLOCK_MIXER();
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d distance to %f", self->elementNumber, distance);
	}
}

static OSStatus renderCallback(void* inRefCon,
							   AudioUnitRenderActionFlags* ioActionFlags,
							   const AudioTimeStamp* inTimeStamp,
							   UInt32 inElementNumber,
							   UInt32 inNumberFrames,
							   AudioBufferList* ioData)
{
	AUCAudioSource* source = (AUCAudioSource*)inRefCon;
	AUCAudioBuffer* buffer = source->buffer;
	
	UInt32 inSize = buffer->numBytes;
	UInt32 inStartPos = source->currentBytePos;
	UInt32 bytesPerFrame = buffer->format.mBytesPerFrame;
	unsigned int numChannels = buffer->channelData[1] == NULL ? 1 : 2;
	
	UInt32 requestedBytes = inNumberFrames * bytesPerFrame;

	if(source->muted)
	{
		// TODO: Strange behavior here: If I have two sources and the second
		// one gets muted and sets this flag, BOTH sources get muted!
		// Only seems to affect 3dmixer.
//		*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
		source->currentBytePos = (inStartPos + requestedBytes) % inSize;
		for(unsigned int i = 0; i < numChannels; i++)
		{
			memset(ioData->mBuffers[i].mData, 0, requestedBytes);
		}
		return noErr;
	}
	else
	{
//		*ioActionFlags &= ~kAudioUnitRenderAction_OutputIsSilence;
	}

								  
	for(unsigned int i = 0; i < numChannels; i++)
	{
		UInt8* inData = (UInt8*)buffer->channelData[i];
		UInt8* outData = (UInt8*)ioData->mBuffers[i].mData;
		
		UInt32 inPos = inStartPos;
		UInt32 outPos = 0;
		UInt32 bytesRemaining = requestedBytes;
		
		while(bytesRemaining > 0)
		{
			UInt32 bytesToCopy = bytesRemaining;
			if(inPos + bytesToCopy > inSize)
			{
				bytesToCopy = inSize - inPos;
			}
			memcpy(outData+outPos, inData+inPos, bytesToCopy);
			bytesRemaining -= bytesToCopy;
			outPos += bytesToCopy;
			inPos += bytesToCopy;
			inPos %= inSize;
		}
		source->currentBytePos = inPos;
	}
    
    return noErr;
}
