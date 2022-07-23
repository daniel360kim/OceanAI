#ifndef FUSION_H
#define FUSION_H

#include "../../Data/data_struct.h"
#include "SensorFusion.h"



class Fusion
{
public:
    Fusion() {}
    SF filter;
    void update(Data &data)
    {
        filter.MadgwickUpdate(data.fgyr.x, data.fgyr.y, data.fgyr.z, data.facc.x, data.facc.y, data.facc.z, data.mag.x, data.mag.y, data.mag.z, data.dt);
        data.rel_ori.x = Orientation::constrainAngle_whole(filter.getRoll());
        data.rel_ori.y = Orientation::constrainAngle_whole(filter.getPitch());
        data.rel_ori.z = filter.getYaw();
    }

};

#endif