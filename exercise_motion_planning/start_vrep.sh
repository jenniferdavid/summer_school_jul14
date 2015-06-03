#!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"
cd "$HERE/V-REP_PRO_EDU_V3_1_2_64_Linux/" && ./vrep.sh $HERE/vcharge_summer_school_python.ttt
