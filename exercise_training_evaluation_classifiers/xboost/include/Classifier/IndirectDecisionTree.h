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

#ifndef _INDIRECT_DECISION_TREE_H
#define _INDIRECT_DECISION_TREE_H

/** @file IndirectDecisionTree.h
 *  @brief implement a decision tree with feature stored in a map */

#include <iostream>
#include "Types.h"

/** An [indirect] decision Tree. It contains an index of a feature.
 *  If has some sub-nodes,uses th to split in them. If not have subnodes, it reports the category */
template<class DataType>
class IndirectDecisionTree {
public:

    /// index to a Binary Classifier
    int classifier;
    /// Classifier Threshold
    DataType th;

    /// if not a node, a category
    int category;

    /// sub-nodes (if null is a leaf)
    IndirectDecisionTree<DataType> *left, *right;

public:


    IndirectDecisionTree() : left(0), right(0) { }

    IndirectDecisionTree(const IndirectDecisionTree<DataType> & src) : left(0), right(0)
    {
        classifier = src.classifier;
        th = src.th;
        category = src.category;

        if(src.left)
            left = new IndirectDecisionTree<DataType>(*src.left);
        if(src.right)
            right = new IndirectDecisionTree<DataType>(*src.right);
    }

    static std::string signature() {
        return "x-decision-tree";
    }

    void operator=(const IndirectDecisionTree<DataType> & src)
    {
        delete left;
        delete right;

        classifier = src.classifier;
        th = src.th;
        category = src.category;

        if(src.left)
            left = new IndirectDecisionTree<DataType>(*src.left);
        else
            left = 0;
        if(src.right)
            right = new IndirectDecisionTree<DataType>(*src.right);
        else
            right = 0;
    }

    void print(int depth)
    {
        for(int i =0; i<depth; ++i) std::cout << "|     ";
        if(left!=0)
        {
            std::cout << "* f[" << classifier <<"] > " << th << "?\n";
            right->print(depth+1);
            left->print(depth+1);
        }
        else
            std::cout << "# " << category << std::endl;
    }

    /// execute classification using the DecisionTree
    int classify(const DataType *features, long stride) const {
        if(left!=0)
        {
            return ( features[classifier * stride] > th ) ? right->classify(features, stride) : left->classify(features, stride);
        }

        return category;
    }

    ///
    inline int operator()(const DataType *features, long stride) const {
        return classify(features, stride);
    }


    ~IndirectDecisionTree() {
        delete left;
        delete right;
    }

};


#endif
