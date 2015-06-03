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

#ifndef _CASCADE_CLASSIFIER_H
#define _CASCADE_CLASSIFIER_H

/** @file SoftCascade.h
  * @brief A boosted soft cascade classifier
  **/

/// A single classifier based on a single HaarFeature
/// @param WeakClassifier a Weak Classifier type.
template<class WeakClassifier >
struct SoftCascadeStage : public BoostableClassifier< WeakClassifier > {
public:
  
    /// the optimized version of the SoftCascadeStage classifier
    typedef SoftCascadeStage< typename WeakClassifier::OptimizedType > OptimizedType;

public:
  
    /** threshold of rejection of this step */
    float rejection;

public:

    SoftCascadeStage() : rejection(0.0f) { }

    /// @note dummy to avoid missunderstood during function calling
    template<class R, class P1, class P2>
    SoftCascadeStage(bool dummy, const SoftCascadeStage<R> & src, P1 p1, P2 p2) : BoostableClassifier< WeakClassifier >( src, p1, p2), rejection (src.rejection)
    {
    }
    
    template<class R, class P1, class P2, class P3>
    SoftCascadeStage(const SoftCascadeStage<R> & src, P1 scale, P2 offset, P3 stride) : BoostableClassifier< WeakClassifier >( src, scale, offset, stride), rejection (src.rejection)
    {
    }
    
    SoftCascadeStage(const WeakClassifier & weak, float a, float r) : BoostableClassifier<WeakClassifier>(weak, a), rejection(r) { }

    static std::string signature() {
        return "softcascade-" + BoostableClassifier<WeakClassifier>::signature();
    }
};

// IO operators

template<class T>
std::istream & operator >> (std::istream & in, SoftCascadeStage<T> & s);

template<class T>
std::ostream & operator << (std::ostream & out, const SoftCascadeStage<T> & s);

/// A classifier that classify with majority voting of N (weak) classifier and uses a Soft Cascade rejection mechanism.
/// Result are voted for majority and rejected with threshold.
/// @param T a (Boostable) Weak Classifier type
template<class T>
struct SoftCascadeClassifier {

public:

/// the inner weak classifier
    typedef T WeakClassifier;
/// A softcascadeboostable classifier is a weak classifier with a weight associated and a rejection threshold
    typedef SoftCascadeStage< T > Classifier;
/// List of BoostSoftCascade classifier
    typedef std::vector< Classifier > ClassifierList;

public:
  
    /// the optimized version of the SoftCascadeStage classifier
    typedef SoftCascadeClassifier< typename T::OptimizedType > OptimizedType;
    
public:
  
/// A che collection of SoftCascadeStage weak classifier
    ClassifierList m_weak_classifiers;

public:

  /// Create and convert a SoftCascade
    template<class R>
    SoftCascadeClassifier(const SoftCascadeClassifier<R> & src, int scale, int offset, long stride)
    {
        for(typename SoftCascadeClassifier<R>::ClassifierList::const_iterator i = src.m_weak_classifiers.begin(); i!= src.m_weak_classifiers.end(); ++i)
        {
            m_weak_classifiers.push_back( SoftCascadeStage< T > (*i, scale, offset, stride) );
        }
    }

  /// Create and convert a SoftCascade
    template<class R, class Param1Type, class Param2Type>
    SoftCascadeClassifier(const SoftCascadeClassifier<R> & src, Param1Type p1, const Param2Type & p2)
    {
        for(typename SoftCascadeClassifier<R>::ClassifierList::const_iterator i = src.m_weak_classifiers.begin(); i!= src.m_weak_classifiers.end(); ++i)
        {
            m_weak_classifiers.push_back( SoftCascadeStage< T > (true, *i, p1, p2) );
        }
    }
    
/// ctor
    SoftCascadeClassifier();
/// ctor with file with data
    SoftCascadeClassifier(std::istream & in);
/// dtor
    ~SoftCascadeClassifier();

/// Return the number of classifiers
    inline unsigned int length() const  {
        return m_weak_classifiers.size();
    }

    inline void insert(const WeakClassifier & weak, float alpha, float reject)
    {
        m_weak_classifiers.push_back( Classifier(weak, alpha, reject) );
    }

/// propagate signature
    static std::string signature() {
        return Classifier::signature();
    }
    
/// Return the inner list of classifier
inline ClassifierList & list() { return m_weak_classifiers; }
inline const ClassifierList & list() const { return m_weak_classifiers; }    

/// the real computer
    template<class TParam1>
    static float compute(const TParam1 pIntImage, const Classifier *classifier, int n);
    
/// the real computer
    template<class TParam1, class TParam2>
    static float compute(const TParam1 pIntImage, const TParam2 stride, const Classifier *classifier, int n);
    
    template<class FeatureType>
    bool export_features(std::vector<FeatureType> & out) const 
    {
      for(typename ClassifierList::const_iterator i = m_weak_classifiers.begin(); i != m_weak_classifiers.end(); i++)
        out.push_back(*i);
    }

    /** avoid rejection mechanism, for training purpose
     *  @todo make name homogeneous with others mechanism
     */
    template<class TParam1, class TParam2>
    float raw(const TParam1 pIntImage, const TParam2 stride) const
    {
        float acc = 0.0;
        for(int i = 0; i<m_weak_classifiers.size(); i++)
        {
            acc += m_weak_classifiers[i](pIntImage, stride);
        }
        return acc;
    }

/// the operator to perform a classification using the softcascade
/// @return the sum of single classifiers multiplied by weights, or -1 for negative
    template<class TParam1>
    inline float operator()(const TParam1 pIntImage) const {
        return compute(pIntImage, &m_weak_classifiers[0], m_weak_classifiers.size());
    }    
    
/// the operator to perform a classification using the softcascade
/// @return the sum of single classifiers multiplied by weights, or -1 for negative
    template<class TParam1, class TParam2>
    inline float operator()(const TParam1 pIntImage, const TParam2 stride) const {
        return compute(pIntImage, stride, &m_weak_classifiers[0], m_weak_classifiers.size());
    }
};

template<class T>
std::istream & operator >>(std::istream & in, SoftCascadeClassifier<T> & dst);

template<class T>
std::ostream & operator << (std::ostream & out, const SoftCascadeClassifier<T> & src);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline std::istream & operator >> (std::istream & in, SoftCascadeStage<T> & s)
{
    in >> s.rejection >> static_cast< BoostableClassifier<T> &>(s);
    return in;
}

template<class T>
inline std::ostream & operator << (std::ostream & out, const SoftCascadeStage<T> & s)
{
    out << s.rejection << ' '  << static_cast<const BoostableClassifier<T> &>(s);
    return out;
}

// IO
template<class T>
std::istream & operator >>(std::istream & in, SoftCascadeClassifier<T> & dst)
{
    while(in)
    {
        typename SoftCascadeClassifier<T>::Classifier h;
        in >> h;
        if(in)
        {
            dst.m_weak_classifiers.push_back(h);
        }
    }
    return in;
}

// IO
template<class T>
std::ostream & operator << (std::ostream & out, const SoftCascadeClassifier<T> & src)
{
    for(typename SoftCascadeClassifier<T>::ClassifierList::const_iterator i = src.m_weak_classifiers.begin(); i != src.m_weak_classifiers.end(); i++)
        out << *i << '\n';
    return out;
}


template<class T>
SoftCascadeClassifier<T>::SoftCascadeClassifier() { }

template<class T>
SoftCascadeClassifier<T>::~SoftCascadeClassifier() { }

template<class T>
SoftCascadeClassifier<T>::SoftCascadeClassifier(std::istream & in)
{
    while(in)
    {
        typename SoftCascadeClassifier<T>::Classifier h;
        in >> h;
        if(in)
        {
            // std::cout << h << std::endl;
            m_weak_classifiers.push_back(h);
        }
    }
}

template<class T>
template<class TParam1, class TParam2>
float SoftCascadeClassifier<T>::compute(const TParam1 pIntImage, const TParam2 stride, const Classifier *bc, int n)
{
    float acc = 0.0f;
    for(int i = 0; i<n; i++)
    {
        acc += bc[i](pIntImage, stride);
        if(acc < bc[i].rejection) // reject now
            return -1.0f;
    }
    return acc;
}

template<class T>
template<class TParam1>
float SoftCascadeClassifier<T>::compute(const TParam1 pIntImage, const Classifier *bc, int n)
{
    float acc = 0.0f;
    for(int i = 0; i<n; i++)
    {
        acc += bc[i](pIntImage);
        if(acc < bc[i].rejection) // reject now
            return -1.0f;
    }
    return acc;
}

#endif
