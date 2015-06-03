/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
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

#ifndef _DECISION_TREE_H
#define  DECISION_TREE_H

/** @file DecisionTree.h
 *  @brief implement a decision tree */

#include <iostream>
#include "Types.h"

/** A decision Tree.
 *  If has some sub nodes,uses th to split in them. If not have subnodes, report the category 
 */
template<class T, class DataType = int>
class DecisionTree {
public:

    /// a feature extractor
    T classifier;
    
    /// Classifier Threshold
    DataType th;

    /// sub-nodes (if null is a leaf)
    DecisionTree<T, DataType> *left, *right;

    /// if there are not subnodes, this value -1 .. 1 return the class and probability
    float category;

public:
  
    ///
    typedef DecisionTree< typename T::OptimizedType, DataType> OptimizedType;

    /// DecisionTree is (generic) a RealClassifier
    static const ClassifierType Type = RealClassifier;

public:
  
  /// Copy & convert constructor
    template<class P1, class SrcFeature>
    DecisionTree(const DecisionTree<SrcFeature, DataType> & src, const P1 & p1) : left(0), right(0)
    {
        classifier = src.classifier; // TODO: convert
        th = src.th;
        category = src.category;

        if(src.left)
            left = new DecisionTree<T>(*src.left, p1);
        if(src.right)
            right = new DecisionTree<T>(*src.right, p1);
    }
    
  /// Copy & convert constructor
    template<class P1, class P2, class SrcFeature>
    DecisionTree(const DecisionTree<SrcFeature, DataType> & src, P1 p1, const P2 & p2) : classifier(src.classifier, p1, p2), left(0), right(0)
    {
        th = src.th;
        category = src.category;

        if(src.left)
            left = new DecisionTree<T>(*src.left, p1, p2);
        if(src.right)
            right = new DecisionTree<T>(*src.right, p1, p2);
    }
        
  
  /// Copy & convert constructor
    template<class P1, class P2, class P3>
    DecisionTree(const DecisionTree<T, DataType> & src, P1 p1, P2 p2, P3 p3) : left(0), right(0)
    {
        classifier = src.classifier; // TODO: convert
        th = src.th;
        category = src.category;

        if(src.left)
            left = new DecisionTree<T>(*src.left, p1, p2, p3);
        if(src.right)
            right = new DecisionTree<T>(*src.right, p1, p2, p3);
    }

    DecisionTree(const DecisionTree<T, DataType> & src) : left(0), right(0)
    {
        classifier = src.classifier;
        th = src.th;
        category = src.category;

        if(src.left)
            left = new DecisionTree<T>(*src.left);
        if(src.right)
            right = new DecisionTree<T>(*src.right);
    }

    void operator=(const DecisionTree<T> & src)
    {
        delete left;
        delete right;

        classifier = src.classifier;
        th = src.th;
        category = src.category;

        if(src.left)
            left = new DecisionTree<T>(*src.left);
        else
            left = 0;
        if(src.right)
            right = new DecisionTree<T>(*src.right);
        else
            right = 0;
    }

    DecisionTree() : left(0), right(0) { }
    DecisionTree(std::istream &in) : left(0), right(0)
    {
        load(in);
    }

    bool load(std::istream &in)
    {
        std::string c;
        delete left;
        delete right;

        in >> c;
        if(in.eof())
            return false;

        if(c == "n")
        {
            left = new DecisionTree<T>();
            right = new DecisionTree<T>();
            in >> th;
            in >> classifier;
            in >> *left;
            in >> *right;
        }
        else if(c == "l")
        {
            in >> category;
        }
        else
        {
            std::cerr << c << " at " << in.tellg() << std::endl;
            throw std::runtime_error("wrong file format");
        }
        return false;
    }

    void save(std::ostream &out) const
    {

        if(left!=0)
        {
            // node
            out << 'n' << ' ' << th << ' ' << classifier << ' ' << *left << ' ' << *right;
        }
        else
        {
            // leaf
            out << 'l' << ' ' << category;
        }
    }

    ~DecisionTree() {
        delete left;
        delete right;
    }

    static std::string signature() {
        return T::signature() + "-decision-tree";
    }

/// internally normalized
    static float max_response() {
        return 1.0f;
    }

    template<class FeatureType>
    bool export_features(std::vector<FeatureType> & out) const
    {
        // unimplementend yet
        return false;
    }

    /// perform the classification stage (1 params)
    template<class Param1>
    float classify(const unsigned int *image, Param1 stride) const {
        if(left!=0)
        {
            return ( classifier.response(image, stride) > th ) ? right->classify(image, stride) : left->classify(image, stride);
        }

        return category;
    }
    
    /// perform the classification stage (no params)
    float classify(const unsigned int *image) const {
        if(left!=0)
        {
            return ( classifier.response(image) > th ) ? right->classify(image) : left->classify(image);
        }

        return category;
    }    

    template<class Param1>
    inline float operator()(const unsigned int *image, Param1 stride) const {
        return classify(image, stride);
    }
    
    inline float operator()(const unsigned int *image) const {
        return classify(image);
    }    
    
    /// scale the response of the decision tree
    void scale_response(float factor)
    {
      if(left!=0)
      {
        left->scale_response(factor);
        right->scale_response(factor);
      }
      else
      {
        category *= factor;
      }
    }

    /// estimate the number of layer evaluated (roughly)
    float MeasureAvgDepth() const {
        if(left!=0)
        {
            return 1.0 + (left->MeasureAvgDepth() + right->MeasureAvgDepth()) * 0.5f;
        }
        else
        {
            return 0.0f;
        }
    }

    /// transform a Probabilistic Decision Tree in a Discrete Decision Tree
    void Discrete()
    {
        if(left!=0)
        {
            left->Discrete();
            right->Discrete();
        }
        else
        {
            category = (category > 0.0) ? 1.0 : -1.0;
        }
    }

    /// MMMMH?
    float getAlpha() const {
        return 1.0f;
    }

};

template<class T>
std::istream & operator >> (std::istream & in, DecisionTree<T>& s);
template<class T>
std::ostream & operator << (std::ostream & out, const DecisionTree<T> & s);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
std::istream & operator >> (std::istream & in, DecisionTree<T>& s)
{
    s.load(in);
    return in;
}

template<class T>
std::ostream & operator << (std::ostream & out, const DecisionTree<T> & s)
{
    s.save(out);
    return out;
}


#endif
