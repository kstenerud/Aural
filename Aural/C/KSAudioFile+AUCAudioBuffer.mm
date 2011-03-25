//
//  KSAudioFile+AUCAudioBuffer.m
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

#import "KSAudioFile+AUCAudioBuffer.h"
#import "AUCLogging.h"
#import "AUCAudioBuffer.h"


#define kDefaultSampleRate 44100


@implementation KSAudioFile (AudioBuffer)

+ (Float64) hardwareSampleRate
{
	Float64 value = 0;
	UInt32 size = sizeof(value);
	OSStatus error = AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate,
											 &size,
											 &value);
	if(noErr != error)
	{
		REPORT_AUDIOSESSION_ERROR(error,
								  "Could not get current hardware sample rate. Using %d",
								  kDefaultSampleRate);
		return kDefaultSampleRate;
	}
	return value;
}

+ (aural::AudioBuffer*) aucBufferWithUrl:(NSURL*) url
							  stereo:(bool) stereo
{
	return [self aucBufferWithUrl:url
						   stereo:stereo
					   sampleRate:(UInt32)[self hardwareSampleRate]];
}

+ (aural::AudioBuffer*) aucBufferWithUrl:(NSURL*) url
							  stereo:(bool) stereo
						  sampleRate:(UInt32) sampleRate
{
	KSAudioFile* file = [[self alloc] initWithUrl:url];
	[file setToAudioBufferFormatWithStereo:stereo
								sampleRate:sampleRate];
	
	aural::AudioBuffer* buffer = [file aucBufferWithName:[url description]
										  startFrame:0
										   numFrames:-1];
	
	[file release];
	
	return buffer;
}

- (void) setToAudioBufferFormatWithStereo:(bool) stereo
{
	[self setToAudioBufferFormatWithStereo:stereo
								sampleRate:(UInt32)[[self class] hardwareSampleRate]];
}

- (void) setToAudioBufferFormatWithStereo:(bool) stereo
							   sampleRate:(UInt32) sampleRate
{
	//	self.sampleRate = sampleRate;
	_outputDescription.mSampleRate = sampleRate;
	// TODO: Need to sort this part out better.
//	[self setTo32BitCanonicalFormatWithStereo:stereo interleaved:NO];
	[self setTo16BitCanonicalFormatWithStereo:NO interleaved:YES];
}

- (aural::AudioBuffer*) aucBufferWithName:(NSString*) name
						   startFrame:(SInt64) startFrame
							numFrames:(SInt64) numFrames
{
	NSArray* channelData = [self audioDataWithStartFrame:startFrame
											   numFrames:numFrames
											 interleaved:NO];
	
	DataBuffer* leftData = [channelData objectAtIndex:0];
	DataBuffer* rightdata = [channelData count] > 1 ? [channelData objectAtIndex:1] : nil;

	leftData.freeOnDealloc = NO;
	rightdata.freeOnDealloc = NO;

	return new aural::AudioBuffer(leftData.data,
								  rightdata.data,
								  leftData.numBytes,
								  _outputDescription);
}

@end
