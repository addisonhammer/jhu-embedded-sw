import geopandas
import matplotlib
import os
import time
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.colors as mcolors

matplotlib.rcParams['interactive'] == True

REDMOND = 'zip://.\\common\\assets\\Redmond\\Parcel.zip'
USA = '.\\common\\assets\\states.shp'
SEATTLE = 'zip://.\\common\\assets\\Seattle_City_Limits.zip'
FILE_PATH = USA
LAT = -122.1
LONG = 47.63
LAT_SPAN = 2
LONG_SPAN = 1
ALT = 100
ALT_SPAN = 100

# Globals
lats = []
lons = []
alts = []

def make_colormap(seq):
    """Return a LinearSegmentedColormap
    seq: a sequence of floats and RGB-tuples. The floats should be increasing
    and in the interval (0,1).
    """
    seq = [(None,) * 3, 0.0] + list(seq) + [1.0, (None,) * 3]
    cdict = {'red': [], 'green': [], 'blue': []}
    for i, item in enumerate(seq):
        if isinstance(item, float):
            r1, g1, b1 = seq[i - 1]
            r2, g2, b2 = seq[i + 1]
            cdict['red'].append([item, r1, r2])
            cdict['green'].append([item, g1, g2])
            cdict['blue'].append([item, b1, b2])
    return mcolors.LinearSegmentedColormap('CustomMap', cdict)

# Init Plot
WORLD = geopandas.read_file(FILE_PATH)
ax = WORLD.plot()
ax.set_xlim((LAT-LAT_SPAN,LAT+LAT_SPAN))
ax.set_ylim((LONG-LONG_SPAN, LONG+LONG_SPAN))

c = mcolors.ColorConverter().to_rgb
rvb = make_colormap(
    [c('red'), c('violet'), 0.33, c('violet'), c('blue'), 0.66, c('blue')])
# N = 1000
# array_dg = np.random.uniform(0, 10, size=(N, 2))
# colors = np.random.uniform(-2, 2, size=(N,))
# plt.scatter(array_dg[:, 0], array_dg[:, 1], c=colors, cmap=rvb)
# plt.colorbar()
# plt.show()

plt.ion()
plt.show()

# append and redraw new points
def plot_point(lat, lon, alt):
    lats.append(lat)
    lons.append(lon)
    alts.append((alt-ALT)/ALT_SPAN)
    plt.scatter(lats, lons, marker='x', c=alts, cmap=rvb)
    plt.draw()
    plt.pause(0.1)

if __name__ == '__main__':
    plt.draw()
    plt.pause(0.1)
    time.sleep(10)