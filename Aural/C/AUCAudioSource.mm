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

#import <AudioToolbox/AudioToolbox.h>
#include <SoundTouch/SoundTouch.h>
#import "AUCAudioSource.h"
#import "AUCAudioBuffer.h"
#import "AUCAudioContext.h"
#import "AUCMacros.h"
#import "AUCLogging.h"
#import "SoundTouchRenderEngine.h"
#import "PassthroughRenderEngine.h"

namespace aural {

	static OSStatus renderCallback(void* inRefCon,
								   AudioUnitRenderActionFlags* ioActionFlags,
								   const AudioTimeStamp* inTimeStamp,
								   UInt32 inElementNumber,
								   UInt32 inNumberFrames,
								   AudioBufferList* ioData);
	
#pragma mark -
#pragma mark Methods

AudioSource::AudioSource(AudioContext& context)
: context_(context)
, gain_(1.0f)
, pan_(0.5f)
, renderEngine_(new SoundTouchRenderEngine())
{
	elementNumber_ = context.notifySourceInit(*this);
	if(INVALID_ELEMENT == elementNumber_)
	{
		AUC_LOG_ERROR("Cannot allocate more than %d sources per context", kMaxSourcesPerContext);
		// TODO: exception?
	}
	
	AUC_LOG_DEBUG("Initialized source %d", elementNumber_);
}

AudioSource::~AudioSource()
{
	disableRenderProc();
	context_.notifySourceDestroy(*this);
	delete renderEngine_;
}

OSStatus AudioSource::setFloat32Property(UInt32 property,
										 Float32 value)
{
	return AudioUnitSetParameter(context_.mixerUnit(),
								 property,
								 kAudioUnitScope_Input,
								 elementNumber_,
								 value,
								 sizeof(value));
}

void AudioSource::enableRenderProc()
{
	OSStatus error = noErr;
	
	AURenderCallbackStruct renderCallbackStruct;
	renderCallbackStruct.inputProc = &renderCallback;
	renderCallbackStruct.inputProcRefCon = this;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		error = AudioUnitSetProperty(context_.mixerUnit(),
									 kAudioUnitProperty_SetRenderCallback,
									 kAudioUnitScope_Input, 
									 elementNumber_,
									 &renderCallbackStruct,
									 sizeof(renderCallbackStruct));
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set mixer input callback for element %d", elementNumber_);
	}
}

void AudioSource::disableRenderProc()
{
	OSStatus error = noErr;
	
	AURenderCallbackStruct renderCallbackStruct;
	renderCallbackStruct.inputProc = NULL;
	renderCallbackStruct.inputProcRefCon = NULL;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		error = AudioUnitSetProperty(context_.mixerUnit(),
									 kAudioUnitProperty_SetRenderCallback,
									 kAudioUnitScope_Input, 
									 elementNumber_,
									 &renderCallbackStruct,
									 sizeof(renderCallbackStruct));
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not clear mixer input callback for element %d", elementNumber_);
	}
}

void AudioSource::setBuffer(AudioBuffer* buffer)
{
	OSStatus error = noErr;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		if(buffer != buffer_)
		{
			buffer_ = buffer;
			
			error = AudioUnitSetProperty(context_.mixerUnit(),
										 kAudioUnitProperty_StreamFormat,
										 kAudioUnitScope_Input,
										 elementNumber_,
										 &buffer_->format(),
										 sizeof(buffer_->format()));
			renderEngine_->setBuffer(buffer_->leftChannelData(), buffer_->numBytes());
		}		
	}
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set mixer input format for element %d", elementNumber_);
	}
}

void AudioSource::setMuted(bool muted)
{
	OSStatus error = noErr;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		if(muted != muted_)
		{
			//NSLog(@"Set %d to muted %d", elementNumber_, muted);
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
				muted_ = muted;
			}
		}
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d mute to %d", elementNumber_, muted);
	}
}

void AudioSource::setPaused(bool paused)
{
	if(paused != paused_)
	{
		paused = paused_;
		if(paused_)
		{
			disableRenderProc();
		}
		else
		{
			enableRenderProc();
		}
	}
}
	
void AudioSource::play()
{
	if(playing_)
	{
		stop();
	}
	
	enableRenderProc();
	playing_ = TRUE;
	paused_ = FALSE;
}

void AudioSource::stop()
{
	disableRenderProc();
	renderEngine_->setPosition(0);
	playing_ = FALSE;
	paused_ = FALSE;
	currentBytePos_ = 0;
}

void AudioSource::setGain(float gain)
{
	OSStatus error = noErr;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		if(gain != gain_)
		{
			// Converting to sound pressure, so * 20
			Float32	db = fmaxf(log10f(gain) * 20.0f, -120.0f);
			error = setFloat32Property(k3DMixerParam_Gain, db);
			if(noErr == error)
			{
				gain_ = gain;
			}
		}
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d gain to %f", elementNumber_, gain);
	}
}
	
void AudioSource::setPitch(float pitch)
{
	float totalAmount = (pitch - 1.0f) * 2.0f;
//	NSLog(@"%f = %f", pitch, totalAmount);

	((SoundTouchRenderEngine*)renderEngine_)->setPitchSemiTones(totalAmount);
	pitch_ = pitch;
}

void AudioSource::setPlaybackRate(float pitch)
{
	OSStatus error = noErr;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		if(pitch != pitch_)
		{
			error = setFloat32Property(k3DMixerParam_PlaybackRate, pitch);
			if(noErr == error)
			{
				pitch_ = pitch;
			}
		}
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d pitch to %f", elementNumber_, pitch);
	}
}

void AudioSource::setPan(float pan)
{
	OSStatus error = noErr;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		if(pan != pan_)
		{
			// Distance needs to be something other than 0 (inside your head)
			setDistance(1.0f);
//kAudioUnitProperty_3DMixerAttenuationCurve
			Float32 azimuth = fminf(pan*90, 90);
			azimuth = fmaxf(azimuth, -90);
			error = setFloat32Property(k3DMixerParam_Azimuth, azimuth);
			if(noErr == error)
			{
				pan_ = pan;
			}
		}
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d pan to %f", elementNumber_, pan);
	}
}

void AudioSource::setDistance(float distance)
{
	OSStatus error = noErr;
	
	{
		OPTIONAL_LOCK(context_.mutex());
		if(distance != distance_)
		{
			error = setFloat32Property(k3DMixerParam_Distance, distance);
			if(noErr == error)
			{
				distance_ = distance;
			}
		}
	}
	
	if(noErr != error)
	{
		REPORT_AUGRAPH_ERROR(error, "Could not set source %d distance to %f", elementNumber_, distance);
	}
}


static OSStatus renderCallback(void* inRefCon,
							   AudioUnitRenderActionFlags* ioActionFlags,
							   const AudioTimeStamp* inTimeStamp,
							   UInt32 inElementNumber,
							   UInt32 inNumberFrames,
							   AudioBufferList* ioData)
{
	AudioSource* source = (AudioSource*)inRefCon;
	AudioRenderEngine* engine = source->renderEngine();
	
	engine->renderTo(ioData->mBuffers[0].mData, inNumberFrames);
	    
    return noErr;
}

	/*
static OSStatus renderCallbackOld(void* inRefCon,
							   AudioUnitRenderActionFlags* ioActionFlags,
							   const AudioTimeStamp* inTimeStamp,
							   UInt32 inElementNumber,
							   UInt32 inNumberFrames,
							   AudioBufferList* ioData)
{
	AudioSource* source = (AudioSource*)inRefCon;
	AudioBuffer* buffer = source->buffer();
	
	UInt32 inSize = buffer->numBytes();
	UInt32 inStartPos = source->currentBytePos();
	UInt32 bytesPerFrame = buffer->format().mBytesPerFrame;
	unsigned int numChannels = buffer->rightChannelData() == NULL ? 1 : 2;
	
	UInt32 requestedBytes = inNumberFrames * bytesPerFrame;

	if(source->muted())
	{
		// TODO: Strange behavior here: If I have two sources and the second
		// one gets muted and sets this flag, BOTH sources get muted!
		// Only seems to affect 3dmixer.
//		*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
		source->setCurrentBytePos((inStartPos + requestedBytes) % inSize);
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

	void*const channelData[2] = {buffer->leftChannelData(), buffer->rightChannelData()};
								  
	for(unsigned int i = 0; i < numChannels; i++)
	{
		UInt8* inData = (UInt8*const)channelData[i];
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
		source->setCurrentBytePos(inPos);
	}
    
    return noErr;
}
	 */

} // namespace
