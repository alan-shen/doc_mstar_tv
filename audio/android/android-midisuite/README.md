===========================================================================================
## 下载: 
	git clone https://github.com/philburk/android-midisuite

## 编译:
	必须基于 Android M 或之后的版本

## Reference:
	https://source.android.com/devices/audio/midi.html
	https://source.android.com/devices/audio/midi_test.html

===========================================================================================
App	Description

MidiScope or MidiScope			-	displays MIDI messages on-screen
MidiKeyboard					-	sends MIDI messages by pressing an on-screen music keyboard
MidiSynthExample or MidiSynth	-	simple MIDI synthesizer that uses sawtooth oscillators
MidiBtlePairing					-	pairs an Android device with a BLE peripheral
MidiTools						-	library dependency of the above apps

===========================================================================================
# android-midisuite

## Introduction

Android MIDI test programs and examples.

This is not an official Google product.

This is a suite of MIDI Apps that run on Android M or later.

They are intended for use by OEMs for testing MIDI
on new devices. But they may also be of use to
other developers.

## Building

Requires a platform build environment; see
[Building the System](https://source.android.com/source/building.html)

After your platform build environment is initialized, type:

```
mm
```

The resulting APKs are located in $OUT/data/app/Midi\*/Midi\*.apk

## Directories

The MidiTools folder contain general purpose MIDI classes
that are used by the other Apps.

MidiScope displays MIDI Messages on the screen.

MidiKeyboard display a simple on-screen music keyboard.

MidiSynthExample is a simple sawtooth MIDI synthesizer.

MidiBtlePairing pairs with Bluetooth MIDI devices.

See the README files in the App directories for more information.
