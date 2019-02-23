#!/usr/bin/python
#
# This code is released under the MIT License.
#
# Copyright (c) 2019 Methlal Pallewatta
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# and associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial
# portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
# LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import smbus
import time
import os

address = 0x20

bus = smbus.SMBus(1)

while (1):
    read=bus.read_i2c_block_data(0x20,0x00,3)
    print ("Status = 0x%x" % read[2])
    if (read[2] & 0x2) :
        print ("USB Power = ON")
    else :
        print ("Running on battery.")
        print ("BAT Voltage = %f" % ((read[0] + (read[1] << 8)) * 5.0/4096))
        if (read[2] & 0x1) :
	    print ("Low Battery !.")
    print ""
    time.sleep(5)

