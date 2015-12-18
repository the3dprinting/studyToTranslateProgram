#ifndef slic3r_Extruder_hpp_
#define slic3r_Extruder_hpp_

#include <myinit.h>
#include "Point.hpp"
#include "PrintConfig.hpp"

namespace Slic3r {

class Extruder
{
    public:
    int id;   //识别本身的id号，调用config里面的参数好多需要根据这个id号来提取
    double E;  //
    double absolute_E;  //
    double retracted;   //
    double restart_extra;
    double e_per_mm3;
    double retract_speed_mm_min;
    
    Extruder(int id, GCodeConfig *config);
    virtual ~Extruder() {}
    void reset();  //本身的E、absolute_E、retracted、restart_extra设置为0
    double extrude(double dE);  //
    double retract(double length, double restart_extra);
    double unretract();
    double e_per_mm(double mm3_per_mm) const;
    double extruded_volume() const;
    double used_filament() const;
    
    double filament_diameter() const;      //丝直径,实际返回config里面的相关值
    double extrusion_multiplier() const;   //挤出倍数,实际返回config里面的相关值
    double retract_length() const;    //回缩长度,实际返回config里面的相关值
    double retract_lift() const;      //回缩提升,实际返回config里面的相关值
    int retract_speed() const;        //回缩速度,实际返回config里面的相关值
    double retract_restart_extra() const;    //没看懂字面意思,实际返回config里面的相关值
    double retract_length_toolchange() const;    //没看懂字面意思,实际返回config里面的相关值
    double retract_restart_extra_toolchange() const;    //没看懂字面意思,实际返回config里面的相关值
    
    private:
    GCodeConfig *config;
};

}

#endif
