#!/usr/bin/python3

import sys
import math

def usage():
    print('Usage: pressure.py <height [m]> <pressure [hPa]> <temperature [°C]>')
    sys.exit(1)

def calc_E(temperature):
    """Temperature must be in °C"""
    if temperature < 9.1:
        return 5.6402 * (-0.0916 + math.exp(0.06*temperature))
    else:
        return 18.2194 * (1.0463 - math.exp(-0.0666*temperature))

def C_to_K(temperature):
    """Converts Celsius to Kelvin"""
    return temperature + 273.25

def calculate_pressure(height, abspressure, temperature):
    """See http://de.wikipedia.org/wiki/Barometrische_H%C3%B6henformel#Reduktion_auf_Meeresh.C3.B6he"""

    # constants
    g0 = 9.80665
    R = 287.05
    a = 0.0065
    Ch = 0.12
    E = calc_E(temperature)
    Th = C_to_K(temperature)
    x = g0 / (R * (Th + Ch*E + a*height/2.0)) * height

    return abspressure * math.exp(x)

def main():
    if len(sys.argv) != 4:
        usage()

    height = float(sys.argv[1])
    abspressure = float(sys.argv[2])
    temperature = float(sys.argv[3])

    pressure = calculate_pressure(height, abspressure, temperature)
    print('Relative pressure: %f hPa' % (pressure))

if __name__ == "__main__":
    main()
