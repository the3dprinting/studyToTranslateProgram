#ifndef slic3r_ExtrusionEntityCollection_hpp_
#define slic3r_ExtrusionEntityCollection_hpp_

#include <myinit.h>
#include "ExtrusionEntity.hpp"

namespace Slic3r {

class ExtrusionEntityCollection : public ExtrusionEntity
{
    public:
    ExtrusionEntityCollection* clone() const;   //返回一个本身的复制的新的指针
    ExtrusionEntitiesPtr entities;      //typedef std::vector<ExtrusionEntity*> ExtrusionEntitiesPtr;使用父类指针说明内容可path可loop
    std::vector<size_t> orig_indices;  // handy for XS
    bool no_sort;    //
    ExtrusionEntityCollection(): no_sort(false) {};  //构造是默认sort了
    ExtrusionEntityCollection(const ExtrusionEntityCollection &collection);        //
    ExtrusionEntityCollection& operator= (const ExtrusionEntityCollection &other); //相当于swap了一下other，但是other不变
    bool is_collection() const {
        return true;
    };
    bool can_reverse() const {  //没有sort就不能reverse
        return !this->no_sort;
    };
    void swap (ExtrusionEntityCollection &c);  //将本身的entities、orig_indices和no_sort与传入的c交换
    void chained_path(ExtrusionEntityCollection* retval, bool no_reverse = false, std::vector<size_t>* orig_indices = NULL) const;
    void chained_path_from(Point start_near, ExtrusionEntityCollection* retval, bool no_reverse = false, std::vector<size_t>* orig_indices = NULL) const;
    void reverse();   //entities里面非loop反转，然后entities整体反转
    Point first_point() const;   //entities里面第一个条目的first_point()
    Point last_point() const;    //entities里面最后一个条目的last_point()
    Polygons grow() const;       //
    size_t items_count() const;  //
    void flatten(ExtrusionEntityCollection* retval) const;
    double min_mm3_per_mm() const;   //
};

}

#endif
