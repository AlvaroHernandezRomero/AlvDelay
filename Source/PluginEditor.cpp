/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AlvDelayAudioProcessorEditor::AlvDelayAudioProcessorEditor (AlvDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200); //tamaño de la pantalla del plugin ancho y alto

    addAndMakeVisible(&feedback); //hace que el elemento en este caso el slider para el feedback sea visible
    feedback.setSliderStyle(Slider::SliderStyle::Rotary); //tipo de slider
    feedback.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20); //colocacion del texto de solo lectura respecto al slider y tamaño (ancho y alto)
    feedback.onValueChange = [this] {feedback.getValue(); };
    processor.fb = (float)feedback.getValue();
    feedback.setColour(Slider::rotarySliderFillColourId, Colours::white);
    feedback.setColour(Slider::textBoxBackgroundColourId, Colours::white);
    feedback.setColour(Slider::textBoxTextColourId, Colours::black);
    feedback.addListener(this);
    sld_fb = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parametros, "feedback", feedback);//guarda el valor del slider feedback nos da los rangos definidos en el procesador

    addAndMakeVisible(&feedback_label);
    feedback_label.setText("Feedback",dontSendNotification); //identificador para el slider de feedback
    feedback_label.attachToComponent(&feedback, false); // a que componente va ligada la etiqueta slider,boton... y su colocacion con true se colocaria en la izquierda y con false arriba del elemento

    addAndMakeVisible(&delay);

    delay.setSliderStyle(Slider::SliderStyle::Rotary);
    delay.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    delay.setTextValueSuffix(" ms");
    delay.onValueChange = [this] {delay.getValue(); };
    processor.dly = (float)delay.getValue();
    delay.setColour(Slider::rotarySliderFillColourId, Colours::white);
    delay.setColour(Slider::textBoxBackgroundColourId, Colours::white);
    delay.setColour(Slider::textBoxTextColourId, Colours::black);
    delay.addListener(this);
    sld_delay = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parametros, "delay", delay); //guarda el valor del slider delay ademas carga el valor que se guarda 

    addAndMakeVisible(&delay_label);
    delay_label.setText("Delay", dontSendNotification);
    delay_label.attachToComponent(&delay, false);

    addAndMakeVisible(&etiqueta);
    etiqueta.addItem("Normal", 1);
    etiqueta.addItem("Ping-Pong", 2);
    etiqueta.setColour(ComboBox::backgroundColourId, Colours::black);
    etiqueta.setColour(ComboBox::textColourId, Colours::white);
    etiqueta.setSelectedId(1);
    etiqueta.addListener(this);
    etiqueta_estado = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parametros, "combobox", etiqueta);
    startTimer(50);
}

AlvDelayAudioProcessorEditor::~AlvDelayAudioProcessorEditor()
{
}

//==============================================================================
void AlvDelayAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black); //color del fondo 

    g.setColour (Colours::white);//color de las letras
    g.setFont (30.0f); // tamaño de las letras 
    g.drawFittedText ("ALVDELAY", getLocalBounds(), Justification::centredTop, 1); //nombre que aparece en pantalla y colocacion 
}

void AlvDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    feedback.setBounds(50, 50, 100, 100); //colocacion del slider de feedback y tamaño x,y,ancho,alto
    delay.setBounds(250, 50, 100, 100);
    etiqueta.setBounds(150, 50, 100, 50);
}

void AlvDelayAudioProcessorEditor::sliderValueChanged(Slider* slider) 
{
    if (slider == &feedback) {
        feedback.setValue(feedback.getValue(), dontSendNotification);
        processor.fb = (float)feedback.getValue();
    }else if (slider == &delay) {
        delay.setValue(delay.getValue(), dontSendNotification);
        processor.dly = (float)delay.getValue();
    }
}
void AlvDelayAudioProcessorEditor::comboBoxChanged(ComboBox* box)
{
    processor.menu = box->getSelectedId();
}
void AlvDelayAudioProcessorEditor::timerCallback() {
    feedback.setValue(processor.fb, dontSendNotification);
    delay.setValue(processor.dly, dontSendNotification);
}