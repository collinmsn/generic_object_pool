#ifndef _CFOOD_POOLABLE_OBJECT_FACTORY_H_
#define _CFOOD_POOLABLE_OBJECT_FACTORY_H_

namespace cfood {

template<typename PoolableObject>
class ObjectFactory {
public:
  PoolableObject* create_object() {
    return new PoolableObject();
  }
};

}
#endif
