#include "Test.h"

ReportTest ExportStat(const double *r, const int *category, int n_samples, double threshold)
{
    unsigned int cd[2],er[2];
    ReportTest report;
// compute margin
// pmargin contiene il valore del piu' piccolo dei positivi, nmargin il valore del piu' grande dei negativi
    double pmargin = 1000000000.0; // a large positive number
    double nmargin = -1000000000.0; // a large negative number
    cd[0] = er[0] = cd[1] = er[1] = 0;

    double pavg,navg;
    double min_r, max_r;
    pavg=navg = 0.0;
    
    min_r = max_r = r[0];

    for(int i = 0; i<n_samples; i++)
    {
        double ret = r[i];
        
        if(ret > max_r)
          max_r = ret;
        if(ret < min_r)
          min_r = ret;

        if(category[i] == 1)
        {
            // gt is positive
            pavg += ret;

            if(ret < pmargin)
                pmargin = ret;

            if(ret>threshold)
            {
                cd[0]++;
            }
            else
            {
                er[0]++;
            }
        }
        else
        {
            // gt is negative
            navg += ret;
            if(ret > nmargin)
                nmargin = ret;
            if(ret<threshold)
            {
                cd[1]++;
            }
            else
            {
                er[1]++;
            }
        }
    }

    pavg /= (double) (cd[0]+er[0]);
    navg /= (double) (cd[1]+er[1]);
    report.TP = cd[0];
    report.TN = cd[1];
    report.FN = er[0];
    report.FP = er[1];

    report.pmargin = pmargin;
    report.nmargin = nmargin;

    report.print(std::cout);
    std::cout << "Average: " << pavg << "(+) " << navg << "(-)\n";

    if(pmargin>nmargin)
        std::cout << "Margin: " << pmargin-nmargin  << '/' << (max_r - min_r) <<  std::endl;
    else
    {
        int pm, nm;
        pm = nm = 0;
        for(int i = 0; i<n_samples; i++)
        {
            if(category[i] == 1 && r[i]<nmargin)
                pm++;
            if(category[i] == -1 && r[i]>pmargin)
                nm++;
        }

        std::cout << "No separation: " << pmargin-nmargin << '/' <<(max_r - min_r) << " (" << pm+nm << " pattern inside margin: " << pm << " +," << nm << " -)" << std::endl;
    }

    return report;
}

