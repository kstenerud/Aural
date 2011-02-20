//
//  KSAudioFile.h
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

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>


/**
 * Reads and potentially converts audio data from a url.
 *
 * By default, this class will simply stream the audio data as-is to a data
 * buffer. If any properties are changed, it will convert to a format with the
 * new properties as it reads.
 *
 * Note: This object holds system resources while alive, so resource
 *       management rules apply. To explicitly release all held system
 *       resources, use the "close" method.
 */
@interface KSAudioFile : NSObject
{
	NSURL* _url;

	SInt64 _frameCount;
	
	/** The OS specific file handle */
	ExtAudioFileRef _fileHandle;
	
	/** A description of the audio data being read from the url. */
	AudioStreamBasicDescription _inputDescription;

	/** A description of the audio data being converted to. */
	AudioStreamBasicDescription _outputDescription;
}

/** The URL of the audio data */
@property(nonatomic,readonly) NSURL* url;

/** The total number of audio frames in this file */
@property(nonatomic,readonly) SInt64 frameCount;

/** The format of the audio data being read from the url.
 *
 * This is the original format of the data, not necessarily what is
 * being output while reading the file. <br>
 *
 * By default when first opening a file, the output format will match the
 * input format until you change it by setting properties or calling one
 * of the [setToXXBitCanonicalFormatWithStereo:] methods.
 */
@property(nonatomic,readonly) AudioStreamBasicDescription* inputFormat;

/** The number of sample frames per second of the data in the output
 *  stream.
 */
@property(nonatomic,readwrite) Float64 sampleRate;

/** A four char code indicating the general kind of data in the output
 *  stream.
 */
@property(nonatomic,readwrite) UInt32 formatID;

/** Flags specific to each format. */
@property(nonatomic,readwrite) UInt32 formatFlags;

/** The number of bytes in a packet of data. */
@property(nonatomic,readwrite) UInt32 bytesPerPacket;

/** The number of sample frames in each packet of data. */
@property(nonatomic,readwrite) UInt32 framesPerPacket;

/** The number of bytes in a single sample frame of data. */
@property(nonatomic,readwrite) UInt32 bytesPerFrame;

/** The number of channels in each frame of data. */
@property(nonatomic,readwrite) UInt32 channelsPerFrame;

/** The number of bits of sample data for each channel in a frame of data. */
@property(nonatomic,readwrite) UInt32 bitsPerChannel;


/** Set the output format to the 16 bit canonical format of this device.
 *
 * On iOS, this is a 16-bit signed integer. On OSX, this is actually a
 * 32-bit floating point number.
 *
 * @param stereo If true, output 2 channels (stereo), otherwise output 1
 *               channel (mono).
 * @param interleaved If true, set up for interleaved channel data.
 */
- (void) setTo16BitCanonicalFormatWithStereo:(bool) stereo
								 interleaved:(bool) interleaved;

/** Set the output format to the 32 bit canonical format of this device.
 *
 * On iOS, this is a 32-bit signed 8.24 fixed point number. On OSX, this is a
 * 32-bit floating point number.
 *
 * @param stereo If true, output 2 channels (stereo), otherwise output 1
 *               channel (mono).
 * @param interleaved If true, set up for interleaved channel data.
 */
- (void) setTo32BitCanonicalFormatWithStereo:(bool) stereo
								 interleaved:(bool) interleaved;

/** Set the output format.
 * You can either set the output format using individual properties,
 * or just set everything in one shot using this method.
 *
 * @param format The format to set. The values inside format will be copied.
 */
- (void) setOutputFormat:(AudioStreamBasicDescription*) format;

/** Open the audio file at the specified URL.
 *
 * @param url The URL to open the audio file from.
 * @return A new audio file object.
 */
+ (KSAudioFile*) fileWithUrl:(NSURL*) url;

/** Initialize this object with the audio file at the specified URL.
 *
 * @param url The URL to open the audio file from.
 * @return The initialized audio file object.
 */
- (id) initWithUrl:(NSURL*) url;

/** Close any OS resources in use by this object.
 * Any operations called on this object after closing will likely fail.
 */
- (void) close;

/** Read audio data from this file into a new buffer.
 * If the output format specifies more than one channel, all channels
 * will be combined into a single buffer.
 *
 * @param startFrame The starting audio frame to read data from.
 * @param numFrames The number of frames to read (-1 means read to the end of
 *                  the file).
 * @return The audio data or nil on error.
 */
- (NSMutableData*) audioDataWithStartFrame:(SInt64) startFrame
								 numFrames:(SInt64) numFrames;

/** Read audio data from this file into new buffer(s).
 *
 * @param startFrame The starting audio frame to read data from.
 * @param numFrames The number of frames to read (-1 means read to the end of
 *                  the file).
 * @param interleaved If NO, the data for individual channels specified in
 *                    the output format will be read into separate buffers.
 * @return An array of NSMutableData containing audio data.
 */
- (NSArray*) audioDataWithStartFrame:(SInt64) startFrame
						   numFrames:(SInt64) numFrames
						 interleaved:(bool) interleaved;

/** Notify an error if the specified ExtAudio error code indicates an error.
 * This will log the error and also potentially post an audio error notification
 * (OALAudioErrorNotification) if it is suspected that this error is a result of
 * the audio session getting corrupted.
 *
 * @param errorCode: The error code returned from an OS call.
 * @param function: The function name where the error occurred.
 * @param description: A printf-style description of what happened.
 */
+ (void) notifyExtAudioError:(OSStatus)errorCode
					function:(const char*) function
				 description:(NSString*) description, ...;

@end
