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

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "AUCTypes.h"

#define kMaxSourcesPerContext 1024


DEFINE_INTERFACE(AUCAudioContext)
{
	// =======
	// Methods
	// =======

	void (*destroy)(AUCAudioContext* self);
	AUCAudioSource* (*createSource)(AUCAudioContext* self);

	void (*setNumElements)(AUCAudioContext* self, UInt32 numElements);

	Float64 (*getSampleRate)(AUCAudioContext* self);
	void (*setSampleRate)(AUCAudioContext* self, Float64 sampleRate);

	UInt32 (*getMaxFramesPerSlice)(AUCAudioContext* self);
	void (*setMaxFramesPerSlice)(AUCAudioContext* self, UInt32 maxFramesPerSlice);

	BOOL (*getActive)(AUCAudioContext* self);
	void (*setActive)(AUCAudioContext* self, BOOL active);

	unsigned int (*notifySourceInit)(AUCAudioContext* self, AUCAudioSource* source);
	void (*notifySourceDestroy)(AUCAudioContext* self, AUCAudioSource* source);

	// =======
	// Members
	// =======

	pthread_mutex_t mutex;

	AUCAudioManager* manager;

	Float64 sampleRate;
	
    AUGraph auGraph;
    AudioUnit mixerUnit;
	
	AUCAudioSource* sources[kMaxSourcesPerContext];
	
	BOOL active;
};

AUCAudioContext* AUCAudioContext_create(AUCAudioManager* manager);
