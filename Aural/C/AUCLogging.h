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

#ifdef __cplusplus
extern "C" {
#endif

#pragma mark -
#pragma mark Basic Logging

#define AUC_LOG_ERROR(FMT, ...)   auc_log("Error",   __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)
#define AUC_LOG_WARNING(FMT, ...) auc_log("Warning", __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)
#define AUC_LOG_INFO(FMT, ...)    auc_log("Info",    __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)
#define AUC_LOG_DEBUG(FMT, ...)   auc_log("Debug",   __PRETTY_FUNCTION__, FMT, ##__VA_ARGS__)

void auc_log(const char* level,
			 const char* function,
			 const char* description,
			 ...);


#pragma mark -
#pragma mark AUGraph Error Logging

#define REPORT_AUGRAPH_ERROR(ERROR_CODE, FMT, ...) \
auc_notifyAUGraphError(ERROR_CODE, __PRETTY_FUNCTION__, (FMT), ##__VA_ARGS__)

void auc_notifyAUGraphError(OSStatus errorCode,
							const char* function,
							const char* description,
							...);


#pragma mark -
#pragma mark Audio Session Error Logging

#define REPORT_AUDIOSESSION_ERROR(ERROR_CODE, FMT, ...) \
auc_notifyAudioSessionError(ERROR_CODE, __PRETTY_FUNCTION__, (FMT), ##__VA_ARGS__)

void auc_notifyAudioSessionError(OSStatus errorCode,
								 const char* function,
								 const char* description,
								 ...);

#ifdef __cplusplus
}
#endif
