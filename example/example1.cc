#include "generic_object_pool.h"

class SomeObject : public cfood::PoolableObject<SomeObject> {
public:
  SomeObject() {
    LOG(INFO) << "SomeObject::SomeObject()";
  }
  ~SomeObject() {
    LOG(INFO) << "SomeObject::~SomeObject()";
  }
  void foo() {
    LOG(INFO) << "SomeObject::foo()";
  }
};
int main(int argc, const char** argv) {
  typedef cfood::GenericObjectPool<SomeObject> PoolType;
  typedef cfood::PoolableObjectFactory<SomeObject> FactoryType;
  size_t max_active = 10;
  size_t max_idle = 5;
  boost::shared_ptr<PoolType> pool(new PoolType(boost::shared_ptr<FactoryType>(), max_active, max_idle));
  boost::shared_ptr<SomeObject> obj;
  for (int i = 0; i < 40; ++i) {
    // some other code
    {
      obj = pool->get_object();
      obj->foo();
    }
  }
  return 0;
}
