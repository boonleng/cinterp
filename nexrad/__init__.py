import os
import re
import sys
import glob
import time
import numpy as np

cache_dir = os.path.expanduser('~/Downloads/88d/cache')

def read(filename, cache=True, maxgate=2048, verbose=0):
    basename = os.path.basename(filename)
    cachefile = os.path.join(cache_dir, '{}.npz'.format(os.path.splitext(basename)[0]))
    if cache and os.path.exists(cachefile):
        if verbose:
            print('Loading cache {} ...'.format(cachefile))
        o = np.load(cachefile)
        # Convert some components
        sweep_time = time.localtime(int(o['time']))
        longitude = float(o['longitude'])
        latitude = float(o['latitude'])
        ee = o['ee']
        aa = o['aa']
        rr = o['rr']
        zs = o['z']
        vs = o['v']
        ms = o['m']
    else:
        import pyart
        vol = pyart.io.read_nexrad_archive(filename)
        s = re.search(r'2[0-9]{3}-[01][0-9]-[0-3][0-9]T[0-2][0-9]:[0-5][0-9]:[0-5][0-9]', vol.time['units']).group()
        sweep_time = time.strptime(s, '%Y-%m-%dT%H:%M:%S')
        longitude = vol.longitude['data'][0]
        latitude = vol.latitude['data'][0]
        rr = vol.range['data'][:maxgate]
        ee = None
        aa = None
        ms = None
        zs = None
        vs = None
        for k in [1, 3, 5]:
            sweep_slice = vol.get_slice(k)
            z = vol.fields['reflectivity']['data'][sweep_slice]
            v = vol.fields['velocity']['data'][sweep_slice]
            e = vol.fixed_angle['data'][k]
            a = vol.azimuth['data'][sweep_slice] * np.pi / 180.0
            m = np.logical_or(z.mask, v.mask)
            if len(a) == 720:
                a = a[::2]
                z = z[::2, :maxgate]
                v = v[::2, :maxgate]
                m = m[::2, :maxgate]
            else:
                z = z[:, :maxgate]
                v = v[:, :maxgate]
                m = m[:, :maxgate]
            # Exntend the dimension so that it is in sweep x azimuth x range
            e = np.expand_dims(e, axis=0)
            a = np.expand_dims(a, axis=0)
            m = np.expand_dims(m, axis=0)
            z = np.expand_dims(z, axis=0)
            v = np.expand_dims(v, axis=0)
            if ms is None:
                ee = e
                aa = a
                ms = m
                zs = z
                vs = v
            else:
                ee = np.concatenate((ee, e), axis=0)
                aa = np.concatenate((aa, a), axis=0)
                ms = np.concatenate((ms, m), axis=0)
                zs = np.concatenate((zs, z), axis=0)
                vs = np.concatenate((vs, v), axis=0)
        if verbose:
            print('Saving cache as {} ...'.format(cachefile))
        np.savez(cachefile, longitude=longitude, latitude=latitude, time=time.mktime(sweep_time),
            ee=ee, aa=aa, rr=rr, z=zs, v=vs, m=ms)
    o = {
        'longitude': longitude,
        'latitude': latitude,
        'time': sweep_time,
        'ee': ee,
        'aa': aa,
        'rr': rr,
        'z': zs,
        'v': vs,
        'm': ms
    }
    return o
