//
//  AUCAudioSource.h
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

#ifndef AURAL_AUDIOSOURCE_H
#define AURAL_AUDIOSOURCE_H

#import "AUCMutex.h"
#include "AudioRenderEngine.h"

namespace aural {

class AudioBuffer;
class AudioContext;

class AudioSource
{
public:
	AudioSource(AudioContext& context);
	virtual ~AudioSource();

	AudioBuffer* buffer() {return buffer_;}
	virtual void setBuffer(AudioBuffer* buffer);
	
	AudioContext& context() {return context_;}
	UInt32 elementNumber() {return elementNumber_;}

	bool paused() {return paused_;}
	virtual void setPaused(bool paused);
	
	bool muted() {return muted_;}
	virtual void setMuted(bool muted);
	
	float gain() {return gain_;}
	virtual void setGain(float gain);
	
	float pan() {return pan_;}
	virtual void setPan(float pan);

	void setPitch(float pitch);
	
	void setPlaybackRate(float pitch);

	virtual void play();
	virtual void stop();
	
	unsigned int currentBytePos() {return currentBytePos_;}
	virtual void setCurrentBytePos(unsigned int currentBytePos) {currentBytePos_ = currentBytePos;}

	AudioRenderEngine* renderEngine() {return renderEngine_;}
protected:
	void enableRenderProc();
	void disableRenderProc();
	OSStatus setFloat32Property(UInt32 property,
								Float32 value);
	void setDistance(float distance);

protected:	
	float gain_;
	BOOL muted_;
	BOOL paused_;
	BOOL playing_;
	float pan_;

	// Internal
	float distance_;
	// position or pan...
	// playback finished callback...
	
	AudioBuffer* buffer_;
	AudioContext& context_;
	
	unsigned int currentBytePos_;
	
	UInt32 elementNumber_;
	AudioRenderEngine* renderEngine_;
	
	float pitch_;
	float playbackRate_;
};

} // namespace

#endif // AURAL_AUDIOSOURCE_H

