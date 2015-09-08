#!/bin/bash

adb wait-for-devices

adb logcat -s "UpdateService | UpdateCheck"
