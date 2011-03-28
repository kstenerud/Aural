//
//  AUCLogging.h
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

#ifndef AURAL_LOGGING_H
#define AURAL_LOGGING_H

#ifdef __cplusplus
extern "C"
{
#endif


#pragma mark -
#pragma mark Basic Logging

#define AURAL_LOG_ERROR(FMT, ...)   aural_log("Error",   __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)
#define AURAL_LOG_WARNING(FMT, ...) aural_log("Warning", __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)
#define AURAL_LOG_INFO(FMT, ...)    aural_log("Info",    __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)
#define AURAL_LOG_DEBUG(FMT, ...)   aural_log("Debug",   __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)

    void aural_log(const char*const level,
                   const char*const function,
                   const char*const description,
                   ...);


#pragma mark -
#pragma mark AUGraph Error Logging

#define REPORT_AUDIOUNIT_ERROR(ERROR_CODE, FMT, ...) \
aural_notifyAudioUnitError(ERROR_CODE, __PRETTY_FUNCTION__, (FMT), ##__VA_ARGS__)

    void aural_notifyAudioUnitError(const OSStatus errorCode,
                                    const char*const function,
                                    const char*const description,
                                    ...);


#pragma mark -
#pragma mark Audio Session Error Logging

#define REPORT_AUDIOSESSION_ERROR(ERROR_CODE, FMT, ...) \
aural_notifyAudioSessionError(ERROR_CODE, __PRETTY_FUNCTION__, (FMT), ##__VA_ARGS__)

    void aural_notifyAudioSessionError(const OSStatus errorCode,
                                       const char*const function,
                                       const char*const description,
                                       ...);

#ifdef __cplusplus
}
#endif

#endif // AURAL_LOGGING_H
