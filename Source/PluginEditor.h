/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AlvDelayAudioProcessorEditor  : public AudioProcessorEditor , private Slider::Listener, public Timer ,private ComboBox::Listener
{
public:
    AlvDelayAudioProcessorEditor (AlvDelayAudioProcessor&);
    ~AlvDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback();
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    void sliderValueChanged(Slider* slider) override;
    void comboBoxChanged(ComboBox* box) override;
    AlvDelayAudioProcessor& processor; //Nos sirve para comunicar la interfaz grafica con el procesador
    Slider feedback, delay; //creamos las variables de tipo slider para crear una interfaz grafica
    Label feedback_label, delay_label; // seran etiquetas para poner el nombre del parametro que maneja cada slider
    ComboBox etiqueta;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sld_fb; //creamos un sliderattachment que nos sirve para comunicarnos con un value tree state de maneras que podamos guardar el estado de nuestro slide
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sld_delay;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> etiqueta_estado;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlvDelayAudioProcessorEditor)
};
