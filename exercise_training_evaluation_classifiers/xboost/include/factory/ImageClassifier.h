/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 * Copyright (c) 2013-2014 Luca Castangia <l.caio@ce.unipr.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _IMAGE_CLASSIFIER_H
#define _IMAGE_CLASSIFIER_H

#include <vector>
#include <iostream>
#include "Types.h"
#include "Image.h"

/** @file ImageClassifier.h
 * @brief virtual classes to work on classifier.
 *        ImageClassifier is the simplest one, computing the response image.
 * */


/// "Binary" Classifier, virtual class
class ImageClassifier {

private:
    /// geometry
    int width,height;
public:

    virtual ~ImageClassifier();

    /// default constructor, import geometry from istream
    /// read width, height
    ImageClassifier(std::istream & in) {
        in >> width >> height;
    }

    ImageClassifier(int w,int h) : width(w), height(w) {
    }

    /// return the [current] classifier geometry
    size GetClassifierGeometry() const {
        return size(width,height);
    }

    /// return the classifier signature
    virtual std::string getSignature() const = 0;

    /// Import an image.
    ///  the Image is preprocessed for further detection performed by operator() and Process.
    ///  no scale changes are applyed to image.
    /// TODO: stride before width and height???
    virtual void setImage(const unsigned char *ptr, long stride, unsigned int width, unsigned int height)  = 0;

    /// setImage for the following operator() and Process
    void setImage(const ImageHandle & src)
    {
        setImage(src.data, src.stride, src.width, src.height);
    }

    /// compute response for an image point imported using setImage.
    /// @param x0,y0 an image point (top-left coordinate of window)
    virtual float operator() (int x0, int y0) const  = 0;

    /// Compute the response image for one scale, one row from (x0,y0) for n points, step @a step
    ///  process all points of image imported with setImage
    virtual void Process (double *out, int x0, int y0, int n, int step=1) const = 0;

    /// Release memory associated to setImage
    /// TODO: is necessary?
    virtual void Release() = 0;
};

#endif
