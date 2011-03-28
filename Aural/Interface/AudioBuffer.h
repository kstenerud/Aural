//
//  AUCAudioBuffer.h
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

#ifndef AURAL_AUDIOBUFFER_H
#define AURAL_AUDIOBUFFER_H


#include <AudioToolbox/AudioToolbox.h>


namespace aural
{
    class AudioBuffer
    {
    public:
        AudioBuffer(void* leftChannelData,
                    void* rightChannelData,
                    unsigned int numBytes,
                    AudioStreamBasicDescription& format);
        ~AudioBuffer();
        
        void* leftChannelData() {return leftChannelData_;}
        void* rightChannelData() {return rightChannelData_;}
        const unsigned int& numFrames() {return numFrames_;}
        const unsigned int& numBytes() {return numBytes_;}
        const AudioStreamBasicDescription& format() {return format_;}
        
    private:
        void* leftChannelData_;
        void* rightChannelData_;
        unsigned int numFrames_;
        unsigned int numBytes_;
        AudioStreamBasicDescription format_;
        
        /*
         Float64 mSampleRate;
         UInt32  mFormatID;
         UInt32  mFormatFlags;
         UInt32  mBytesPerPacket;
         UInt32  mFramesPerPacket;
         UInt32  mBytesPerFrame;
         UInt32  mChannelsPerFrame;
         UInt32  mBitsPerChannel;
         */
    };
}

#endif // AURAL_AUDIOBUFFER_H
