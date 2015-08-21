#!/usr/bin/python

'''
***************************************************
'''

import os
import string
import sys
import re

#symbols_dir = './out/target/product/entrapment/symbols'
#cross_compile = './prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/arm-linux-androideabi-'
symbols_dir = './out/target/product/gladiator/symbols'
cross_compile = './prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.8/bin/arm-linux-androideabi-'
addr2line_cmd = cross_compile + "addr2line"
cppfilt_cmd = cross_compile + "c++filt"
maps_line = re.compile('[0-9a-f]{8}-[0-9a-f]{8}')
addr2line_map = dict()

def addr2line(lib, addr, abs_addr):
  list = addr2line_map.get(addr)
  if list != None:
    return list
  else:
    list = []

  if lib != "":
    cmd = addr2line_cmd + " -f -e " + symbols_dir + lib + " 0x" + addr
    stream = os.popen(cmd)
    lines = stream.readlines()
    list = map(string.strip, lines)
  else:
    list = []

  if list != []:
    mangled_name = list[0];
    cmd = cppfilt_cmd + " " + mangled_name
    stream = os.popen(cmd)

    list[0] = stream.readline()
    stream.close()
    list = map(string.strip, list)
  else:
    list = [ '0x%s unresolved' %(abs_addr.zfill(8)), lib ]

  addr2line_map.__setitem__(addr, list)
  return list

def convertAddress(abs_addr, maps):
  ret = (None, None)

  for m in maps:
    if abs_addr >= m[0] and abs_addr <= m[1]:
      if len(m) == 3:
        addr = '%08x' %(string.atoi(abs_addr, 16) - string.atoi(m[0], 16))
        lib = m[2]
        ret = (addr, lib)
  return ret

def parseRecord(lines, start, end):
  bt_list = []
  maps = []

  for i in range(start, end+1):
    s = lines[i]
    if s.startswith('z'):
      line = s.split()

      bt = (line[3], line[5], line[7 : len(line)])
      bt_list.append(bt)
    elif maps_line.match(s):
      l = s.split()
      m = l[0].split('-')
      if len(l) == 6:
        m.append(l[5])
      maps.append(m)

  for bt in bt_list:
    sz = string.atoi(bt[0])
    num = string.atoi(bt[1])
    size = sz * num

    show_header = True
    for abs_addr in bt[2]:
      (addr, lib) = convertAddress(abs_addr, maps)

      if (addr != None and lib == None):
        print '        0x%s unresolved' %(abs_addr)
        continue
      elif (addr == None):
        continue

      if show_header == True:
        print '    * Malloc/New Size: %d = %dx%d' % (size, sz, num)
        print '    *               >>> Backtrace <<<                 '
        print '    **************************************************'
        show_header = False

      func_line_pair = addr2line(lib, addr, abs_addr)
      if func_line_pair[0].startswith('leak_malloc') or \
         func_line_pair[0].startswith('leak_calloc') or \
         func_line_pair[0].startswith('leak_realloc') or \
         func_line_pair[0].startswith('leak_memalign'):
        continue
      if func_line_pair[0].startswith('malloc') or \
         func_line_pair[0].startswith('calloc') or \
         func_line_pair[0].startswith('realloc') or \
         func_line_pair[0].startswith('memalign'):
        print '        ' + func_line_pair[0] + '(sz=%d)' % (sz)
        continue
      print "        %s <-> %s" % (func_line_pair[0], func_line_pair[1])
    print ''

def main(argv):
  f = open(argv[0], 'r')
  f.seek(0)

  lines = f.readlines()
  start = 0
  end   = 0
  size  = len(lines)

  for i in range(0, size):
    if lines[i].startswith('Total memory:'):
      print '\n' + lines[i] + '----------------------------------------'
      start = i
    elif lines[i].startswith('END'):
      end = i
      # parse leak record
      if (end > start): parseRecord(lines, start, end)

if __name__ == '__main__': main(sys.argv[1:])
