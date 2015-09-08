#!/bin/bash

adb wait-for-devices

logcat -s "UpdateService | UpdateCheck"
