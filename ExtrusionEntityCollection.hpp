#ifndef slic3r_ExtrusionEntityCollection_hpp_
#define slic3r_ExtrusionEntityCollection_hpp_

#include <myinit.h>
#include "ExtrusionEntity.hpp"

namespace Slic3r {

class ExtrusionEntityCollection : public ExtrusionEntity
{
    public:
    ExtrusionEntityCollection* clone() const;   //����һ������ĸ��Ƶ��µ�ָ��
    ExtrusionEntitiesPtr entities;      //typedef std::vector<ExtrusionEntity*> ExtrusionEntitiesPtr;ʹ�ø���ָ��˵�����ݿ�path��loop
    std::vector<size_t> orig_indices;  // handy for XS
    bool no_sort;    //
    ExtrusionEntityCollection(): no_sort(false) {};  //������Ĭ��sort��
    ExtrusionEntityCollection(const ExtrusionEntityCollection &collection);        //
    ExtrusionEntityCollection& operator= (const ExtrusionEntityCollection &other); //�൱��swap��һ��other������other����
    bool is_collection() const {
        return true;
    };
    bool can_reverse() const {  //û��sort�Ͳ���reverse
        return !this->no_sort;
    };
    void swap (ExtrusionEntityCollection &c);  //�������entities��orig_indices��no_sort�봫���c����
    void chained_path(ExtrusionEntityCollection* retval, bool no_reverse = false, std::vector<size_t>* orig_indices = NULL) const;
    void chained_path_from(Point start_near, ExtrusionEntityCollection* retval, bool no_reverse = false, std::vector<size_t>* orig_indices = NULL) const;
    void reverse();   //entities�����loop��ת��Ȼ��entities���巴ת
    Point first_point() const;   //entities�����һ����Ŀ��first_point()
    Point last_point() const;    //entities�������һ����Ŀ��last_point()
    Polygons grow() const;       //
    size_t items_count() const;  //
    void flatten(ExtrusionEntityCollection* retval) const;
    double min_mm3_per_mm() const;   //
};

}

#endif
