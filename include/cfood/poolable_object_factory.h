#ifndef _CFOOD_POOLABLE_OBJECT_FACTORY_H_
#define _CFOOD_POOLABLE_OBJECT_FACTORY_H_

namespace cfood {

  template<typename PoolableObject>
    class PoolableObjectFactory {
  public:
    virtual ~PoolableObjectFactory() {
    }
    virtual PoolableObject* create_object() {
      return new PoolableObject();
    }
    virtual void destroy_object(PoolableObject* obj) {
      delete obj;
    }
    /**
     * called everty time before the object is returned to pool
     */
    virtual void passivate_object(PoolableObject* obj) {
    }
    /**
     * called everty time the object is get from the pool
     */
    virtual void activate_object(PoolableObject* obj) {
      obj->inc_reuse_count();
    }
  };

}
#endif
