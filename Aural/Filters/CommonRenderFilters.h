//
//  CommonRenderFilters.h
//  Aural
//
//  Created by Karl Stenerud on 3/27/11.
//

#ifndef AURAL_COMMONRENDERFILTERS_H
#define AURAL_COMMONRENDERFILTERS_H


#include "AudioRenderFilter.h"


namespace aural
{
    class MonoBufferRenderSource: public AudioRenderSource
    {
    public:
        void setBuffer(AudioBuffer*const buffer);
        unsigned long position();
        void setPosition(const unsigned long frameNumber);

        void reset();
        void prepare();
        const unsigned long frameSize();
        void readFrames(const unsigned long numFrames, void*const dst);
        void skipFrames(const unsigned long numFrames);
        
    private:
        AudioBuffer* audioBuffer_;
        unsigned long frameSize_;
        unsigned long numFrames_;
        unsigned long position_;
    };


    inline void MonoBufferRenderSource::setBuffer(AudioBuffer*const buffer)
    {
        audioBuffer_ = buffer;
        frameSize_ = 2;
        numFrames_ = audioBuffer_->numFrames();
        
        setPosition(0);
    }
    
    inline unsigned long MonoBufferRenderSource::position()
    {
        return position_;
    }
    
    inline void MonoBufferRenderSource::setPosition(const unsigned long frameNumber)
    {
        position_ = frameNumber;
    }
    
    inline void MonoBufferRenderSource::reset()
    {
        setPosition(0);
    }
    
    inline void MonoBufferRenderSource::prepare()
    {
        // Nothing to do
    }

    inline const unsigned long MonoBufferRenderSource::frameSize()
    {
        return frameSize_;
    }

    inline void MonoBufferRenderSource::readFrames(const unsigned long numFrames, void*const dst)
    {
        size_t newPosition = copy_circular(dst,
                                           numFrames * frameSize_,
                                           audioBuffer_->leftChannelData(),
                                           position_ * frameSize_,
                                           audioBuffer_->numBytes());
        
        position_ = newPosition / frameSize_;
    }
    
    inline void MonoBufferRenderSource::skipFrames(const unsigned long numFrames)
    {
        size_t newPosition = advance_circular(numFrames * frameSize_,
                                              position_ * frameSize_,
                                              audioBuffer_->numBytes());
        
        position_ = newPosition / frameSize_;
    }
}

#endif // AURAL_COMMONRENDERFILTERS_H
