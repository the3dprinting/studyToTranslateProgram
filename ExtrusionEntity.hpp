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

class ExtrusionEntity   //�����࣬���麯����
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

class ExtrusionPath : public ExtrusionEntity   //����·����
{
    public:
    Polyline polyline;   //����������߶�
    ExtrusionRole role;  //ö�����һ��Ԫ�أ���������������
	double mm3_per_mm;   // mm^3 of plastic per mm of linear head motion
	float width;         //���
    float height;        //·���߶�
    
    ExtrusionPath(ExtrusionRole role) : role(role), mm3_per_mm(-1), width(-1), height(-1) {};   //���캯������������·����ɫ
    ExtrusionPath* clone() const;   //��¡����·����ָ��һ��
    void reverse();    //����polyline��ת
    Point first_point() const;   //���ر���polyline�ĵ�һ����
    Point last_point() const;    //���ر���polyline�����һ����
    void intersect_expolygons(const ExPolygonCollection &collection, ExtrusionEntityCollection* retval) const;  //����polyline��collection�󽻺󣬽���洢��retval->entities��
    void subtract_expolygons(const ExPolygonCollection &collection, ExtrusionEntityCollection* retval) const;   //����polyline��collection���󣬽���洢��retval->entities��
    void clip_end(double distance);    //�ӱ���pollyline�е����һ�����ȥdistance������߶Σ����distance�������߶γ��ȣ���PointsΪ��
    void simplify(double tolerance);   //������߶Σ�ֻҪ����Ҹ�С��tolerance���ɡ�
    double length() const;      //���ر���polyline���ܳ���
    bool is_perimeter() const;   //���ر����Ƿ���erPerimeter����erExternalPerimeter����erOverhangPerimeter
    bool is_infill() const;   //���ر����Ƿ���erBridgeInfill����erInternalInfill���� erSolidInfill����erTopSolidInfill
    bool is_solid_infill() const;  //���ر����Ƿ���erBridgeInfill����erSolidInfill����erTopSolidInfill
    bool is_bridge() const;  //���ر����Ƿ���erBridgeInfill����erOverhangPerimeter
    Polygons grow() const;   
    double min_mm3_per_mm() const {
        return this->mm3_per_mm;
    };

    private:
    void _inflate_collection(const Polylines &polylines, ExtrusionEntityCollection* collection) const;  //polylines�洢��collection->entities��
};

typedef std::vector<ExtrusionPath> ExtrusionPaths;   //����ö�ExtrusionPath�����

class ExtrusionLoop : public ExtrusionEntity   //����ѭ����
{
    public:
    ExtrusionPaths paths;     //�����кö༷��·��
    ExtrusionLoopRole role;   //����ѭ���Ľ�ɫ��������������
    
    ExtrusionLoop(ExtrusionLoopRole role = elrDefault) : role(role) {};
    bool is_loop() const {
        return true;
    };
    bool can_reverse() const {
        return false;
    };
    ExtrusionLoop* clone() const;   //��¡����·����ָ��һ��
    bool make_clockwise();          //�������polygon�������ж��������Ƿ�reverse()
    bool make_counter_clockwise();  //�������polygon�������ж��������Ƿ�reverse()
    void reverse();                 //��ת����paths��ÿ��path�ĵ��˳��
    Point first_point() const;      //����paths�ĵ�һ����
    Point last_point() const;       //����paths�����һ���㣬ͬ��һ���㣨����
    Polygon polygon() const;        //����paths�����path�����ϲ���һ�����polygon��
    double length() const;          //���ر���paths������ܳ���
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
