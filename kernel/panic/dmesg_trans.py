#!/usr/bin/env python

import os
import re
import string
import sys
import commands

cross_compile = './prebuilts/gcc/linux-x86/aarch64/linaro-aarch64_linux-2014.09/bin/aarch64-linux-gnu-'
trace_line = re.compile("(.*)(\[<[0-9a-f]+>\]) (.*)")

def addr2line(line, exe, addr):
  cmd = cross_compile + 'addr2line -e ' + exe + ' ' + addr
  (status, output) = commands.getstatusoutput(cmd)
  print line + " : " + output

if __name__ == '__main__':
  buf  = 'cat ' + sys.argv[1]
  vmlinuz = sys.argv[2]

  stream = os.popen(buf)
  while (True):
    line = stream.readline()

    if (line == ''): # EOF reached
      break

    line = line.strip()
    match = trace_line.match(line)
    if (match):
      groups = match.groups()
      _m = re.findall(r"[0-9a-f]+", groups[1])
      if _m != None:
        addr = _m[0]
        addr2line(line, vmlinuz, addr)
    else:
      print line
      sys.stdout.flush()

  stream.close()
