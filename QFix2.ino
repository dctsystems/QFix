
//Reads from HW Midi input
//Writes to HW or USB Midi
//define this to use USB Output
#define ENABLE_USB_OUT
//#define ENABLE_SER_OUT
//#define PEDALS 2
//#define VOLUME
#define LED

#ifdef PEDALS
static int pedalPin[PEDALS]={11,12};
static int pedalCC[PEDALS]={0x40,0x41};
static int pedalState[PEDALS];
static int pedalOutput[PEDALS];
#endif

#ifdef VOLUME
const int volumePin = A5;
const int volumeCC = 7;
#endif

#ifdef LED
const uint8_t statusLED = 13;
const uint16_t ledTimout = 1000;

static uint16_t ledTimer;

static void activateLED(void)
{
  digitalWrite(statusLED, LOW);//:LED is _ON_ by default, so flash it low
  ledTimer = ledTimout;
}
#endif

#ifdef ENABLE_USB_OUT
#include <USB-MIDI.h>
USING_NAMESPACE_MIDI;

typedef USBMIDI_NAMESPACE::usbMidiTransport __umt;
typedef MIDI_NAMESPACE::MidiInterface<__umt> __ss;
__umt usbMIDI(0); // cableNr
__ss MIDICoreUSB((__umt&)usbMIDI);
typedef Message<MIDI_NAMESPACE::DefaultSettings::SysExMaxSize> MidiMessage;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDICoreSerial);

#define INPUT_TRANSPORT MIDICoreSerial
#define USB_TRANSPORT MIDICoreUSB
#define SER_TRANSPORT MIDICoreSerial
#else
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#define INPUT_TRANSPORT MIDI
#define SER_TRANSPORT MIDI
#endif


int8_t currentOffset[3];
const int maxNotes = 4;
uint8_t currentNote[3][maxNotes];


static void deleteFromNoteList(int chan, int note)
{
  int i;
  for (i = 0; i < maxNotes; i++)
  {
    if (currentNote[chan][i] == note)
    {
      currentNote[chan][i] = 0;
      return;
    }
  }
}
static void addToNoteList(int chan, int note)
{
  int i;
  deleteFromNoteList( chan, note);
  for (i = 0; i < maxNotes; i++)
  {
    if (currentNote[chan][i] == 0)
    {
      currentNote[chan][i] = note;
      return;
    }
  }
}
static void clearNoteList(int chan)
{
  int i;
  for (i = 0; i < maxNotes; i++)
  {
    if (currentNote[chan][i] > 0)
    {
#ifdef ENABLE_USB_OUT
      USB_TRANSPORT.sendNoteOff(currentNote[chan][i], 0, chan + 14);
#endif
#ifdef ENABLE_SER_OUT
      SER_TRANSPORT.sendNoteOff(currentNote[chan][i], 0, chan + 14);
#endif
      currentNote[chan][i] = 0;
    }
  }
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
#ifdef LED
  activateLED();
#endif

  if (channel < 14)
  {
#ifdef ENABLE_USB_OUT
    USB_TRANSPORT.sendNoteOn( pitch, velocity, channel);
#endif
#ifdef ENABLE_SER_OUT
    SER_TRANSPORT.sendNoteOn( pitch, velocity, channel);
#endif
  }
  else
  {
    uint8_t chan = channel - 14;
    int newNote = pitch + currentOffset[chan];
#ifdef ENABLE_USB_OUT
    USB_TRANSPORT.sendNoteOn(newNote, velocity, channel);
#endif
#ifdef ENABLE_SER_OUT
    SER_TRANSPORT.sendNoteOn(newNote, velocity, channel);
#endif

    addToNoteList(chan, newNote);
  }
}

static void handleNoteOff(byte channel, byte pitch, byte velocity)
{
#ifdef LED
  activateLED();
#endif


  if (channel < 14)
  {
#ifdef ENABLE_USB_OUT
    USB_TRANSPORT.sendNoteOff(pitch, 64, channel);
#endif
#ifdef ENABLE_SER_OUT
    SER_TRANSPORT.sendNoteOff(pitch, 64, channel);
#endif
  }
  else
  {
    uint8_t chan = channel - 14;
    int newNote = pitch + currentOffset[chan];
#ifdef ENABLE_USB_OUT
    USB_TRANSPORT.sendNoteOff(pitch + currentOffset[chan], velocity, channel);
#endif
#ifdef ENABLE_SER_OUT
    SER_TRANSPORT.sendNoteOff(pitch + currentOffset[chan], velocity, channel);
#endif
    deleteFromNoteList(chan, newNote);
  }
}

static void handlePitchBend(byte channel, int bend)
{
#ifdef LED
  activateLED();
#endif


  if (channel < 14)
  {
#ifdef ENABLE_USB_OUT
    USB_TRANSPORT.sendPitchBend( bend, channel);

    USB_TRANSPORT.sendPitchBend( bend, 14);
    USB_TRANSPORT.sendPitchBend( bend, 15);
    USB_TRANSPORT.sendPitchBend( bend, 16);
#endif
#ifdef ENABLE_SER_OUT
    SER_TRANSPORT.sendPitchBend( bend, channel);

    SER_TRANSPORT.sendPitchBend( bend, 14);
    SER_TRANSPORT.sendPitchBend( bend, 15);
    SER_TRANSPORT.sendPitchBend( bend, 16);
#endif
  }
  else
  {
    uint8_t chan = channel - 14;
    clearNoteList(chan);

    currentOffset[chan] = (bend) / 682;

  }
}

//Pass through all other Channel Messages unchanged (System Messages to through automatically)
static void handleControlChange(byte channel, byte number, byte value)
{
#ifdef LED
  activateLED();
#endif

#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendControlChange(number, value, channel);
#endif
#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendControlChange(number, value, channel);
#endif

}
static void handleProgramChange(byte channel, byte number)
{
#ifdef LED
  activateLED();
#endif
#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendProgramChange(number, channel);
#endif
#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendProgramChange(number, channel);
#endif
}

static void handleAfterTouchChannel(byte channel, byte pressure)
{
#ifdef LED
  activateLED();
#endif

#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendAfterTouch(pressure, channel);
#endif
#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendAfterTouch(pressure, channel);
#endif
}
static void handleAfterTouchPoly(byte channel, byte note, byte pressure)
{
#ifdef LED
  activateLED();
#endif

  if (channel < 14)
  {
#ifdef ENABLE_USB_OUT
    USB_TRANSPORT.sendAfterTouch( note, pressure, channel);
#endif
#ifdef ENABLE_SER_OUT
    SER_TRANSPORT.sendAfterTouch( note, pressure, channel);
#endif
  }
  else
  {
    uint8_t chan = channel - 14;
#ifdef ENABLE_USB_OUT
    USB_TRANSPORT.sendAfterTouch(note + currentOffset[chan], pressure, channel);
#endif
#ifdef ENABLE_SER_OUT
    SER_TRANSPORT.sendAfterTouch(note + currentOffset[chan], pressure, channel);
#endif
  }
}

//Realtime Messages: Need to explicitly handle these to copy between transports
static void handleClock(void)
{
#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendClock();
#endif
#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendClock();
#endif
}
static void handleStart(void)
{
#ifdef LED
  activateLED();
#endif
#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendStart();
#endif

#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendStart();
#endif
}

static void handleContinue(void)
{
#ifdef LED
  activateLED();
#endif
#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendContinue();
#endif
#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendContinue();
#endif
}
static void handleStop(void)
{
#ifdef LED
  activateLED();
#endif
#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendStop();
#endif
#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendStop();
#endif
}
static void handleSystemReset(void)
{
#ifdef LED
  activateLED();
#endif
#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.sendSystemReset();
#endif
#ifdef ENABLE_SER_OUT
  SER_TRANSPORT.sendSystemReset();
#endif
}

void setup()
{
#ifdef LED
  pinMode(statusLED, OUTPUT);
  digitalWrite(statusLED, HIGH);
#endif

  // Connect the handleNoteOn function to the library,
  // so it is called upon reception of a NoteOn.
  INPUT_TRANSPORT.setHandleNoteOn(handleNoteOn);
  INPUT_TRANSPORT.setHandleNoteOff(handleNoteOff);
  INPUT_TRANSPORT.setHandlePitchBend(handlePitchBend);
  INPUT_TRANSPORT.setHandleControlChange(handleControlChange);
  INPUT_TRANSPORT.setHandleProgramChange(handleProgramChange);

  //INPUT_TRANSPORT.setHandleAfterTouchChannel(handleAfterTouchChannel);
  //INPUT_TRANSPORT.setHandleAfterTouchPoly(handleAfterTouchPoly);

  INPUT_TRANSPORT.setHandleClock(handleClock);
  INPUT_TRANSPORT.setHandleStart(handleStart);
  INPUT_TRANSPORT.setHandleContinue(handleContinue);
  INPUT_TRANSPORT.setHandleStop(handleStop);

  // Initiate MIDI communications, listen to all channels
  SER_TRANSPORT.begin(MIDI_CHANNEL_OMNI);
  SER_TRANSPORT.turnThruOff();
#ifdef XTRANSPORT
  USB_TRANSPORT.begin(MIDI_CHANNEL_OMNI);
  USB_TRANSPORT.turnThruOff();
#endif


#ifdef PEDALS
int i;
for(i=0;i<PEDALS;i++)
  {
  pinMode(pedalPin[i], INPUT_PULLUP);
  pedalState[i] = digitalRead(pedalPin[i]);
  }
#endif


  //Serial.begin(9600);
}

void loop()
{
  SER_TRANSPORT.read();
#ifdef ENABLE_USB_OUT
  USB_TRANSPORT.read();
#endif



#ifdef LED
  if (ledTimer)
  {
    ledTimer--;
    if (ledTimer == 0)
      digitalWrite(statusLED, HIGH);
  }
#endif

  static uint32_t volumeCounter;
  volumeCounter++;
  if ((volumeCounter & 0xfff) == 0)
  {
    int newVal;

    //Lets add some foot pedals!
#ifdef PEDALS
  int i;
    for(i=0;i<PEDALS;i++)
    {
      newVal = digitalRead(pedalPin[i]);
      if (newVal != pedalState[i])
      {
        pedalState[i] = newVal;
         pedalOutput[i]=127-pedalOutput[i];
        //Serial.println("C:");
        //Serial.println(newVal);
        //Serial.println(pedalState[0]); 
  #ifdef ENABLE_USB_OUT
        USB_TRANSPORT.sendControlChange(pedalCC[i], pedalOutput[i], 14);
        USB_TRANSPORT.sendControlChange(pedalCC[i], pedalOutput[i], 15);
        USB_TRANSPORT.sendControlChange(pedalCC[i], pedalOutput[i], 16);
  #endif
  #ifdef ENABLE_SER_OUT
        SER_TRANSPORT.sendControlChange(pedalCC[i], pedalOutput[i], 14);
        SER_TRANSPORT.sendControlChange(pedalCC[i], pedalOutput[i], 15);
        SER_TRANSPORT.sendControlChange(pedalCC[i], pedalOutput[i], 16);
  #endif
 #ifdef LED
  activateLED();
#endif
      }
    }
#endif

#ifdef VOLUME
    static uint8_t volumeValue;
    static uint16_t maxVal;
    static float volumeScale;
    newVal = analogRead(volumePin);//0...1023
    if(newVal>maxVal)
      {
      maxVal=newVal;
      volumeScale=127.0/maxVal;
      }
    newVal*=volumeScale;

   if (newVal != volumeValue)
    {
      volumeValue = newVal;
#ifdef ENABLE_USB_OUT
      USB_TRANSPORT.sendControlChange(volumeCC, volumeValue, 14);
      USB_TRANSPORT.sendControlChange(volumeCC, volumeValue, 15);
      USB_TRANSPORT.sendControlChange(volumeCC, volumeValue, 16);
#endif
#ifdef ENABLE_SER_OUT
      SER_TRANSPORT.sendControlChange(volumeCC, volumeValue, 14);
      SER_TRANSPORT.sendControlChange(volumeCC, volumeValue, 15);
      SER_TRANSPORT.sendControlChange(volumeCC, volumeValue, 16);
#endif
#ifdef LED
  activateLED();
#endif
    }
#endif
  }

}
