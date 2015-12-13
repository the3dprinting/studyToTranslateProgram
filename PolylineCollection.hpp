#ifndef slic3r_PolylineCollection_hpp_
#define slic3r_PolylineCollection_hpp_

#include <myinit.h>
#include "Polyline.hpp"

namespace Slic3r {

class PolylineCollection
{
    public:
    Polylines polylines;
    void chained_path(PolylineCollection* retval, bool no_reverse = false) const;  //ͬ�£�ֻ����ʼ��Ϊ��һ��poliline�ĵ�һ����
    void chained_path_from(Point start_near, PolylineCollection* retval, bool no_reverse = false) const;//��start_near��ʼ�ҵ�polylines
    //���polyline���������ʼ�㣬Ȼ������polyline�����յ㵱����һ����ʼ�㣬���չ滮�õ�polyline�洢��retval���棬boolΪ���ʾ�滮ʱ
    //polyline���Է�ת����һ��polyline����������յ������������Է�ת���polyline    
    Point leftmost_point() const;  //����Polylines��������ߵĵ�
};

}

#endif
