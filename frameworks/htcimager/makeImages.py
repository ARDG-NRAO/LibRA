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

class makeImages(object):
    def __init__(self, inputArgs):
        interface = inputArgs.interface
        if interface == 'libra_cl':
            from libra_cl_imager import libra_cl_imager

            imager = libra_cl_imager(inputArgs)
        elif interface == 'libra_py':
            raise TypeError('Not integrated')
        elif interface == 'casa_htclean':
            raise TypeError('Not integrated')
        elif interface == 'custom':
            from customImager import customImager

            imager = customImager(inputArgs)
        else:
            raise TypeError(interface + ': unknown interface')
        
        self.outputImageList = imager.outputImageList