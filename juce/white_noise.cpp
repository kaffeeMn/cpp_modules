#include "juce-huckleberry-osx/extras/Projucer/JuceLibraryCode/JuceHeader.h"
#include <stdexcept>

class WhiteNoiseComponent : public AudioAppComponent{

    public:
    
        WhiteNoiseComponent(x,y,m,n){
            // size of widget
            setSize(x,y);
            // channel matrix
            setAudioChannels(m,n);
            this.random = new Random();
            // default volume at half of the maximum output
            this.volume_ctl = 0.5f;
        }
        ~WhiteNoiseComponent(){
            // managing garbage collection
            this.volume_ctl = 0f
            del this.volume_ctl;
            del this.random;
            shoutdownAudio();
        }

        void set_volume(float& new_vol){
            /*
             *  This mehtod sets the maximum volume of the white noise.
             *  Using a refernce to overcome constant updates and for efficient 
             *  memory allocation.
             *
             *  :param new_vol: reference to float number, e.g. numeric value of
             *                  a knobs position
             */
            if(new_vol > 1. || new_vol < 0.){
                throw :: invalid_argument(
                    "received value outside of [0,1] for volume control"
                );
            }else{
                this.volume_ctl = new_vol;
            }
        }

        void getNextAudioBlock(const AudioSourceChannelInfo& ac_buffer)
        override{
        /*
         *  This method generates the next audio-block for the audio output.
         *  Due to this component being white noise, the buffer of the source 
         *  will be filled with random values
         *
         * :param ac_buffer: audio source reference, containing the buffer to 
         *                   fill
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
                    buffer[i] = (
                        (this.random.nextFloat() * 2f - 1f) * this.volume_ctl
                    );
                }
            }
        }

    private:

        // random number factory
        Random random;
        // variable for volume control
        float volume_ctl;
}
