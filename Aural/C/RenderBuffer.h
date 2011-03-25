/*
 *  RenderBuffer.h
 *  Aural
 *
 *  Created by Karl Stenerud on 3/10/11.
 *  Copyright 2011 xxx. All rights reserved.
 *
 */

#include <memory.h>

namespace aural {

	class RenderBuffer
	{
	public:
		RenderBuffer(unsigned long sizeInBytes)
		: sizeInBytes_(sizeInBytes)
		, mask_(sizeInBytes-1)
		, data_(new unsigned char[sizeInBytes])
		, headOffset_(0)
		, tailOffset_(0)
		{
			memset(data_, 0, sizeInBytes_);
		}
		
		~RenderBuffer()
		{
			delete [] data_;
		}

		void injectBytes(const void*const src, const unsigned long numBytes);
		
		void extractBytes(void* dst, const unsigned long numBytes);
		
		long bytesRemaining()
		{
			return (headOffset_+sizeInBytes_-tailOffset_) & mask_;
		}
		
	private:
		unsigned long sizeInBytes_;
		unsigned long mask_;
		unsigned char* data_;
		unsigned long headOffset_;
		unsigned long tailOffset_;
	};
}
