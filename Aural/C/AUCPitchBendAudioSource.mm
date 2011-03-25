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

#if 0

#import <AudioToolbox/AudioToolbox.h>
#import "SoundTouch.h"
#import "AUCPitchBendAudioSource.h"
#import "AUCAudioBuffer.h"
#import "AUCAudioContext.h"
#import "AUCMacros.h"
#import "AUCLogging.h"

namespace aural {
	
	using namespace soundtouch;
	
#define OPTIONAL_LOCK_MIXER() OPTIONAL_LOCK(context_.mutex())
	
	
#pragma mark -
#pragma mark Forward Declarations
	
	static OSStatus renderCallback(void* inRefCon,
								   AudioUnitRenderActionFlags* ioActionFlags,
								   const AudioTimeStamp* inTimeStamp,
								   UInt32 inElementNumber,
								   UInt32 inNumberFrames,
								   AudioBufferList* ioData);
	
	
	
	
#pragma mark -
#pragma mark Methods
	
	AudioSourcePitchBend::AudioSourcePitchBend(AudioContext& context)
	: context_(context)
	, gain_(1.0f)
	, pitch_(1.0f)
	, pan_(0.5f)
	, soundTouch_(new SoundTouch)
	{
		elementNumber_ = context.notifySourceInit(*this);
		if(INVALID_ELEMENT == elementNumber_)
		{
			AUC_LOG_ERROR("Cannot allocate more than %d sources per context", kMaxSourcesPerContext);
			// TODO: exception?
		}
		
		initSoundTouch();
		
		AUC_LOG_DEBUG("Initialized source %d", elementNumber_);
	}
	
	AudioSourcePitchBend::~AudioSourcePitchBend()
	{
		disableRenderProc();
		context_.notifySourceDestroy(*this);
		delete (SoundTouch*)soundTouch_;
	}
	
	OSStatus AudioSourcePitchBend::setFloat32Property(UInt32 property,
											 Float32 value)
	{
		return AudioUnitSetParameter(context_.mixerUnit(),
									 property,
									 kAudioUnitScope_Input,
									 elementNumber_,
									 value,
									 sizeof(value));
	}
	
	void AudioSourcePitchBend::enableRenderProc()
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
	
	void AudioSourcePitchBend::disableRenderProc()
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
	
	void AudioSourcePitchBend::setBuffer(AudioBuffer* buffer)
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
			}		
		}
		if(noErr != error)
		{
			REPORT_AUGRAPH_ERROR(error, "Could not set mixer input format for element %d", elementNumber_);
		}
	}
	
	void AudioSourcePitchBend::setMuted(bool muted)
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
	
	void AudioSourcePitchBend::setPaused(bool paused)
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
	
	void AudioSourcePitchBend::play()
	{
		if(playing_)
		{
			stop();
		}
		
		enableRenderProc();
		playing_ = TRUE;
		paused_ = FALSE;
	}
	
	void AudioSourcePitchBend::stop()
	{
		disableRenderProc();
		playing_ = FALSE;
		paused_ = FALSE;
		currentBytePos_ = 0;
	}
	
	void AudioSourcePitchBend::setGain(float gain)
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
	
	void AudioSourcePitchBend::setPitch(float pitch)
	{
		float totalAmount = (pitch - 1.0f) * 2.0f;
		//	NSLog(@"%f = %f", pitch, totalAmount);
		
		SoundTouch* pSoundTouch = (SoundTouch*)soundTouch_;
		pSoundTouch->setPitchSemiTones(totalAmount);
		pitch_ = pitch;
	}
	
	void AudioSourcePitchBend::setPlaybackRate(float pitch)
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
	
	void AudioSourcePitchBend::setPan(float pan)
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
	
	void AudioSourcePitchBend::setDistance(float distance)
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
	
	
	void AudioSourcePitchBend::initSoundTouch()
	{
		SoundTouch* pSoundTouch = (SoundTouch*)soundTouch_;
		
		int sampleRate = 44100;
		int channels = 1;
		
		pSoundTouch->setSampleRate(sampleRate);
		pSoundTouch->setChannels(channels);
		
		pSoundTouch->setTempoChange(0.0f);
		pSoundTouch->setPitchSemiTones(0.0f);
		pSoundTouch->setRateChange(0.0f);
		
		pSoundTouch->setSetting(SETTING_USE_QUICKSEEK, YES);
		pSoundTouch->setSetting(SETTING_USE_AA_FILTER, NO);
		
		if (0)
		{
			// use settings for speech processing
			pSoundTouch->setSetting(SETTING_SEQUENCE_MS, 40);
			pSoundTouch->setSetting(SETTING_SEEKWINDOW_MS, 15);
			pSoundTouch->setSetting(SETTING_OVERLAP_MS, 8);
		}
	}
	
	static OSStatus renderCallback(void* inRefCon,
								   AudioUnitRenderActionFlags* ioActionFlags,
								   const AudioTimeStamp* inTimeStamp,
								   UInt32 inElementNumber,
								   UInt32 inNumberFrames,
								   AudioBufferList* ioData)
	{
		AudioSourcePitchBend* source = (AudioSourcePitchBend*)inRefCon;
		AudioBuffer* buffer = source->buffer();
		SoundTouch* pSoundTouch = (SoundTouch*)source->soundTouch();
		
		UInt32 inSize = buffer->numBytes();
		UInt32 inStartPos = source->currentBytePos();
		UInt32 bytesPerFrame = buffer->format().mBytesPerFrame;	
		UInt32 requestedBytes = inNumberFrames * bytesPerFrame;
		
		UInt8* inData = (UInt8*)buffer->leftChannelData();
		UInt8* outData = (UInt8*)ioData->mBuffers[0].mData;
		
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
			pSoundTouch->putSamples((SAMPLETYPE*)(inData+inPos), bytesToCopy / bytesPerFrame);
			//		memcpy(outData+outPos, inData+inPos, bytesToCopy);
			bytesRemaining -= bytesToCopy;
			outPos += bytesToCopy;
			inPos += bytesToCopy;
			inPos %= inSize;
		}
		
		source->setCurrentBytePos(inPos);
		
		int remainingSamples = inNumberFrames;
		int nSamples = 0;
		int dstPos = 0;
		char* dst = (char*)outData;
		
		do 
		{
			nSamples = pSoundTouch->receiveSamples((SAMPLETYPE*)(dst+dstPos), remainingSamples);
			dstPos += nSamples * bytesPerFrame;
			remainingSamples -= nSamples;
		} while (nSamples != 0);
		
		/*
		 pSoundTouch->flush();
		 do 
		 {
		 nSamples = pSoundTouch->receiveSamples((SAMPLETYPE*)(dst+dstPos), remainingSamples);
		 dstPos += nSamples * bytesPerFrame;
		 remainingSamples -= nSamples;
		 } while (nSamples != 0);
		 
		 pSoundTouch->clear();
		 */
	    
		return noErr;
	}

} // namespace

#endif
