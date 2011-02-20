//
//  AUCLogging.m
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

#include "AUCLogging.h"
#import <AudioToolbox/AudioToolbox.h>



void auc_log(const char* level,
			 const char* function,
			 const char* description,
			 ...)
{
	va_list args;
	va_start(args, description);
	NSString* descFormat = [[NSString alloc] initWithCString:description encoding:NSUTF8StringEncoding];
	NSString* desc = [[NSString alloc] initWithFormat:descFormat arguments:args];
	[descFormat release];
	va_end(args);
	
	NSLog(@"%s: %s: %@", level, function, desc);
	
	[desc release];
}


void auc_notifyAUGraphError(OSStatus errorCode,
							const char* function,
							const char* description,
							...)
{
	char* errorString;
	
	switch(errorCode)
	{
		case kAUGraphErr_NodeNotFound:
			errorString = "Node not found";
			break;
		case kAUGraphErr_InvalidConnection:
			errorString = "The attempted connection between two nodes cannot be made";
			break;
		case kAUGraphErr_OutputNodeErr:
			errorString = "Output Node Error";
			break;
		case kAUGraphErr_CannotDoInCurrentContext:
			errorString = "Cannot do in current context";
			break;
		case kAUGraphErr_InvalidAudioUnit:
			errorString = "Invalid audio unit";
			break;
		case noErr:
			return;
		default:
			errorString = "Unknown AUGraph error";
	}

	va_list args;
	va_start(args, description);
	NSString* descFormat = [[NSString alloc] initWithCString:description encoding:NSUTF8StringEncoding];
	NSString* desc = [[NSString alloc] initWithFormat:descFormat arguments:args];
	[descFormat release];
	va_end(args);

	NSLog(@"Error: %s: %@ (error code 0x%08x: %s)", function, desc, errorCode, errorString);

	[desc release];
}


void auc_notifyAudioSessionError(OSStatus errorCode,
								 const char* function,
								 const char* description,
								 ...)
{
	char* errorString;
	
	switch(errorCode)
	{
		case kAudioSessionNotInitialized:
			errorString = "Audio session not initialized";
			break;
		case kAudioSessionAlreadyInitialized:
			errorString = "Audio session already initialized";
			break;
		case kAudioSessionInitializationError:
			errorString = "Audio sesion initialization error";
			break;
		case kAudioSessionUnsupportedPropertyError:
			errorString = "Unsupported audio session property";
			break;
		case kAudioSessionBadPropertySizeError:
			errorString = "Bad audio session property size";
			break;
		case kAudioSessionNotActiveError:
			errorString = "Audio session is not active";
			break;
#if 0 // Documented but not implemented on iOS
		case kAudioSessionNoHardwareError:
			errorString = "Hardware not available for audio session";
			break;
#endif
#ifdef __IPHONE_3_1
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1
		case kAudioSessionNoCategorySet:
			errorString = "No audio session category set";
			break;
		case kAudioSessionIncompatibleCategory:
			errorString = "Incompatible audio session category";
			break;
#endif /* __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1 */
#endif /* __IPHONE_3_1 */
		case noErr:
			return;
		default:
			errorString = "Unknown audio session error";
	}
	
	va_list args;
	va_start(args, description);
	NSString* descFormat = [[NSString alloc] initWithCString:description encoding:NSUTF8StringEncoding];
	NSString* desc = [[NSString alloc] initWithFormat:descFormat arguments:args];
	[descFormat release];
	va_end(args);
	
	NSLog(@"Error: %s: %@ (error code 0x%08x: %s)", function, desc, errorCode, errorString);
	
	[desc release];
}
