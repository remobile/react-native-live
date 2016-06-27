/*

 Video Core
 Copyright (c) 2014 James G. Hurley

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */
#include "MicSource.h"
#include <videocore/mixers/IAudioMixer.hpp>

namespace videocore { namespace Android {

    MicSource::MicSource(double sampleRate, int channelCount)
    : m_sampleRate(sampleRate), m_channelCount(channelCount)
    {
    }
    MicSource::~MicSource() {
    }
    void
    MicSource::inputCallback(uint8_t *data, size_t data_size, int inNumberFrames)
    {
        auto output = m_output.lock();
        if(output) {
            videocore::AudioBufferMetadata md (0.);
            
            md.setData(m_sampleRate,
                       16,
                       m_channelCount,
                       kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked,
                       m_channelCount * 2,
                       inNumberFrames,
                       false,
                       false,
                       shared_from_this());
            
            output->pushBuffer(data, data_size, md);
        }
    }
   
    void
    MicSource::setOutput(std::shared_ptr<IOutput> output) {
        m_output = output;
        auto mixer = std::dynamic_pointer_cast<IAudioMixer>(output);
        mixer->registerSource(shared_from_this());
    }
}
}
