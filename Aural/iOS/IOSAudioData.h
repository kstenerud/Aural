//
//  IOSAudioData.h
//  Aural
//
//  Created by Karl Stenerud on 3/28/11.
//  Copyright 2011 Stenerud. All rights reserved.
//

#ifndef AURAL_IOSAUDIODATA_H
#define AURAL_IOSAUDIODATA_H

#include "AudioData.h"
#include <AudioToolbox/AudioToolbox.h>

// TODO: IOSAudioDataSoftwareStream, IOSAudioDataHardwareStream

namespace aural
{
    class IOSAudioData: public AudioData
    {
    public:
        IOSAudioData(void*const leftChannelData,
                     void*const rightChannelData,
                     const unsigned int numBytes,
                     AudioStreamBasicDescription& format);
        ~IOSAudioData();
        
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


#endif // AURAL_IOSAUDIODATA_H
