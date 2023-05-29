#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

int currentOffset[3];
const int maxNotes=5;
int currentNote[3][5];

void deleteFromNoteList(int chan, int note)
{
  int i;
  for(i=0;i<maxNotes;i++)
    {
      if(currentNote[chan][i]==note)
        {
        currentNote[chan][i]=0;
        return;
        }
    }
}
void addToNoteList(int chan, int note)
{
  int i;
  deleteFromNoteList( chan, note);
  for(i=0;i<maxNotes;i++)
    {
      if(currentNote[chan][i]==0)
        {
        currentNote[chan][i]=note;
        return;
        }
    }
}
void clearNoteList(int chan)
{
  int i;
  for(i=0;i<maxNotes;i++)
    {
      if(currentNote[chan][i]>0)
        {
        MIDI.sendNoteOff(currentNote[chan][i], 0, chan+14);
        currentNote[chan][i]=0;
        }
    }
}
int ledTimer;
const int ledTimout = 1000;

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  digitalWrite(LED_BUILTIN, HIGH);
  ledTimer = ledTimout;
  if (channel < 14)
  {
    MIDI.sendNoteOn( pitch, velocity, channel);
  }
  else
  {
    uint8_t chan = channel - 14;
    int newNote=pitch + currentOffset[chan];
    MIDI.sendNoteOn(newNote, velocity, channel);
    addToNoteList(chan,newNote);
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  digitalWrite(LED_BUILTIN, HIGH);
  ledTimer = ledTimout;

  if (channel < 14)
  {
    MIDI.sendNoteOff(pitch, 64, channel);
  }
  else
  {
    uint8_t chan = channel - 14;
    int newNote=pitch + currentOffset[chan];
    MIDI.sendNoteOff(pitch + currentOffset[chan], velocity, channel);
    deleteFromNoteList(chan,newNote);
  }
}

void handlePitchBend(byte channel, int bend)
{
  digitalWrite(LED_BUILTIN, HIGH);
  ledTimer = ledTimout;

  if (channel < 14)
  {
    MIDI.sendPitchBend( bend, channel);

    MIDI.sendPitchBend( bend, 14);
    MIDI.sendPitchBend( bend, 15);
    MIDI.sendPitchBend( bend, 16);
    }
  else
  {
    uint8_t chan = channel - 14;
   clearNoteList(chan);
    
    currentOffset[chan] = (bend) / 682;
 
  }
}

//Pass through all other Channel Messages unchanged (System Messages to through automatically)
void handleControlChange(byte channel, byte number, byte value)
{
  digitalWrite(LED_BUILTIN, HIGH);
  ledTimer = ledTimout;
  MIDI.sendControlChange(number, value, channel);

}
void handleProgramChange(byte channel, byte number)
{
  digitalWrite(LED_BUILTIN, HIGH);
  ledTimer = ledTimout;
  MIDI.sendProgramChange(number, channel);
}
void handleAfterTouchChannel(byte channel, byte pressure)
{
  digitalWrite(LED_BUILTIN, HIGH);
  ledTimer = ledTimout;
  MIDI.sendAfterTouch(pressure, channel);
}

void handleAfterTouchPoly(byte channel, byte note, byte pressure)
{
  digitalWrite(LED_BUILTIN, HIGH);
  ledTimer = ledTimout;

  if (channel < 14)
  {
    MIDI.sendAfterTouch( note, pressure, channel);
  }
  else
  {
    uint8_t chan = channel - 14;
    MIDI.sendAfterTouch(note + currentOffset[chan], pressure, channel);
  }

}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect the handleNoteOn function to the library,
  // so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.setHandleProgramChange(handleProgramChange);
  MIDI.setHandleAfterTouchChannel(handleAfterTouchChannel);
  MIDI.setHandleAfterTouchPoly(handleAfterTouchPoly);


  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  MIDI.turnThruOn(midi::Thru::DifferentChannel); //In Omni will block all channel messages and pass all systemn messages

}

void loop()
{
  MIDI.read();
  if (ledTimer > 0)
  {
    ledTimer--;
    if (ledTimer == 0)
      digitalWrite(LED_BUILTIN, LOW);
  }
}
