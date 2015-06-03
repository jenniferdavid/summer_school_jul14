
# svn co http://svn/tools/AdaBoost/trunk/ adaboost
svn export http://svn/tools/AdaBoost/trunk/ adaboost
#cp include adaboost/include
#cp src adaboost/src

tar cvjf ../adaboost.tar.bz2 adaboost 

rm -rf adaboost
