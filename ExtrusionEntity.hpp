#ifndef slic3r_ExtrusionEntity_hpp_
#define slic3r_ExtrusionEntity_hpp_

#include <myinit.h>
#include "Polygon.hpp"
#include "Polyline.hpp"

namespace Slic3r {

class ExPolygonCollection;
class ExtrusionEntityCollection;
class Extruder;

/* Each ExtrusionRole value identifies a distinct set of { extruder, speed } */
enum ExtrusionRole {
    erNone,
    erPerimeter,
    erExternalPerimeter,
    erOverhangPerimeter,
    erInternalInfill,
    erSolidInfill,
    erTopSolidInfill,
    erBridgeInfill,
    erGapFill,
    erSkirt,
    erSupportMaterial,
    erSupportMaterialInterface,
};

/* Special flags describing loop */
enum ExtrusionLoopRole {
    elrDefault,
    elrContourInternalPerimeter,
};

class ExtrusionEntity   //抽象类，纯虚函数！
{
    public:
    virtual bool is_collection() const {
        return false;
    };
    virtual bool is_loop() const {
        return false;
    };
    virtual bool can_reverse() const {
        return true;
    };
    virtual ExtrusionEntity* clone() const = 0;
    virtual ~ExtrusionEntity() {};
    virtual void reverse() = 0;
    virtual Point first_point() const = 0;
    virtual Point last_point() const = 0;
    virtual Polygons grow() const = 0;
    virtual double min_mm3_per_mm() const = 0;
};

typedef std::vector<ExtrusionEntity*> ExtrusionEntitiesPtr;

class ExtrusionPath : public ExtrusionEntity   //挤出路径类
{
    public:
    Polyline polyline;   //本身的所有线段
    ExtrusionRole role;  //枚举类的一个元素，代表其自身的类别！
	double mm3_per_mm;   // mm^3 of plastic per mm of linear head motion
	float width;         //宽度
    float height;        //路径高度
    
    ExtrusionPath(ExtrusionRole role) : role(role), mm3_per_mm(-1), width(-1), height(-1) {};   //构造函数，仅仅传入路径角色
    ExtrusionPath* clone() const;   //克隆本身路径的指针一个
    void reverse();    //本身polyline反转
    Point first_point() const;   //返回本身polyline的第一个点
    Point last_point() const;    //返回本身polyline的最后一个点
    void intersect_expolygons(const ExPolygonCollection &collection, ExtrusionEntityCollection* retval) const;  //本身polyline和collection求交后，结果存储在retval->entities里
    void subtract_expolygons(const ExPolygonCollection &collection, ExtrusionEntityCollection* retval) const;   //本身polyline和collection求差后，结果存储在retval->entities里
    void clip_end(double distance);    //从本身pollyline中的最后一个点减去distance距离的线段，如果distance大于总线段长度，则Points为空
    void simplify(double tolerance);   //化简多线段，只要误差弦高小于tolerance即可。
    double length() const;      //返回本身polyline的总长度
    bool is_perimeter() const;   //返回本身是否是erPerimeter或者erExternalPerimeter或者erOverhangPerimeter
    bool is_infill() const;   //返回本身是否是erBridgeInfill或者erInternalInfill或者 erSolidInfill或者erTopSolidInfill
    bool is_solid_infill() const;  //返回本身是否是erBridgeInfill或者erSolidInfill或者erTopSolidInfill
    bool is_bridge() const;  //返回本身是否是erBridgeInfill或者erOverhangPerimeter
    Polygons grow() const;   
    double min_mm3_per_mm() const {
        return this->mm3_per_mm;
    };

    private:
    void _inflate_collection(const Polylines &polylines, ExtrusionEntityCollection* collection) const;  //polylines存储到collection->entities里
};

typedef std::vector<ExtrusionPath> ExtrusionPaths;   //定义好多ExtrusionPath的组合

class ExtrusionLoop : public ExtrusionEntity   //挤出循环类
{
    public:
    ExtrusionPaths paths;     //里面有好多挤出路径
    ExtrusionLoopRole role;   //挤出循环的角色，代表自身的类别
    
    ExtrusionLoop(ExtrusionLoopRole role = elrDefault) : role(role) {};
    bool is_loop() const {
        return true;
    };
    bool can_reverse() const {
        return false;
    };
    ExtrusionLoop* clone() const;   //克隆本身路径的指针一个
    bool make_clockwise();          //本身进行polygon（）后，判断来绝对是否reverse()
    bool make_counter_clockwise();  //本身进行polygon（）后，判断来绝对是否reverse()
    void reverse();                 //反转本身paths和每个path的点的顺序
    Point first_point() const;      //所有paths的第一个点
    Point last_point() const;       //所有paths的最后一个点，同第一个点（？）
    Polygon polygon() const;        //所有paths里面的path接连合并到一起，组成polygon类
    double length() const;          //返回本身paths里面的总长度
    bool split_at_vertex(const Point &point);    //
    void split_at(const Point &point);           //
    void clip_end(double distance, ExtrusionPaths* paths) const; //
    bool has_overhang_point(const Point &point) const;    //
    bool is_perimeter() const;      //
    bool is_infill() const;         //
    bool is_solid_infill() const;   //
    Polygons grow() const;          //
    double min_mm3_per_mm() const;  //
};

}

#endif
