//
//  Util.cpp
//  Aural
//
//  Created by Karl Stenerud on 3/26/11.
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


#include "Util.h"


namespace aural
{
    size_t copy_circular(void*const dst,
                         const size_t numBytesToCopy,
                         const void*const src,
                         const size_t srcOffset,
                         const size_t srcLength)
    {
        size_t dstPosition = 0;
        size_t srcPosition = srcOffset;
        size_t segmentLength = numBytesToCopy;
        const unsigned char*const srcBytes = static_cast<const unsigned char*const>(src);
        unsigned char*const dstBytes = static_cast<unsigned char*const>(dst);
        
        while(srcPosition+segmentLength >= srcLength)
        {
            segmentLength = srcLength - srcPosition;
            memcpy(dstBytes+dstPosition, srcBytes+srcPosition, segmentLength);
            dstPosition += segmentLength;
            srcPosition = 0;
            segmentLength = numBytesToCopy - dstPosition;
        }
        
        if(segmentLength > 0)
        {
            memcpy(dstBytes+dstPosition, srcBytes+srcPosition, segmentLength);
            srcPosition += segmentLength;
        }
        
        return srcPosition;
    }
    
    size_t advance_circular(const size_t numBytesToAdvance,
                            const size_t srcOffset,
                            const size_t srcLength)
    {
        return (srcOffset + numBytesToAdvance) % srcLength;
    }
}
