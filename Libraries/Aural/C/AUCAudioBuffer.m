//
//  AUCAudioBuffer.m
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

#import "AUCAudioBuffer.h"
#import "AUCMacros.h"


#pragma mark -
#pragma mark Forward Declarations

static void self_destroy(AUCAudioBuffer* self);



#pragma mark -
#pragma mark Methods

AUCAudioBuffer* AUCAudioBuffer_create(NSArray* channelData,
									  AudioStreamBasicDescription* format)
{
	// Creation
	AUCAudioBuffer* self = ALLOC_OBJECT(AUCAudioBuffer);
	if(NULL == self)
	{
		return NULL;
	}
	
	// Method Assignment
	self->destroy = self_destroy;
	
	// Other Initialization
	self->format = *format;
	NSData* monoChannel = [channelData objectAtIndex:0];
	self->numBytes = [monoChannel length];
	self->numFrames = self->numBytes / self->format.mBytesPerFrame;
	
	self->channelData[0] = [monoChannel bytes];
	if([channelData count] > 1)
	{
		self->channelData[1] = [[channelData objectAtIndex:1] bytes];
	}
	else
	{
		self->channelData[1] = NULL;
	}
	
	return self;
}

static void self_destroy(AUCAudioBuffer* self)
{
	FREE_MEMORY(self->channelData[0]);
	FREE_MEMORY(self->channelData[1]);

	FREE_OBJECT(self);
}
