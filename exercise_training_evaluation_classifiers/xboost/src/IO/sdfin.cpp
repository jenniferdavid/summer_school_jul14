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

#include "IO/sdfin.h"
#include <sstream>
#include <iostream>

sdfin::sdfin(const char *set_description_file)
{
    std::ifstream in (set_description_file);
    std::string str;

    int category = 0;
    bool category_is_valid = false;
    bool negative_mode = true;
    
    read_point = 0;
    
    if(!in)
    {
      std::cerr << "Error: Failed to open \"" << set_description_file << '\"' << std::endl;
    }
    else
    while(getline(in, str))
    {
        if(str[0]=='#')
        {
            std::istringstream opt(str);
            char c;
            std::string cmd;
            opt >> c >> cmd;
            if(cmd=="CLASS")
            {
                opt >> category;
                category_is_valid = true;
                negative_mode = false;
            }
            else if(cmd=="AUTO")
            {
                category_is_valid = false;
                negative_mode = false;
            }
            else if(cmd=="DEFAULT")
            {
                category_is_valid = false;
                negative_mode = true; // All the image could be used as negatives source
            }
            else
                std::cerr << "Unknown command: " << cmd << std::endl;
        }
        else
        {
            // parse command line
            std::istringstream opt(str);
            datasetitem item;

            opt >> item.filename;

            if(!item.filename.empty())
            {
                item.auto_negative = negative_mode; // DEFAULT mode = true

                // ROI, optional.
                do {
                    datasetobject obj;
                    obj.weight = 1.0;
                    // NOTE CLASS with a ROI with a different category is quite weird!
                    opt >> obj.category >> obj.roi.x0 >> obj.roi.y0 >> obj.roi.x1 >> obj.roi.y1;
                    if(opt)
                    {
                        item.object.push_back(obj);
                    }
                } while(opt);

                // CLASS or AUTO with no object in ROI
                if((!negative_mode) && item.object.empty())
                {
                    if(!category_is_valid)
                        std::cerr << "Invalid syntax for AUTO mode" << std::endl;
                    else
                    {
                      // 0,0,0,0 = entire frame
                        datasetobject dummy;
                        dummy.category = category;
                        dummy.roi = rect(0,0,0,0);
                        item.object.push_back(dummy);
                    }
                }

                m_list.push_back(item);
            }
        }

    }

}

sdfin::~sdfin()
{
}

bool sdfin::next(datasetitem & item)
{
    if(read_point>=m_list.size())
      return false;

    item = m_list[read_point];
    read_point ++ ;
    return true;
}