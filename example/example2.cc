#include "generic_object_pool.h"

class ObjFromFactory : public cfood::PoolableObject<ObjFromFactory> {
public:
  ObjFromFactory() {
    LOG(INFO) << "ObjFromFactory::ObjFromFactory()";
  }
  ~ObjFromFactory() {
    LOG(INFO) << "ObjFromFactory::~ObjFromFactory()";
  }
  void foo() {
    LOG(INFO) << "ObjFromFactory::foo()";
  }
  void* from_other_;
};

namespace cfood {
  template <>
  class PoolableObjectFactory<ObjFromFactory> {
  public:
    PoolableObjectFactory() {}
    PoolableObjectFactory(const size_t buf_size) : buf_size_(buf_size) {
    }
    ObjFromFactory* create_object() {
      LOG(INFO) << "PoolableObjectFactory<ObjFromFactory>::create_object()";
      void* buf = malloc(buf_size_);
      ObjFromFactory* obj = new ObjFromFactory();
      obj->from_other_ = buf;
      return obj;
    }
    void destroy_object(ObjFromFactory* obj) {
      LOG(INFO) << "PoolableObjectFactory<ObjFromFactory>::destroy_object()";
      free(obj->from_other_);
      delete obj;
    }
  private:
    size_t buf_size_;
  };
}

int main(int argc, const char** argv) {
  typedef cfood::GenericObjectPool<ObjFromFactory> PoolType;
  typedef cfood::PoolableObjectFactory<ObjFromFactory> FactoryType;
  size_t max_active = 10;
  size_t max_idle = 5;
  size_t buf_size = 128;
  boost::shared_ptr<FactoryType> factory(new FactoryType(buf_size));
  boost::shared_ptr<PoolType> pool(new PoolType(factory, max_active, max_idle));
  for (int i = 0; i < 40; ++i) {
    // some other code
    {
      boost::shared_ptr<ObjFromFactory> obj = pool->get_object(); 
      obj->foo(); 
      // if some error occur  
      // obj->set_reusable(false); 
    }
  }
  return 0;
}
