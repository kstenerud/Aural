//
//  KSAudioFile.m
//  ObjectiveGems
//
//  Created by Karl Stenerud on 10-12-24.
//
// Copyright 2010 Karl Stenerud
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

#import "KSAudioFile.h"


/** Synthesize a property which synchronizes with the output stream's
 * AudioStreamBasicDescription.
 *
 * @param TYPE The property's type.
 * @param NAME_CAMEL The property name in camelCase.
 * @param NAME_CAPITALIZED The property name in CapitalizedCase.
 */
#define SYNTHESIZE_ASBD_PROPERTY(TYPE, NAME_CAMEL, NAME_CAPITALIZED) \
- (TYPE) NAME_CAMEL \
{ \
	return _outputDescription.m##NAME_CAPITALIZED; \
} \
\
- (void) set##NAME_CAPITALIZED:(TYPE) value \
{ \
	if(_outputDescription.m##NAME_CAPITALIZED != value) \
	{ \
		_outputDescription.m##NAME_CAPITALIZED = value; \
		[self update_outputDescription]; \
	} \
}


/** Report an error.
 *
 * @param FMT Message with NSLog() style formatting.
 * @param ... Arguments
 */
#define REPORT_ERROR(FMT, ...) \
NSLog(@"Error: %s: %@", \
	  __PRETTY_FUNCTION__, \
	  [NSString stringWithFormat:(FMT), ##__VA_ARGS__])

/** Report an ExtAudio error.
 *
 * @param ERROR_CODE The error code.
 * @param FMT Message with NSLog() style formatting.
 * @param ... Arguments
 */
#define REPORT_EXTAUDIO_ERROR(ERROR_CODE, FMT, ...) \
[KSAudioFile notifyExtAudioError:(ERROR_CODE) \
						function:__PRETTY_FUNCTION__ \
					 description:(FMT), ##__VA_ARGS__]; \


/**
 * (INTERNAL USE) Private methods for OALAudioFile. 
 */
@interface KSAudioFile ()

/** (INTERNAL USE) Close any resources belonging to the OS.
 */
- (void) closeOSResources;

/** (INTERNAL USE) Update the output streaming format.
 */
- (void) update_outputDescription;

/** (INTERNAL USE) Set the output format to the canonical format of this
 *  device.
 *
 * @param stereo If true, output 2 channels (stereo), otherwise output 1
 *               channel (mono).
 * @param interleaved If true, set up for interleaved channel data.
 * @param bytesPerChannel The number of bytes per channel.
 */
- (void) setToCanonicalFormatWithStereo:(bool) stereo
						    interleaved:(bool) interleaved
						bytesPerChannel:(UInt32) bytesPerSample;

@end


@implementation KSAudioFile

+ (KSAudioFile*) fileWithUrl:(NSURL*) url
{
	return [[[self alloc] initWithUrl:url] autorelease];
}


- (id) initWithUrl:(NSURL*) urlIn
{
	if(nil != (self = [super init]))
	{
		_url = [urlIn retain];
		
		OSStatus error = noErr;
		UInt32 size = 0;
		
		if(nil == _url)
		{
			REPORT_ERROR(@"Cannot open NULL file / url");
			error = -1;
			goto onDone;
		}
		
		// Open the file
		if(noErr != (error = ExtAudioFileOpenURL((CFURLRef)_url, &_fileHandle)))
		{
			REPORT_EXTAUDIO_ERROR(error, @"Could not open url %@", _url);
			goto onDone;
		}
		
		// Get some info about the file
		size = sizeof(_frameCount);
		if(noErr != (error = ExtAudioFileGetProperty(_fileHandle,
													 kExtAudioFileProperty_FileLengthFrames,
													 &size,
													 &_frameCount)))
		{
			REPORT_EXTAUDIO_ERROR(error, @"Could not get frame count for file (url = %@)", _url);
			goto onDone;
		}

		memset(&_inputDescription, 0, sizeof(_inputDescription));
		size = sizeof(_inputDescription);
		if(noErr != (error = ExtAudioFileGetProperty(_fileHandle,
													 kExtAudioFileProperty_FileDataFormat,
													 &size,
													 &_inputDescription)))
		{
			REPORT_EXTAUDIO_ERROR(error, @"Could not get audio format for file (url = %@)", _url);
			goto onDone;
		}
		
		_outputDescription = _inputDescription;
		
	onDone:
		if(noErr != error)
		{
			[self release];
			return nil;
		}
		
	}
	return self;
}

- (void) dealloc
{
	[self closeOSResources];
	
	[_url release];
	
	[super dealloc];
}

- (void) closeOSResources
{
	@synchronized(self)
	{
		if(nil != _fileHandle)
		{
			OSStatus error = ExtAudioFileDispose(_fileHandle);
			if(noErr != error)
			{
				REPORT_EXTAUDIO_ERROR(error, @"Error closing file (url = %@)", _url);
			}
			_fileHandle = nil;
		}
	}
}

- (void) close
{
	[self closeOSResources];
}

- (NSString*) description
{
	return [NSString stringWithFormat:@"<%@: %p: %@>", [self class], self, _url];
}

@synthesize url = _url;

@synthesize frameCount = _frameCount;

SYNTHESIZE_ASBD_PROPERTY(Float64, sampleRate, SampleRate);

SYNTHESIZE_ASBD_PROPERTY(UInt32, formatID, FormatID);

SYNTHESIZE_ASBD_PROPERTY(UInt32, formatFlags, FormatFlags);

SYNTHESIZE_ASBD_PROPERTY(UInt32, bytesPerPacket, BytesPerPacket);

SYNTHESIZE_ASBD_PROPERTY(UInt32, framesPerPacket, FramesPerPacket);

SYNTHESIZE_ASBD_PROPERTY(UInt32, bytesPerFrame, BytesPerFrame);

SYNTHESIZE_ASBD_PROPERTY(UInt32, channelsPerFrame, ChannelsPerFrame);

SYNTHESIZE_ASBD_PROPERTY(UInt32, bitsPerChannel, BitsPerChannel);

- (AudioStreamBasicDescription*) inputFormat
{
	return &_inputDescription;
}

- (void) update_outputDescription
{
	OSStatus error = noErr;
	if(noErr != (error = ExtAudioFileSetProperty(_fileHandle,
												 kExtAudioFileProperty_ClientDataFormat,
												 sizeof(AudioStreamBasicDescription),
												 &_outputDescription)))
	{
		REPORT_EXTAUDIO_ERROR(error, @"Could not set new audio format for file (url = %@)", _url);
	}
}

- (void) setToCanonicalFormatWithStereo:(bool) stereo
						    interleaved:(bool) interleaved
						bytesPerChannel:(UInt32) bytesPerChannel
{
	AudioStreamBasicDescription newFormat = _inputDescription;

	newFormat.mSampleRate       = _outputDescription.mSampleRate;
	newFormat.mFormatID         = kAudioFormatLinearPCM;
	newFormat.mFormatFlags      = kAudioFormatFlagsAudioUnitCanonical;
	newFormat.mFramesPerPacket  = 1;
	newFormat.mChannelsPerFrame = stereo && _inputDescription.mChannelsPerFrame > 1 ? 2 : 1;
	newFormat.mBitsPerChannel   = bytesPerChannel * 8;
	
	if(interleaved)
	{
		newFormat.mBytesPerFrame = newFormat.mChannelsPerFrame * bytesPerChannel;
	}
	else
	{
		newFormat.mBytesPerFrame = bytesPerChannel;
		newFormat.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
	}
	newFormat.mBytesPerPacket = newFormat.mBytesPerFrame * newFormat.mFramesPerPacket;

	[self setOutputFormat:&newFormat];
}

- (void) setTo16BitCanonicalFormatWithStereo:(bool) stereo
								 interleaved:(bool) interleaved
{
	return [self setToCanonicalFormatWithStereo:stereo
								    interleaved:interleaved
								bytesPerChannel:sizeof(AudioSampleType)];
}

- (void) setTo32BitCanonicalFormatWithStereo:(bool) stereo
								 interleaved:(bool) interleaved
{
	return [self setToCanonicalFormatWithStereo:stereo
								    interleaved:interleaved
								bytesPerChannel:sizeof(AudioUnitSampleType)];
}

- (void) setOutputFormat:(AudioStreamBasicDescription*) format
{
	if(memcmp(&_outputDescription, format, sizeof(_outputDescription)) != 0)
	{
//		_outputDescription = *format;
		memcpy(&_outputDescription, format, sizeof(_outputDescription));
		[self update_outputDescription];
	}
}

- (NSMutableData*) audioDataWithStartFrame:(SInt64) startFrame
								 numFrames:(SInt64) numFrames
{
	return [[self audioDataWithStartFrame:startFrame
								numFrames:numFrames
							  interleaved:YES] objectAtIndex:0];
}

- (NSArray*) audioDataWithStartFrame:(SInt64) startFrame
						   numFrames:(SInt64) numFrames
						 interleaved:(bool) interleaved
{
	NSAssert(nil != _fileHandle, @"Attempted to read from closed file");
	
	OSStatus error = noErr;
	UInt32 numBuffers = 0;
	UInt32 streamSize = 0;
	UInt32 channelsPerBuffer = 0;
	UInt32 numFramesRead = 0;
	AudioBufferList* bufferList = nil;
	NSMutableArray* buffers = nil;
	
	// < 0 means read to the end of the file.
	if(numFrames < 0)
	{
		numFrames = _frameCount - startFrame;
	}
	
	if(interleaved)
	{
		numBuffers = 1;
		streamSize = (UInt32)numFrames * _outputDescription.mBytesPerFrame;
		channelsPerBuffer = _outputDescription.mChannelsPerFrame;
	}
	else
	{
		numBuffers = _outputDescription.mChannelsPerFrame;
		streamSize = (UInt32)numFrames * _outputDescription.mBitsPerChannel / 8;
		channelsPerBuffer = 1;
	}
	
	bufferList = (AudioBufferList*)[[NSMutableData dataWithLength:sizeof(AudioBufferList)
									 + sizeof(AudioBuffer)
									 * (numBuffers - 1)] mutableBytes];
	bufferList->mNumberBuffers = numBuffers;
	buffers = [[NSMutableArray alloc] initWithCapacity:numBuffers];
	
	for(UInt32 i = 0; i < numBuffers; i++)
	{
		NSMutableData* data = [[NSMutableData alloc] initWithLength:streamSize];
		[buffers addObject:data];
		[data release];
		
        bufferList->mBuffers[i].mNumberChannels = channelsPerBuffer;
        bufferList->mBuffers[i].mDataByteSize = [data length];
        bufferList->mBuffers[i].mData = [data mutableBytes];
	}
	
	if(noErr != (error = ExtAudioFileSeek(_fileHandle, startFrame)))
	{
		REPORT_EXTAUDIO_ERROR(error, @"Could not seek to %ll in file (url = %@)",
							  startFrame,
							  _url);
		goto onFail;
	}
	
	numFramesRead = (UInt32)numFrames;
	if(noErr != (error = ExtAudioFileRead(_fileHandle, &numFramesRead, bufferList)))
	{
		REPORT_EXTAUDIO_ERROR(error, @"Could not read audio data in file (url = %@)",
							  _url);
		goto onFail;
	}
	
	return [buffers autorelease];
	
onFail:
	[buffers release];
	return nil;
}

+ (void) notifyExtAudioError:(OSStatus)errorCode
					function:(const char*) function
				 description:(NSString*) description, ...
{
	NSString* errorString = nil;
	
	switch(errorCode)
	{
#ifdef __IPHONE_3_1
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1
		case kExtAudioFileError_CodecUnavailableInputConsumed:
			errorString = @"Write function interrupted - last buffer written";
			break;
		case kExtAudioFileError_CodecUnavailableInputNotConsumed:
			errorString = @"Write function interrupted - last buffer not written";
			break;
#endif /* __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1 */
#endif /* __IPHONE_3_1 */
		case kExtAudioFileError_InvalidProperty:
			errorString = @"Invalid property";
			break;
		case kExtAudioFileError_InvalidPropertySize:
			errorString = @"Invalid property size";
			break;
		case kExtAudioFileError_NonPCMClientFormat:
			errorString = @"Non-PCM client format";
			break;
		case kExtAudioFileError_InvalidChannelMap:
			errorString = @"Wrong number of channels for format";
			break;
		case kExtAudioFileError_InvalidOperationOrder:
			errorString = @"Invalid operation order";
			break;
		case kExtAudioFileError_InvalidDataFormat:
			errorString = @"Invalid data format";
			break;
		case kExtAudioFileError_MaxPacketSizeUnknown:
			errorString = @"Max packet size unknown";
			break;
		case kExtAudioFileError_InvalidSeek:
			errorString = @"Seek offset out of bounds";
			break;
		case kExtAudioFileError_AsyncWriteTooLarge:
			errorString = @"Async write too large";
			break;
		case kExtAudioFileError_AsyncWriteBufferOverflow:
			errorString = @"Async write could not be completed in time";
			break;
		case noErr:
			return;
		default:
			errorString = @"Unknown ExtAudio error";
	}
	
	va_list args;
	va_start(args, description);
	description = [[NSString alloc] initWithFormat:description arguments:args];
	va_end(args);
	
	NSLog(@"Error: %s: %@",
		  function,
		  [NSString stringWithFormat:@"%@ (error code %d: %@)",
		   description,
		   errorCode,
		   errorString]);
	
	[description release];
}
	
@end

