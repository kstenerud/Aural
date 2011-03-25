/*
 *  SoundTouchRenderEngine.h
 *  Aural
 *
 *  Created by Karl Stenerud on 3/11/11.
 *  Copyright 2011 xxx. All rights reserved.
 *
 */

#ifndef AURAL_SOUNDTOUCHRENDERENGINE_H
#define AURAL_SOUNDTOUCHRENDERENGINE_H

#include "AudioRenderEngine.h"
#include <stdlib.h>
#include <SoundTouch/SoundTouch.h>

namespace aural {

	class SoundTouchRenderEngine: public AudioRenderEngine
	{
	public:
		SoundTouchRenderEngine();
		~SoundTouchRenderEngine();

		virtual void renderTo(void*const buffer, const unsigned long numFrames);
		virtual void setPosition(const unsigned long framePosition);
		virtual void setBuffer(const void*const buffer, const unsigned long lengthInBytes);
		
		void setPitchSemiTones(float newPitch)
		{
			soundTouch_.setPitchSemiTones(newPitch);
		}

	private:
		void prerender(const unsigned long numFrames);

		soundtouch::SoundTouch soundTouch_;
		const void* frames_;
		unsigned long sizeInFrames_;
		unsigned long framePosition_;
	};
	
}

#endif /* AURAL_SOUNDTOUCHRENDERENGINE_H */
