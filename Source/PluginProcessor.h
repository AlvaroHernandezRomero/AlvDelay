/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class AlvDelayAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AlvDelayAudioProcessor();
    ~AlvDelayAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    AudioProcessorValueTreeState::ParameterLayout createParameters(); //creamos esta funcion para inicializar parametros y vincularlos con los sliders
    AudioProcessorValueTreeState parametros; //esta variable servira para guardar el estado de nuestros parametros
    float fb, dly;//estas variables serviran para guardar en el procesador el valor de los sliders
    int menu;
private:
    //==============================================================================
    int length_buffer;
    int readdelayL, readdelayR, writedelayL, writedelayR; //esta variable nos indicara el tamaño del buffer
    float* delay_bufferL;
    float* delay_bufferR;
    double samplerate;
    int pingpongL, pingpongR;
    int menu_ant;
    AudioSampleBuffer retardo; //este sera el buffer que se encargara de sumar las muestras retardadas
    void normal(AudioBuffer<float>&, int chan);
    void pingpong(AudioBuffer<float>&, int chan);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlvDelayAudioProcessor)
};
