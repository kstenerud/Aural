//
//  ObjectALConfig.h
//  ObjectAL
//
//  Created by Karl Stenerud on 10-08-02.
//
// Copyright 2009 Karl Stenerud
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


/* Compile-time configuration for ObjectAL.
 *
 * The defines in this file provide broad guidelines for how ObjectAL will behave
 * in your application.  They can either be set here, or you can set them as user
 * defines in your build configuration.
 */



/** Resets the audio session when an error occurs that may have been caused by
 * a messed up session.
 *
 * In iOS 4.2, there are situations where an underlying sound system such as
 * mediaserverd will crash, putting the audio session in an uncertain state.
 * If this switch is enabled, OALAudioSession will reset itself when certain
 * kinds of audio errors occur.
 *
 * Recommended setting: 1 for release, possibly 0 during development.
 */
#ifndef OBJECTAL_CFG_RESET_AUDIO_SESSION_ON_ERROR
#define OBJECTAL_CFG_RESET_AUDIO_SESSION_ON_ERROR 1
#endif


/** When this option is enabled, all critical ObjectAL operations will be wrapped in
 * synchronized blocks. <br>
 *
 * Turning this off can improve performance a bit if your application makes heavy
 * use of audio calls, but you'll be on your own for ensuring two threads don't
 * access the same part of the audio library at the same time. <br>
 *
 * Recommended setting: 1
 */
#ifndef OBJECTAL_CFG_SYNCHRONIZED_OPERATIONS
#define OBJECTAL_CFG_SYNCHRONIZED_OPERATIONS 1
#endif


/** When this option is other than LEVEL_NONE, ObjectAL will output log entries that correspond
 * to the LEVEL:
 *
 * LEVEL_NONE:    No output
 * LEVEL_ERROR:   Errors only
 * LEVEL_WARNING: Errors, Warnings
 * LEVEL_INFO:    Errors, Warnings, Info
 * LEVEL_DEBUG:   Errors, Warnings, Info, Debug
 *
 * Setting this to LEVEL_NONE will cause most internal functions to not bother checking error codes.
 *
 * Recommended setting: LEVEL_WARNING
 */
#ifndef OBJECTAL_CFG_LOG_LEVEL
#define OBJECTAL_CFG_LOG_LEVEL LEVEL_WARNING
#endif
