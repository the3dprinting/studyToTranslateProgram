#ifndef slic3r_Extruder_hpp_
#define slic3r_Extruder_hpp_

#include <myinit.h>
#include "Point.hpp"
#include "PrintConfig.hpp"

namespace Slic3r {

class Extruder
{
    public:
    int id;   //ʶ�����id�ţ�����config����Ĳ����ö���Ҫ�������id������ȡ
    double E;  //
    double absolute_E;  //
    double retracted;   //
    double restart_extra;
    double e_per_mm3;
    double retract_speed_mm_min;
    
    Extruder(int id, GCodeConfig *config);
    virtual ~Extruder() {}
    void reset();  //�����E��absolute_E��retracted��restart_extra����Ϊ0
    double extrude(double dE);  //
    double retract(double length, double restart_extra);
    double unretract();
    double e_per_mm(double mm3_per_mm) const;
    double extruded_volume() const;
    double used_filament() const;
    
    double filament_diameter() const;      //˿ֱ��,ʵ�ʷ���config��������ֵ
    double extrusion_multiplier() const;   //��������,ʵ�ʷ���config��������ֵ
    double retract_length() const;    //��������,ʵ�ʷ���config��������ֵ
    double retract_lift() const;      //��������,ʵ�ʷ���config��������ֵ
    int retract_speed() const;        //�����ٶ�,ʵ�ʷ���config��������ֵ
    double retract_restart_extra() const;    //û����������˼,ʵ�ʷ���config��������ֵ
    double retract_length_toolchange() const;    //û����������˼,ʵ�ʷ���config��������ֵ
    double retract_restart_extra_toolchange() const;    //û����������˼,ʵ�ʷ���config��������ֵ
    
    private:
    GCodeConfig *config;
};

}

#endif
