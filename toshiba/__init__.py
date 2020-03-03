import os
import re
import sys
import glob
import time
import numpy as np
import netCDF4

def read(filename, maxgate=2048, verbose=0):
    fid = netCDF4.Dataset(filename, 'r')

    # Attributes, dimensions, and variables
    # atts = fid.ncattrs()
    # ds = [d for d in fid.dimensions]
    # vs = [v for v in fid.variables]

    # Get the variables that I care about
    lat = float(fid.variables['siteLat'][0])
    lon = float(fid.variables['siteLon'][0])
    start_time = time.gmtime(int(fid.variables['esStartTime'][0]))

    scan_el = np.array(fid.variables['elevationAngle'][0], dtype=np.float)
    ee = np.array(fid.variables['radialElev'])
    aa = np.array(fid.variables['radialAzim'])
    zz = fid.variables['Z']

    r0 = float(fid.variables['firstGateRange'][0])
    dr = float(fid.variables['gateSize'][0])
    ng = min(maxgate, zz.shape[1])
    rr = np.arange(r0, ng * dr, dr)

    m = np.array(zz.mask, dtype=np.bool)[:, :maxgate]
    z = np.array(zz[:, :maxgate])
    v = np.zeros(z.shape)
    z[m] = np.nan
    v[m] = np.nan

    if not zz.shape[0] == len(aa):
        print('Inconsistent azimuth count!  {} / {}'.format(z.shape[0], len(aa)))
        
    fid.close()

    o = {
        'longitude': lon,
        'latitude': lat,
        'time': start_time,
        'scan_el': scan_el,
        'ee': ee,
        'aa': aa,
        'rr': rr,
        'z': z,
        'v': v,
        'm': m
    }
    return o
