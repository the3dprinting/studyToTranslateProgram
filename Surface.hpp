#ifndef slic3r_Surface_hpp_
#define slic3r_Surface_hpp_

#include "ExPolygon.hpp"

namespace Slic3r {

enum SurfaceType { stTop, stBottom, stBottomBridge, stInternal, stInternalSolid, stInternalBridge, stInternalVoid };

class Surface
{
    public:
    SurfaceType     surface_type;
    ExPolygon       expolygon;
    double          thickness;          // in mm
    unsigned short  thickness_layers;   // in layers
    double          bridge_angle;       // in radians, ccw, 0 = East, only 0+ (negative means undefined)
    unsigned short  extra_perimeters;
    
    Surface(SurfaceType _surface_type, const ExPolygon &_expolygon)
        : surface_type(_surface_type), expolygon(_expolygon),
            thickness(-1), thickness_layers(1), bridge_angle(-1), extra_perimeters(0)
        {};
    double area() const;
    bool is_solid() const;   //返回表面的类型是否是solid类型
    bool is_external() const;  //返回表面类型是否是external类型
    bool is_internal() const;  //返回表面类型是否是internal类型
    bool is_bottom() const;    //返回表面类型是否是bottom类型
    bool is_bridge() const;    //返回表面类型是否是bridge类型
    
    #ifdef SLIC3RXS
    void from_SV_check(SV* surface_sv);
    #endif
};

typedef std::vector<Surface> Surfaces;
typedef std::vector<Surface*> SurfacesPtr;

}

#endif
