/*
 *  RenderBuffer.cpp
 *  Aural
 *
 *  Created by Karl Stenerud on 3/10/11.
 *  Copyright 2011 xxx. All rights reserved.
 *
 */

/*
#include "RenderBuffer.h"
#include <memory.h>

namespace aural {
	
	void RenderBuffer::injectBytes(const void*const src, const unsigned long numBytes)
{
	unsigned long bytesToCopy = (headOffset_ + numBytes) & mask_;
	memcpy(data_ + headOffset_, src, bytesToCopy);
	
	headOffset_ = (headOffset_ + bytesToCopy) & mask_;
	unsigned long srcOffset = bytesToCopy;
	bytesToCopy = numBytes - srcOffset;
	
	if(bytesToCopy > 0)
	{
		memcpy(data_+headOffset_, (const unsigned char*const)src+srcOffset, bytesToCopy);
		headOffset_ = (headOffset_ + bytesToCopy) & mask_;
	}
}

	void RenderBuffer::extractBytes(void* dst, const unsigned long numBytes)
{
	unsigned long bytesToCopy = (tailOffset_ + numBytes) & mask_;
	memcpy(dst, data_+tailOffset_, bytesToCopy);
	
	tailOffset_ = (tailOffset_ + bytesToCopy) & mask_;
	unsigned long dstOffset = bytesToCopy;
	bytesToCopy = numBytes - dstOffset;
	
	if(bytesToCopy > 0)
	{
		memcpy((unsigned char*)dst+dstOffset, data_+tailOffset_, bytesToCopy);
		tailOffset_ = (tailOffset_ + bytesToCopy) & mask_;
	}
}

}
*/

#include "RenderBuffer.h"
#include <memory.h>

namespace aural {

	void RenderBuffer::injectBytes(const void*const src, const unsigned long numBytes)
	{
		unsigned long srcOffset = 0;
		unsigned long bytesToCopy = numBytes;
		if(headOffset_+bytesToCopy >= sizeInBytes_)
		{
			bytesToCopy = sizeInBytes_ - headOffset_;
			memcpy(data_+headOffset_, src, bytesToCopy);
			srcOffset = bytesToCopy;
			headOffset_ = 0;
			bytesToCopy = numBytes - srcOffset;
		}
		
		if(bytesToCopy > 0)
		{
			memcpy(data_+headOffset_, (const unsigned char*const)src+srcOffset, bytesToCopy);
			headOffset_ += bytesToCopy;
		}
	}
	
	void RenderBuffer::extractBytes(void* dst, const unsigned long numBytes)
	{
		unsigned long dstOffset = 0;
		unsigned long bytesToCopy = numBytes;
		if(tailOffset_+bytesToCopy >= sizeInBytes_)
		{
			bytesToCopy = sizeInBytes_ - tailOffset_;
			memcpy(dst, data_+tailOffset_, bytesToCopy);
			dstOffset = bytesToCopy;
			tailOffset_ = 0;
			bytesToCopy = numBytes - dstOffset;
		}
		
		if(bytesToCopy > 0)
		{
			memcpy((unsigned char*)dst+dstOffset, data_+tailOffset_, bytesToCopy);
			tailOffset_ += bytesToCopy;
		}
	}
}
