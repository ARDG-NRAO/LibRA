#! /usr/bin/env python3
# Copyright (C) 2024
# Associated Universities, Inc. Washington DC, USA.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.is
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning this should be addressed as follows:
#        Postal address: National Radio Astronomy Observatory
#                        1003 Lopezville Road,
#                        Socorro, NM - 87801, USA
#
# $Id$


import argparse
import os
import re
import time
import numpy as np

from multiprocessing.pool import Pool
from astropy.io import fits

def natural_sort(l):
    convert = lambda text: int(text) if text.isdigit() else text.lower()
    alphanum_key = lambda key: [convert(c) for c in re.split('([0-9]+)', key)]
    return sorted(l, key=alphanum_key)


def make_empty_image(imlist, nstokes=4, outname='concat.fits'):
    """
    Generate an empty dummy FITS data cube. The FITS cube can exceed available
    RAM.

    The 2D image dimensions are derived from the first cube in the list.
    The number of channels in the output cube is assumed to be the length
    of the input list of images.

    """

    with fits.open(imlist[0], memmap=True) as hdu:
        ref_header = hdu[0].header.copy()
        try:
            xdim, ydim = np.squeeze(hdu[0].data).shape[-2:]
        except:
            xdim, ydim = (1, 1)

    print("X-dimension: ", xdim)
    print("Y-dimension: ", ydim)

    zdim = int(len(imlist))
    wdim = nstokes

    dims = tuple([xdim, ydim, zdim, wdim])

    # create header from first input image to preserve WCS keywords
    dummy_data = np.zeros(tuple(1 for d in dims), dtype=np.float32)
    hdu = fits.PrimaryHDU(data=dummy_data, header=ref_header)

    header = hdu.header
    for i, dim in enumerate(dims, 1):
        header["NAXIS%d" % i] = dim

    header.tofile(outname, overwrite=True)

    # create full-sized zero image
    header_size = len(
        header.tostring()
    )  # Probably 2880. We don't pad the header any more; it's just the bare minimum
    data_size = np.prod(dims) * np.dtype(np.float32).itemsize
    # This is not documented in the example, but appears to be Astropy's default behaviour
    # Pad the total file size to a multiple of the header block size
    block_size = 2880
    print(block_size)
    data_size = block_size * (((data_size -1) // block_size) + 1)

    with open(outname, "rb+") as f:
        f.seek(header_size + data_size - 1)
        f.write(b"\0")


def update_fits_header(cube_path, header_dict):
    # Update the header of the FITS cube
    # TODO : Fill in the header with the correct values for the entire cube
    with fits.open(cube_path, memmap=True, ignore_missing_end=True, mode="update") as hdu:
        header = hdu[0].header
        for key, value in header_dict.items():
            header[key] = value



def fill_cube_with_images(imlist, nstokes=4, outname='concat.fits'):
    """
    Fills the empty data cube with fits data.

    The number of channels in the output cube is assumed to be the length
    of the input list of images.
    """
    # TODO: debug: if ignore_missing_end is False, throws an error
    outhdu = fits.open(outname, memmap=True, ignore_missing_end=True, mode="update")
    outdata = outhdu[0].data


    print("Opening all images")
    fptrlist = []
    for im in imlist:
        fptrlist.append(fits.open(im, memmap=True))

    max_chan =  int(len(imlist))
    t0 = time.time()
    for ii in range(0, max_chan):
        t1 = time.time()
        print(f"Processing channel {ii}/{max_chan} in {t1 - t0}s", end='\n')
        for ss in range(nstokes):
            outdata[ss, ii, :, :] = fptrlist[ii][0].data[ss, 0, :, :]

        t0 = time.time()

    outhdu.close()

    # collect per-channel frequencies from input images
    freqs = [float(fits.getheader(im)['CRVAL3']) for im in imlist]
    nchan = len(freqs)
    cdelt3 = np.median(np.diff(freqs))  # channel width
    #crpix3 = (nchan + 1) // 2                    # integer center channel (1-based)
    #crval3 = freqs[0] + (crpix3 - 1) * cdelt3  # frequency at center channel
    crpix3 = 1
    crval3 = freqs[0]  # frequency at first channel

    fitsheader = {
            "NAXIS3": nchan,
            "CRPIX3": crpix3,
            "CRVAL3": float(crval3),
            "CDELT3": float(cdelt3),
            }

    update_fits_header(outname, fitsheader)

    print("Closing all images")
    # Close all images
    for fptr in fptrlist:
        fptr.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Concatenate per-channel FITS images into a single cube.")
    parser.add_argument("files", nargs="+", help="Input FITS files")
    parser.add_argument("--nstokes", type=int, default=1,
                        help="Number of Stokes planes (default: 1)")
    parser.add_argument("--output", default="concat.fits",
                        help="Output filename (default: concat.fits)")
    args = parser.parse_args()

    t = time.time()
    print("Starting fitsconcat at ", time.time())

    imlist = natural_sort(args.files)
    print(f"Matched {len(imlist)} files")
    print("making empty image")
    make_empty_image(imlist, nstokes=args.nstokes, outname=args.output)
    print("filling cube with images")
    fill_cube_with_images(imlist, nstokes=args.nstokes, outname=args.output)
    print("Ending fitsconcat at ", time.time())
    print("Elapsed time is ", time.time() - t)
