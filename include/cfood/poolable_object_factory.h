#ifndef _CFOOD_POOLABLE_OBJECT_FACTORY_H_
#define _CFOOD_POOLABLE_OBJECT_FACTORY_H_

namespace cfood {

template<typename PoolableObject>
class PoolableObjectFactory {
public:
  PoolableObject* create_object() {
    return new PoolableObject();
  }
  void destroy_object(PoolableObject* obj) {
    delete obj;
  }
};

}
#endif
