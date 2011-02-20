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

#import <Foundation/Foundation.h>
#import "AUCTypes.h"
#import "AUCMutex.h"


DEFINE_INTERFACE(AUCAudioSource)
{
	// =======
	// Methods
	// =======
	
	void (*destroy)(AUCAudioSource* self);

	AUCAudioBuffer* (*getBuffer)(AUCAudioSource* self);
	void (*setBuffer)(AUCAudioSource* self, AUCAudioBuffer* buffer);

	BOOL (*getPaused)(AUCAudioSource* self);
	void (*setPaused)(AUCAudioSource* self, BOOL paused);
	
	BOOL (*getMuted)(AUCAudioSource* self);
	void (*setMuted)(AUCAudioSource* self, BOOL muted);
	
	void (*play)(AUCAudioSource* self);
	void (*stop)(AUCAudioSource* self);


	// =======
	// Members
	// =======
	
	pthread_mutex_t mutex;

//	float volume;
//	int numLoops;
	// maybe max/min gain?
	BOOL muted;
	BOOL paused;
	BOOL playing;
//	float pitch;
	// position or pan...
	// playback finished callback...
	
	AUCAudioBuffer* buffer;
	AUCAudioContext* context;
	
	unsigned int currentBytePos;
	
	UInt32 elementNumber;
	
	// Use callback on main thread to actually destroy.
	// Need a way to keep a retain on the source until it is completely destroyed.
	// maybe on the buffer as well?
//	bool toBeDestroyed;
//	bool okToDestroy;
};

AUCAudioSource* AUCAudioSource_create(AUCAudioContext* context);
