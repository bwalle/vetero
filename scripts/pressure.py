#!/usr/bin/python3

import sys

def usage():
    print('Usage: pressure.py <height [m]> <pressure [hPa]>')
    sys.exit(1)

def calculate_pressure(height, abspressure):
    """See the data sheet of the BMP085 for the formula"""

    return abspressure / ((1 - height/44330.0) ** 5.255)

def main():
    if len(sys.argv) != 3:
        usage()

    height = float(sys.argv[1])
    abspressure = float(sys.argv[2])

    pressure = calculate_pressure(height, abspressure)
    print('Relative pressure: %f hPa' % (pressure))

if __name__ == "__main__":
    main()
