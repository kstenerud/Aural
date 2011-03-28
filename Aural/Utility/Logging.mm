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

#include "Logging.h"
#import <AudioToolbox/AudioToolbox.h>
#import "Information.h"


void aural_log(const char* level,
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


void aural_notifyAudioUnitError(OSStatus errorCode,
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
    
    const char* errorString = aural::AudioUnitInfo::errorName(errorCode);

	NSLog(@"Error: %s: %@ (error code %ld: %s)", function, desc, errorCode, errorString);

	[desc release];
}


void aural_notifyAudioSessionError(OSStatus errorCode,
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

    const char* errorString = aural::AudioSessionInfo::errorName(errorCode);

	NSLog(@"Error: %s: %@ (error code 0x%08ld: %s)", function, desc, errorCode, errorString);
	
	[desc release];
}
