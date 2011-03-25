/*
 *  SoundTouchRenderEngine.cpp
 *  Aural
 *
 *  Created by Karl Stenerud on 3/11/11.
 *  Copyright 2011 xxx. All rights reserved.
 *
 */

#include "SoundTouchRenderEngine.h"

static const unsigned int kPreloadFrames = 200;

namespace aural {
	
	using namespace soundtouch;

	SoundTouchRenderEngine::SoundTouchRenderEngine()
	: frames_(NULL)
	, sizeInFrames_(0)
	, framePosition_(0)
	{
		int sampleRate = 44100;
		int channels = 1;
		
		soundTouch_.setSampleRate(sampleRate);
		soundTouch_.setChannels(channels);
		
		soundTouch_.setTempoChange(0.0f);
		soundTouch_.setPitchSemiTones(0.0f);
		soundTouch_.setRateChange(0.0f);
		
		soundTouch_.setSetting(SETTING_USE_QUICKSEEK, TRUE);
		soundTouch_.setSetting(SETTING_USE_AA_FILTER, FALSE);
		
		if (0)
		{
			// use settings for speech processing
			soundTouch_.setSetting(SETTING_SEQUENCE_MS, 40);
			soundTouch_.setSetting(SETTING_SEEKWINDOW_MS, 15);
			soundTouch_.setSetting(SETTING_OVERLAP_MS, 8);
		}
	}
	
	SoundTouchRenderEngine::~SoundTouchRenderEngine()
	{
	}
	
	void SoundTouchRenderEngine::prerender(const unsigned long numFrames)
	{
		SAMPLETYPE* src = (SAMPLETYPE*)frames_;

		unsigned long framesToCopy = numFrames;
		if(framePosition_+framesToCopy >= sizeInFrames_)
		{
			framesToCopy = sizeInFrames_ - framePosition_;
			soundTouch_.putSamples(src+framePosition_, framesToCopy);
			framesToCopy = numFrames - framesToCopy;
			framePosition_ = 0;
		}
		
		if(framesToCopy > 0)
		{
			soundTouch_.putSamples(src+framePosition_, framesToCopy);
			framePosition_ += framesToCopy;
		}
	}

	void SoundTouchRenderEngine::renderTo(void*const buffer, const unsigned long numFrames)
	{
		prerender(numFrames);
		
		int remainingFrames = numFrames;
		int framesRead = 0;
		int dstPos = 0;
		SAMPLETYPE* dst = (SAMPLETYPE*)buffer;
		
		do 
		{
			framesRead = soundTouch_.receiveSamples(dst+dstPos, remainingFrames);
			dstPos += framesRead;
			remainingFrames -= framesRead;
		} while (framesRead != 0);
	}

	void SoundTouchRenderEngine::setPosition(const unsigned long framePosition)
	{
		framePosition_ = framePosition;
		soundTouch_.clear();
		prerender(kPreloadFrames);
	}
	
	void SoundTouchRenderEngine::setBuffer(const void*const buffer, unsigned long lengthInBytes)
	{
		frames_ = buffer;
		sizeInFrames_ = lengthInBytes/sizeof(SAMPLETYPE);
		prerender(kPreloadFrames);
	}
}
