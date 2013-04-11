#ifndef STREAM_UTILS_H
#define STREAM_UTILS_H


#include <map>
#include <string>
#include <vector>
#include <list>
#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/deque.hpp"
#include <exprtk.hpp>
#include "objects/abstract_object.h"
#include "objects/task_assignment_task.h"
#include "objects/task_assignment_task.h"
#include <iostream>
#include <fstream>
#include <forward_list>

template< typename K,typename T>//,typename C, typename Alloc >
std::ostream& operator<<( std::ostream& os, const std::map<K,T>& m )
{
    for (typename std::map<K,T>::const_iterator it=m.begin();it!=m.end();++it)
        os<<it->first <<" "<<it->second <<" ";
    os<<std::endl;
    return os;
}

template< typename T>//,typename C, typename Alloc >
std::ostream& operator<<( std::ostream& os, const std::vector<T>& m )
{
    for (typename std::vector<T>::const_iterator it=m.begin();it!=m.end();++it)
        os<<(*it)<<" ";
    return os;
}

#endif