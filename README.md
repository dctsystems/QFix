# QFix
Arduino Midi Interface for Suzuki QChord (Omnichord)

My friend Gareth was perhaps the most famous QChord player in the world! Certainly one of the few who made a career of it. During lockdown's in 2020 he began working on some material to be released as a album, and tried experiementing with the QChord's Midi outout. He quickly discovered that the Midi output is (to put it mildly) "strange", and is pretty useless for feeding into any kind of DAW.

We talked about some kind of software script, but given the need for a simple, fix and forget solution, I built him a prototype box based in an Arduino Uno, and the standard Arduino Midi Shield. It takes Midi from the QChord, and outputs it as something normal. Just plug it in between your qchord and your other midi device. When lockdown lifted he came round and we tested it, made a few changes and ended up with the code in the QFix.ino file. This is the version Gareth used.

The original ran on an Uno because that's what I had spare at the time, but I subsequenty reworked the code for the Lenonardo, which is in QFix2. This supports USB midi, so can act as both converter and interface (On the original version it uses the 5pin output, to feed a "real" midi interface). This version also has the option to support two pedal switches and an expression pedal (check the #defines in the first few lines to enable them). You can also configure this version to generate 5-pin Midi if you prefer.

I did testing on all the new features of QFix2, but may have introduced a few new bugs.

Gareth was unable to test the final version before his death a few months ago, and without a QChord I'm unlikely to revist any of this code, so I'm just putting it here for anyone who needs it.

If this is useful you then drop a few bucks to Gareths family
https://www.justgiving.com/crowdfunding/gareth-richards-comedian

