#include "juce-huckleberry-osx/extras/Projucer/JuceLibraryCode/JuceHeader.h"
#include <stdexcept>

class WhiteNoiseComponent : public AudioAppComponent{

    public:
    
    WhiteNoiseComponent(){
        setSize(500,500);
        setAudioChannels(2,2);
        this.random = new Random();
        this.volume_ctl = 0.25f;
    }
    ~WhiteNoiseComponent(){
        shoutdownAudio();
    }

    void change_volume(float new_vol){
        if(new_vol > 1 || new_vol < 0){
            throw :: invalid_argument("received value outside of [0,1] for volume control");
        }else{
            this.volume_ctl = new_vol;
        }
    }

    void getNextAudioBlock(const AudioSourceChannelInfo& ac_buffer) override{
    /*
    * :param ac_buffer: reference to the buffer to fill
    */

        for(int c=0; c < ac_buffer.buffer->getNumChannels(); ++c){
            // each channel has its own buffer
            float* const buffer = ac_buffer.buffer->getWritePointer(c, ac_buffer.startSample);
            for(int i=0; i < ac_buffer.numSamples; ++i){
                // each buffer needs to be filled with random values for white noise
                buffer[i] = (this.random.nextFloat() * 2f - 1f) * this.volume_ctl;
            }
        }
    }

    private:

    // random number generator
    Random random;
    // variable for volume control
    float volume_ctl;
}
