//
//  AUCAudioContext.h
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
#ifndef AURAL_AUDIOCONTEXT_H
#define AURAL_AUDIOCONTEXT_H

#import <AudioToolbox/AudioToolbox.h>
#import "AUCMutex.h"

namespace aural {

#define INVALID_ELEMENT UINT_MAX

// TODO: Probably should set this lower since each buffer slot consumes memory.
#define kMaxSourcesPerContext 128

class AudioManager;
class AudioSource;
	
class AudioContext
{
public:
	AudioContext(AudioManager* manager);
	~AudioContext();

	AudioUnit mixerUnit() {return mixerUnit_;}

	UInt32 maxFramesPerSlice();
	void setMaxFramesPerSlice(UInt32 maxFramesPerSlice);
	
	Float64 sampleRate();
	void setSampleRate(Float64 sampleRate);
	
	bool active() {return active_;}
	void setActive(bool active);

	// TODO: should this be exposed?
	void setNumElements(UInt32 numElements);
	
	// TODO: create source
	
	UInt32 notifySourceInit(AudioSource& source);
	void notifySourceDestroy(AudioSource& source);
	
	Mutex& mutex() {return mutex_;}

private:
	Mutex mutex_;

	AudioManager* manager_;

    AUGraph auGraph_;
    AudioUnit mixerUnit_;
	
	AudioSource* sources_[kMaxSourcesPerContext];
	
	bool active_;
};

} // namespace

#endif // AURAL_AUDIOCONTEXT_H
