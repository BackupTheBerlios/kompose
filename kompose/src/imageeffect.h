//
// C++ Interface: imageeffect
//
// Description:
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IMAGEEFFECT_H
#define IMAGEEFFECT_H

#include <vector>

#include <qcolor.h>

class QImage;

/**
@author Hans Oischinger
*/
class ImageEffect
{
public:
  ImageEffect();

  ~ImageEffect();
  


    /**
     * Scales the image.
     *
     * @param image The image to use.
     * @param new_width The new width.
     * @param new_height The new height.
     */

    static void scale(QImage & image, int new_width, int new_height);


private:

    /**
     * Creates the scale factor vectors.
     *
     * @param from_pixels The original number of pixels.
     * @param to_pixels The new number of pixels.
     * @param sum The sum of the factors.
     * @param lengths Here we store the number pixels for the new pixels.
     * @param starts Here we store of indices of the start pixels.
     * @param factor_offsets Here we store the offsets in the factors array.
     * @param factors Here we store the factos.
     */

    static void calcScaleFactors(int from_pixels, int to_pixels, int sum, std::vector<int> & lengths,
                                 std::vector<int> & starts, std::vector<int> & factor_offsets,
                                 std::vector<Q_UINT32> & factors);
};

#endif
