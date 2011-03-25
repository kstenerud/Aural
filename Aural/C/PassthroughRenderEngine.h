/*
 *  PassthroughRenderEngine.h
 *  Aural
 *
 *  Created by Karl Stenerud on 3/11/11.
 *  Copyright 2011 xxx. All rights reserved.
 *
 */

#ifndef AURAL_PASSTHROUGHRENDERENGINE_H
#define AURAL_PASSTHROUGHRENDERENGINE_H

#include "AudioRenderEngine.h"

namespace aural {
	
	class PassthroughRenderEngine: public AudioRenderEngine
	{
	public:
		PassthroughRenderEngine();
		~PassthroughRenderEngine();
		
		virtual void renderTo(void*const buffer, const unsigned long numFrames);
		virtual void setPosition(const unsigned long framePosition);
		virtual void setBuffer(const void*const buffer, const unsigned long lengthInBytes);
		
		void setPitchSemiTones(float newPitch)
		{
		}
		
	private:
		void prerender(const unsigned long numFrames);
		
		const unsigned char* bytes_;
		unsigned long sizeInBytes_;
		unsigned long bytePosition_;
	};
	
}

#endif /* AURAL_PASSTHROUGHRENDERENGINE_H */
