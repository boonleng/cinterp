import numpy as np

def dilate(x):
    y = np.logical_or(x, np.roll(x, 1, axis=0))
    y = np.logical_or(y, np.roll(x, -1, axis=0))
    y[:, 1:] = np.logical_or(y[:, 1:], x[:, :-1])
    y[:, :-1] = np.logical_or(y[:, :-1], x[:, 1:])
    return y

# z - reflectivity map
# aa - azimuth in radians
# rr - range in meters

def cinterp(z, aa, rr, cells):
    dr = 0.5 * (rr[1] - rr[0])
    roll_amount = int(aa[0] * 180.0 / np.pi)
    z_rolled = np.roll(z, roll_amount, axis=0)
    m = np.zeros(z_rolled.shape, dtype=np.bool)
    for c in cells:
        m[c[0], c[1]] = True
    m1 = dilate(m)
    m2 = dilate(m1)
    md = m2 ^ m1

    # mda = np.sum(md, axis=1, dtype=np.bool)
    # ia_beg = next((i for i, j in enumerate(mda) if j), None)
    # ia_end = len(mda) - 1 - next((i for i, j in enumerate(mda[::-1]) if j), None)

    # mdr = np.sum(md, axis=0, dtype=np.bool)
    # ir_beg = next((i for i, j in enumerate(mdr) if j), None)
    # ir_end = len(mdr) - 1 - next((i for i, j in enumerate(mdr[::-1]) if j), None)

    # Gather the cell indices that need interpolation
    indices_bad = []
    a_bad = []
    r_bad = []
    for ia in range(md.shape[0]):
        for ir in range(md.shape[1]):
            if m[ia, ir]:
                indices_bad.append((ia, ir))
                a_bad.append(ia + 0.5)
                r_bad.append(rr[ir] + dr)
    r_bad = np.array(r_bad)
    a_bad = np.array(a_bad) / 180.0 * np.pi
    x_bad = r_bad * np.sin(a_bad)
    y_bad = r_bad * np.cos(a_bad)

    # Gather the cell indices of good data
    indices_good = []
    r_good = []
    a_good = []

    for ia in range(md.shape[0]):
        for ir in range(md.shape[1]):
            if md[ia, ir]:
                indices_good.append((ia, ir))
                a_good.append(ia + 0.5)
                r_good.append(rr[ir] + dr)
    r_good = np.array(r_good)
    a_good = np.array(a_good) / 180.0 * np.pi
    x_good = r_good * np.sin(a_good)
    y_good = r_good * np.cos(a_good)

    # Good reflectivity values surrounding the cluster
    z_good = z_rolled[md]
    
    # Interpolation
    z_interp = z_rolled.copy()

    for k, ii in enumerate(indices_bad):
        dx = x_bad[k] - x_good
        dy = y_bad[k] - y_good
        dd = np.sqrt(dx * dx + dy * dy)
        #z_interp[ii] = np.mean(dd[:10]) * 3.0
        w = 1.0 / dd
        z_interp[ii] = np.sum(z_good * w) / np.sum(w)
        
    return z_interp

