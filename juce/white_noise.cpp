// #include "juce-huckleberry-osx/extras/Projucer/JuceLibraryCode/JuceHeader.h"
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class WhiteNoiseComponent : public AudioAppComponent{

    public:
    
        WhiteNoiseComponent(int x,int y, int m, int n){
            // size of widget
            setSize(x,y);
            // channel matrix
            setAudioChannels(m,n);
            // setting up the random number factory
            this->random = new Random();
            // default volume at half of the maximum output
            this->volume_ctl = new float(0.5f);
        }
        ~WhiteNoiseComponent(){
            this->random = NULL;
            this->volume_ctl = NULL;
            delete this->random;
            delete this->volume_ctl;
            shoutdownAudio();
        }

        void set_volume(float *new_vol){
            /*
             *  This mehtod sets the maximum volume of the white noise.
             *
             *  :param new_vol: adress of float number, e.g. numeric value of
             *                  a knobs position
             */
            if(*new_vol > 1. || *new_vol < 0.){
                throw :: invalid_argument(
                    "received value outside of [0,1] for volume control"
                );
            }else{
                this->volume_ctl = new_vol;
            }
        }

        void getNextAudioBlock(const AudioSourceChannelInfo& ac_buffer)
        override{
            /*
             *  This method generates the next audio-block for the audio output.
             *  Due to this component being white noise, the buffer of the 
             *  source will be filled with random values
             *
             * :param ac_buffer: audio source reference, containing the buffer 
             *                   to fill
             */
            for(int c=0; c < ac_buffer.buffer->getNumChannels(); ++c){
                // each channel has its own buffer
                float* const buffer = ac_buffer.buffer->getWritePointer(
                    c, ac_buffer.startSample
                );
                for(int i=0; i < ac_buffer.numSamples; ++i){
                    /* each buffer needs to be filled with random values 
                     * (in [0,1]) for white noise
                     */
                    buffer[i] = (float) (
                        (this->random.nextFloat() * 2.f - 1.f) * *(this->volume_ctl)
                    );
                }
            }
        }

    private:

        // random number factory
        Random* random;
        // variable for volume control
        float* volume_ctl;
};
