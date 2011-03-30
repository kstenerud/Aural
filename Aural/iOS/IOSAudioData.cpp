//
//  IOSAudioData.cpp
//  Aural
//
//  Created by Karl Stenerud on 3/28/11.
//  Copyright 2011 Stenerud. All rights reserved.
//

#include "IOSAudioData.h"

namespace aural
{
    IOSAudioData::IOSAudioData(void*const leftChannelData,
                               void*const rightChannelData,
                               const unsigned int numBytes,
                               AudioStreamBasicDescription& format)
    : leftChannelData_(leftChannelData)
    , rightChannelData_(rightChannelData)
    , numBytes_(numBytes)
    , format_(format)
    , numFrames_(numBytes / format.mBytesPerFrame)
    {
    }
    
    IOSAudioData::~IOSAudioData()
    {
        if(NULL != leftChannelData_)
        {
            free(leftChannelData_);
        }
        if(NULL != rightChannelData_)
        {
            free(rightChannelData_);
        }
    }
}
