#ifndef slic3r_SurfaceCollection_hpp_
#define slic3r_SurfaceCollection_hpp_

#include "Surface.hpp"
#include <vector>

namespace Slic3r {

class SurfaceCollection
{
    public:
    Surfaces surfaces;
    
    operator Polygons() const;  //����ת����������������һ��surfaceת��ΪPolygons��
    operator ExPolygons() const;   //����ת����������������һ��surfaceת��ΪExPolygons��
    void simplify(double tolerance);  //������һ��surface�����expolygon�����Ҹ����Ϊtolerance��
    void group(std::vector<SurfacesPtr> *retval);  //ͨ�����е����Խ�surfaces����
    template <class T> bool any_internal_contains(const T &item) const;  //surfaces�����Ƿ���internal���͵�surface����item
    template <class T> bool any_bottom_contains(const T &item) const;   //surfaces�����Ƿ���bottom���͵�surface����item
    SurfacesPtr filter_by_type(SurfaceType type);   //��surfaces������type���͵���ѡ��������
    void filter_by_type(SurfaceType type, Polygons* polygons);  //��surfaces������type���͵���ѡ�����ŵ�polygonsL����
};

}

#endif