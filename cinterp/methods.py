import numpy as np

def dilate(x):
    y = np.logical_or(x, np.roll(x, 1, axis=0))
    y = np.logical_or(y, np.roll(x, -1, axis=0))
    y[:, 1:] = np.logical_or(y[:, 1:], x[:, :-1])
    y[:, :-1] = np.logical_or(y[:, :-1], x[:, 1:])
    return y

def cell2mask(shape, cells):
    mask = np.zeros(shape, dtype=np.bool)
    for c in cells:
        mask[c[0], c[1]] = True
    return mask

def cluster_ar_indices(cells, padding=(0, 0)):
    ia_min, ir_min = np.min(cells, axis=0)
    ia_max, ir_max = np.max(cells, axis=0)
    ia_min = ia_min - padding[0]
    ia_max = ia_max + padding[0]
    ir_min = ir_min - padding[1]
    ir_max = ir_max + padding[1]
    return (ia_min, ia_max), (ir_min, ir_max)

def cluster_ranges(mask):
    mda = np.any(mask, axis=1)
    ia_beg = next((i for i, j in enumerate(mda) if j), None)
    ia_end = len(mda) - next((i for i, j in enumerate(mda[::-1]) if j), None)

    mdr = np.any(mask, axis=0)
    ir_beg = next((i for i, j in enumerate(mdr) if j), None)
    ir_end = len(mdr) - next((i for i, j in enumerate(mdr[::-1]) if j), None)

    return np.arange(ia_beg, ia_end), np.arange(ir_beg, ir_end)

# z - reflectivity map
# aa - azimuth in radians
# rr - range in meters

def cinterp(z, aa, rr, cells):
    dr = 0.5 * (rr[1] - rr[0])
    roll_amount = int(aa[0] * 180.0 / np.pi)
    z_rolled = np.roll(z, roll_amount, axis=0)
    m = cell2mask(z.shape, cells)
    m1 = dilate(m)
    m2 = dilate(m1)
    md = m2 ^ m1

    # Gather the cell indices that need interpolation
    ia_sub, ir_sub = cluster_ranges(m)
    indices_bad = []
    a_bad = []
    r_bad = []
    for ia in ia_sub:
        for ir in ir_sub:
            if m[ia, ir]:
                indices_bad.append((ia, ir))
                a_bad.append(ia + 0.5)
                r_bad.append(rr[ir] + dr)
    r_bad = np.array(r_bad)
    a_bad = np.array(a_bad) / 180.0 * np.pi
    x_bad = r_bad * np.sin(a_bad)
    y_bad = r_bad * np.cos(a_bad)

    # Gather the cell indices of good data
    ia_sub, ir_sub = cluster_ranges(md)
    indices_good = []
    a_good = []
    r_good = []
    for ia in ia_sub:
        for ir in ir_sub:
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
        w = 1.0 / np.sqrt(dx * dx + dy * dy)
        z_interp[ii] = np.sum(z_good * w) / np.sum(w)
        
    return z_interp

