/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AlvDelayAudioProcessor::AlvDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), parametros(*this, nullptr, "EstadoSliders", createParameters())
#endif
{
    parametros.state = ValueTree("EstadoSliders");
    fb = 0;
    dly = 0;
    length_buffer = 0;
    readdelayL = 0;
    readdelayR = 0;
    writedelayL = 0;
    writedelayR = 0;
    menu = 1;
    menu_ant = menu;
    pingpongL = 1;
    pingpongR = 0;
}

AlvDelayAudioProcessor::~AlvDelayAudioProcessor()
{
}

//==============================================================================
const String AlvDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AlvDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AlvDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AlvDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AlvDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AlvDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AlvDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AlvDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String AlvDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void AlvDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AlvDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    samplerate = sampleRate;
    retardo.setSize(2, 3 * (int)samplerate); //Damos numero de canales que tendra el buffer (L y R) y tamaño al buffer
    retardo.clear(); // inicializa el buffer
    delay_bufferL = retardo.getWritePointer(0);
    delay_bufferR = retardo.getWritePointer(1);
    pingpongL = 1;
    pingpongR = 0;
}

void AlvDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AlvDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AlvDelayAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    if (length_buffer != (int)(samplerate * (dly / 1000))) {
        readdelayL = 0;
        readdelayR = 0;
        writedelayL = 0;
        writedelayR = 0;
        pingpongL = 1;
        pingpongR = 0;
        retardo.clear();
        length_buffer = (int)(samplerate * (dly / 1000));
    }
    if (menu_ant != menu) {
        readdelayL = 0;
        readdelayR = 0;
        writedelayL = 0;
        writedelayR = 0;
        pingpongL = 1;
        pingpongR = 0;
        retardo.clear();
        menu_ant = menu;
    }
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        if (menu == 1) {
            normal(buffer, channel);
        }
        else {
            pingpong(buffer, channel);
        }
            
    }
}

//==============================================================================
bool AlvDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AlvDelayAudioProcessor::createEditor()
{
    return new AlvDelayAudioProcessorEditor (*this);
}

//==============================================================================
void AlvDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<XmlElement> xml(parametros.copyState().createXml());
    copyXmlToBinary(*xml, destData);
}

void AlvDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<XmlElement> params(getXmlFromBinary(data, sizeInBytes));

    if (params != nullptr)
    {
        if (params->hasTagName(parametros.copyState().getType())) {
            parametros.state = ValueTree::fromXml(*params);
        }
    }
}

void AlvDelayAudioProcessor::normal(AudioBuffer<float>& buffer, int chan)
{
    auto* channelData = buffer.getWritePointer(chan);
    //En la variable channelData se guardan los valores del buffer de la señal de entrada
    //para el canal chan que será 0 o 1 es decir izquierda o derecha
    if (chan == 0) {
        //Si chan == 0 se trata del canal izquierdo
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            //Se hace un bucle que recorre el buffer de entrada muestra a muestra
            if (readdelayL >= length_buffer) {
                //Si la variable readdelay (indica la posición de lectura del buffer circular de delay)
                //Tiene un valor mayor o igual que el tamaño del buffer se le asigna el valor 0
                //esto significa que la muestra que leerá será la primera de esta manera 
                //se vuelve a recorrer el buffer circular del delay para lectura
                readdelayL = 0;
            }
            if (writedelayL >= length_buffer) {
                //Igual que antes pero ahora para escritura
                writedelayL = 0;
            }
            float in = channelData[i]; //se crea la variable in y se le asigna la muestra correspondiente del buffer de entrada
            if (length_buffer == 0) {
                //caso especial si el retardo es 0
                delay_bufferL[writedelayL] = channelData[i];
                channelData[i] = in + fb * delay_bufferL[readdelayL];
            }
            else {
                //Para el resto de casos se asigna al buffer de salida que es el mismo que el de entrada
                //in que es el valor de la muestra de entrada + la muestra correspondiente del delay por la ganancia de feeback fb
                channelData[i] = in + fb * delay_bufferL[readdelayL];
                //Una vez se tiene la muestra de salida se sobreescribe el buffer del delay con el nuevo valor de salida,
                //de esta manera primero se lee el valor que corresponde y luego se sobreescribe de manera que cuando se da una vuelta al buffer
                //y vuelve a pasar por la muestra la salida es el valor que se sobreescribió en la anterior vuelta.
                delay_bufferL[writedelayL] = channelData[i];
            }
            readdelayL++;
            writedelayL++;
            //Se incrementa la posición de la muestra de lectura y escritura en 1.
        }
    }
    if (chan == 1) {
        //Procesado del canal derecho igual que para el canal izquierdo
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            if (readdelayR >= length_buffer) {
                readdelayR = 0;
            }
            if (writedelayR >= length_buffer) {
                writedelayR = 0;
            }
            float in = channelData[i];
            if (length_buffer == 0) {
                delay_bufferR[writedelayR] = channelData[i];
                channelData[i] = in + fb * delay_bufferR[readdelayR];
            }
            else {
                channelData[i] = in + fb * delay_bufferR[readdelayR];
                delay_bufferR[writedelayR] = channelData[i];
            }
            readdelayR++;
            writedelayR++;
        }
    }

}

void AlvDelayAudioProcessor::pingpong(AudioBuffer<float>& buffer, int chan)
{
    auto* channelData = buffer.getWritePointer(chan);
    if (chan == 0) {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            if (readdelayL >= length_buffer) {
                readdelayL = 0;
            }
            if (writedelayL >= length_buffer) {
                writedelayL = 0;
                if (pingpongL == 1) {
                    //La variable pingpongL conmuta cuando el buffer circular da una vuelta completa 
                    //para que en una repetición el retardo suene en un canal diferente
                    pingpongL = 0;
                }
                else {
                    pingpongL = 1;
                }
            }
            float in = channelData[i];
            if (length_buffer == 0) {
                delay_bufferL[writedelayL] = channelData[i];
            }
            else {
                channelData[i] = in + pingpongL * fb * delay_bufferL[readdelayL];//se multiplica la señal retardada por su ganancia y pingpongL
                //de esta manera cuando pingpongL es igual a 0 no sonará el retardo por el canal y cuando valga 1 sí
                float memorydelay = in + fb * delay_bufferL[readdelayL];//Es necesario guardar el valor que tendría la salida en el caso sin Ping-Pong
                //Porque si la variable pingpongL vale 0 y se guarda la salida el buffer se llena de 0 y no habrá más retardos
                delay_bufferL[writedelayL] = memorydelay;
            }
            readdelayL++;
            writedelayL++;
        }
    }
    if (chan == 1) {
        //Procesado del canal derecho igual que para el canal izquierdo
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            if (readdelayR >= length_buffer) {
                readdelayR = 0;
            }
            if (writedelayR >= length_buffer) {
                writedelayR = 0;
                if (pingpongR == 1) {
                    pingpongR = 0;
                }
                else {
                    pingpongR = 1;
                }
            }
            float in = channelData[i];
            if (length_buffer == 0) {
                delay_bufferR[writedelayR] = channelData[i];
            }
            else {
                channelData[i] = in + pingpongR * fb * delay_bufferR[readdelayR];
                float memorydelay = in + fb * delay_bufferL[readdelayR];
                delay_bufferR[writedelayR] = memorydelay;
            }
            readdelayR++;
            writedelayR++;
        }
    }

}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AlvDelayAudioProcessor();
}
AudioProcessorValueTreeState::ParameterLayout
AlvDelayAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<AudioParameterFloat>("feedback","Feedback",0.0f,1.0f,0.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("delay", "Delay", 0.0f, 3000.0f, 0.0f));
    parameters.push_back(std::make_unique<AudioParameterInt>("combobox", "ComboBox", 1, 2, 1));
    return { parameters.begin(), parameters.end() };
}