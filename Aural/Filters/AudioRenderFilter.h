//
//  RenderFilter.h
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

#ifndef AURAL_RENDERFILTER_H
#define AURAL_RENDERFILTER_H


#include "AudioBuffer.h"
#include "Util.h"


namespace aural
{
    class AudioRenderSourceSinkCommonInterface
    {
    public:
		virtual ~AudioRenderSourceSinkCommonInterface() {}
        virtual void reset() = 0;
        virtual void prepare() = 0;
    };
    
    class AudioRenderSource: public virtual AudioRenderSourceSinkCommonInterface
    {
	public:
		virtual ~AudioRenderSource() {}
        virtual void readFrames(const unsigned long numFrames, void*const dst) = 0;
        virtual void skipFrames(const unsigned long numFrames) = 0;
    };

    
    class AudioRenderSink: public virtual AudioRenderSourceSinkCommonInterface
    {
	public:
		virtual ~AudioRenderSink() {}
        virtual void setSource(AudioRenderSource*const source) = 0;
        virtual bool enabled() = 0;
        virtual void setEnabled(const bool value) = 0;
    };
    
    
    class AudioRenderFilter: public AudioRenderSource, public AudioRenderSink
    {
        
    };
    

    class BasicAudioRenderSink: public AudioRenderSink
    {
    public:
        virtual void setSource(AudioRenderSource*const source);
        virtual void reset();
        virtual void prepare();
        
    protected:
        void sourceReadFrames(const unsigned long numFrames, void*const dst);
        void sourceSkipFrames(const unsigned long numFrames);
        
    private:
        AudioRenderSource* source_;
    };
    
    class BasicAudioRenderFilter: public AudioRenderFilter
    {
    public:
        virtual void setSource(AudioRenderSource*const source);
        virtual void reset();
        virtual void prepare();
        
    protected:
        void sourceReadFrames(const unsigned long numFrames, void*const dst);
        void sourceSkipFrames(const unsigned long numFrames);
        
    private:
        AudioRenderSource* source_;
    };
    
    
    class MonoBufferRenderSource: public AudioRenderSource
    {
    public:
        void setBuffer(AudioBuffer*const buffer);
        unsigned long position();
        void setPosition(const unsigned long frameNumber);
        void reset();
        void prepare();
        void readFrames(const unsigned long numFrames, void*const dst);
        void skipFrames(const unsigned long numFrames);

    private:
        AudioBuffer* audioBuffer_;
        unsigned long frameSize_;
        unsigned long numFrames_;
        unsigned long position_;
    };
    
    
    
    
    inline void BasicAudioRenderSink::reset()
    {
        source_->reset();
    }
    
    inline void BasicAudioRenderSink::prepare()
    {
        source_->prepare();
    }
    
    inline void BasicAudioRenderSink::setSource(AudioRenderSource*const source)
    {
        source_ = source;
    }
    
    inline void BasicAudioRenderSink::sourceReadFrames(const unsigned long numFrames, void*const dst)
    {
        source_->readFrames(numFrames, dst);
    }
    
    inline void BasicAudioRenderSink::sourceSkipFrames(const unsigned long numFrames)
    {
        source_->skipFrames(numFrames);
    }
    

    inline void BasicAudioRenderFilter::reset()
    {
        source_->reset();
    }
    
    inline void BasicAudioRenderFilter::prepare()
    {
        source_->prepare();
    }
    
    inline void BasicAudioRenderFilter::setSource(AudioRenderSource*const source)
    {
        source_ = source;
    }
    
    inline void BasicAudioRenderFilter::sourceReadFrames(const unsigned long numFrames, void*const dst)
    {
        source_->readFrames(numFrames, dst);
    }
    
    inline void BasicAudioRenderFilter::sourceSkipFrames(const unsigned long numFrames)
    {
        source_->skipFrames(numFrames);
    }
    


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

#endif // AURAL_RENDERFILTER_H
