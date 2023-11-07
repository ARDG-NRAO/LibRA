/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * File UTCCorrection.h
 */

#ifndef UTCCorrection_CLASS
#define UTCCorrection_CLASS

namespace asdm {

class UTCCorrection : public Interval {
/**
 * The UTCCorrestion class is used in the table of corrections to obtain UTC time
 * from TAI time.  A UTCCorrection instance has the Julian day on which a leap second
 * was added to UTC time and the cumulative number of added leap seconds at that time. 
 * 
 * @version 1.00 Nov 16, 2004
 * @author Allen Farris
 */
public:
	UTCCorrection(double jd, double taiMinusUTC);
	double getJD() const;
	double getTAIMinusUTC() const;

private:
	double jd;
	double taiMinusUTC;

};

inline UTCCorrection::UTCCorrection(double jd, double taiMinusUTC) {
	this->jd = jd;
	this->taiMinusUTC = taiMinusUTC;
}

inline double UTCCorrection::getJD() const {
	return jd;
}

inline double UTCCorrection::getTAIMinusUTC() const {
	return taiMinusUTC;
}

} // End namespace asdm

#endif /* UTCCorrection_CLASS */
