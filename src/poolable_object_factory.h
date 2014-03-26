#ifndef _OBJECT_FACTORY_H_
#define _OBJECT_FACTORY_H_

class PoolObject;
class ObjectFactory {
public:
  virtual ~ObjectFactory() {
  }
  virtual PoolObject* create_object() = 0;
};

template<typename ObjType>
class DefaultObjectFactory : public ObjectFactory {
  virtual PoolObject* create_object() {
    return new ObjType();
  }
};

#endif
