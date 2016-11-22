#!/usr/bin/env python
# $Id: createstartstates.py 732 2008-03-10 15:55:53Z lsc $

import sys
from math import sqrt
from random import random

# --- global params ---
cfg = {}
cfg["outfile"] = "0.xml"
cfg["radius"]  = 2.0

# --- XML content ---
cfg["header"] = """
<states>
    <itno>0</itno>
"""

cfg["footer"] = """
</states>
"""

cfg["body"] = """
    <xagent>
        <name>Circle</name>
        <id>%s</id>
        <x>%s</x>
        <y>%s</y>
        <fx>%s</fx>
        <fy>%s</fy>
        <radius>%s</radius>
    </xagent>
"""

def main():

    readInput()
    ofile = openFile(cfg["outfile"])
    writeXML(ofile)
    ofile.close()
    
def readInput():

    if len(sys.argv) != 2:
        rtfm()
    
    try:
        cfg["agent_count"] = int(sys.argv[1])
    except ValueError:
        print >> sys.stderr, "INVALID INPUT VALUE"
        rtfm()
        

def openFile(filename):
    try:
        out = open(filename, 'w')
    except IOError, (errno, strerror):
        print >> sys.stderr, "ERROR: Unable to open %s for writing" % filename
        print >> sys.stderr, "Failed with IO Error (%s) : %s" % (errno, strerror)
        sys.exit(2)

    return out

def writeXML(file):
    
    i = 0
    
    # define width and height of domain
    width  = (sqrt(cfg["agent_count"]) + 1) * 2 * cfg["radius"]
    height = width
    
    file.write(cfg["header"])
    
    while i < cfg["agent_count"] :
    
        id = i
        fx = 0.0
        fy = 0.0
        radius = cfg["radius"]
        
        # place agent at random point in domain
        x = (random() * width)  + (radius * 0.5) - (width * 0.5)
        y = (random() * height) + (radius * 0.5) - (width * 0.5)

        # write entry
        file.write(cfg["body"] % (id,x,y,fx,fy,radius))
            
        # increment
        i += 1
    
    file.write(cfg["footer"])
        
    
def rtfm():
    print >> sys.stderr, "Usage: %s <agent_count>" % sys.argv[0]
    sys.exit(1)



if __name__ == "__main__": main()
