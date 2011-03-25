/*
 *  AudioRenderEngine.h
 *  Aural
 *
 *  Created by Karl Stenerud on 3/10/11.
 *  Copyright 2011 xxx. All rights reserved.
 *
 */

#ifndef AURAL_AUDIORENGERENGINE_H
#define AURAL_AUDIORENGERENGINE_H

namespace aural {

	class AudioRenderEngine
	{
	public:
		virtual ~AudioRenderEngine() {}
		virtual void renderTo(void*const buffer, const unsigned long numFrames) = 0;
		virtual void setPosition(const unsigned long framePosition) = 0;
		virtual void setBuffer(const void*const buffer, const unsigned long lengthInBytes) = 0;
	};
}

#endif /* AURAL_AUDIORENGERENGINE_H */
