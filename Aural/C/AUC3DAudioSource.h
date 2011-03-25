//
//  AUC3DAudioSource.h
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

#ifndef AURAL_3DAUDIOSOURCE_H
#define AURAL_3DAUDIOSOURCE_H

#import "AUCAudioSource.h"
#import "AUCMutex.h"

namespace aural {
	
	class AudioBuffer;
	class AudioContext;
	
	class AudioSource3D: public AudioSource
	{
	public:
		AudioSource3D(AudioContext& context);
		~AudioSource3D();
		
		AudioBuffer* buffer() {return buffer_;}
		void setBuffer(AudioBuffer* buffer);
		
		AudioContext& context() {return context_;}
		UInt32 elementNumber() {return elementNumber_;}
		
		bool paused() {return paused_;}
		void setPaused(bool paused);
		
		bool muted() {return muted_;}
		void setMuted(bool muted);
		
		float gain() {return gain_;}
		void setGain(float gain);
		
		float playbackRate() {return playbackRate_;}
		void setPlaybackRate(float playbackRate);
		
		float pan() {return pan_;}
		void setPan(float pan);
		
		void play();
		void stop();
		
		unsigned int currentBytePos() {return currentBytePos_;}
		void setCurrentBytePos(unsigned int currentBytePos) {currentBytePos_ = currentBytePos;}
		
	private:
		void enableRenderProc();
		void disableRenderProc();
		void initSoundTouch();
		void setDistance(float distance);
		OSStatus setFloat32Property(UInt32 property,
									Float32 value);
		
	private:	
		Mutex mutex_;
		
		//	int numLoops;
		// maybe max/min gain?
		float gain_;
		BOOL muted_;
		BOOL paused_;
		BOOL playing_;
		float playbackRate_;
		float pan_;
		
		// Internal
		float distance_;
		// position or pan...
		// playback finished callback...
		
		AudioBuffer* buffer_;
		AudioContext& context_;
		
		unsigned int currentBytePos_;
		
		UInt32 elementNumber_;
		
		// Use callback on main thread to actually destroy.
		// Need a way to keep a retain on the source until it is completely destroyed.
		// maybe on the buffer as well?
		//	bool toBeDestroyed;
		//	bool okToDestroy;
	};
	
} // namespace

#endif // AURAL_3DAUDIOSOURCE_H

