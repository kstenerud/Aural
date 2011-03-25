/*
 *  PassthroughRenderEngine.cpp
 *  Aural
 *
 *  Created by Karl Stenerud on 3/11/11.
 *  Copyright 2011 xxx. All rights reserved.
 *
 */

#include "PassthroughRenderEngine.h"
#include <memory.h>

#define kSampleType short

namespace aural {
	
	PassthroughRenderEngine::PassthroughRenderEngine()
	: bytes_(NULL)
	, sizeInBytes_(0)
	, bytePosition_(0)
	{
	}
	
	PassthroughRenderEngine::~PassthroughRenderEngine()
	{
	}
	
	void PassthroughRenderEngine::renderTo(void*const buffer, const unsigned long numFrames)
	{
		unsigned long dstOffset = 0;
		unsigned long numBytes = numFrames * sizeof(kSampleType);
		unsigned long bytesToCopy = numBytes;
		unsigned char* dst = static_cast<unsigned char*>(buffer);

		if(bytePosition_+bytesToCopy >= sizeInBytes_)
		{
			bytesToCopy = sizeInBytes_ - bytePosition_;
			memcpy(dst, bytes_+bytePosition_, bytesToCopy);
			dstOffset = bytesToCopy;
			bytePosition_ = 0;
			bytesToCopy = numBytes - dstOffset;
		}
		
		if(bytesToCopy > 0)
		{
			memcpy(dst+dstOffset, bytes_+bytePosition_, bytesToCopy);
			bytePosition_ += bytesToCopy;
		}
	}
	
	void PassthroughRenderEngine::setPosition(const unsigned long framePosition)
	{
		bytePosition_ = framePosition * sizeof(kSampleType);
	}
	
	void PassthroughRenderEngine::setBuffer(const void*const buffer, unsigned long lengthInBytes)
	{
		bytes_ = static_cast<const unsigned char*>(buffer);
		sizeInBytes_ = lengthInBytes;
	}
}
