#ifndef FUSION_H
#define FUSION_H

#include "../../Data/data_struct.h"
#include "SensorFusion.h"



class Fusion
{
public:
    Fusion() {}
    SF filter;
    FASTRUN void update(Data &data)
    {
        filter.MadgwickUpdate(data.rgyr.x, data.rgyr.y, data.rgyr.z, data.racc.x, data.racc.y, data.racc.z, data.rmag.x, data.rmag.y, data.rmag.z, data.delta_time);
        data.rel_ori.x = Orientation::constrainAngle_whole(filter.getRoll());
        data.rel_ori.y = Orientation::constrainAngle_whole(filter.getPitch());
        data.rel_ori.z = filter.getYaw();
    }

};

#endif