//
//  Information.h
//  Aural
//
//  Created by Karl Stenerud on 3/27/11.
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

#ifndef AURAL_INFORMATION_H
#define AURAL_INFORMATION_H


#include <AudioToolbox/AudioToolbox.h>


namespace aural
{
    class AudioUnitInfo
    {
    public:  
        static const char* scopeName(const AudioUnitScope scope);
        static const char* errorName(const OSStatus errorCode);
        static const char* propertyName(const AudioUnitPropertyID property);
        
        // Note: Assumes 3D mixer
        static const char* parameterName(const AudioUnitParameterID paramter);
    };
    
    class AudioSessionInfo
    {
    public:  
        static const char* errorName(const OSStatus errorCode);
    };
}

#endif // AURAL_INFORMATION_H
