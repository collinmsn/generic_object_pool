#include "cfood/generic_object_pool.h"

class SpecialObj : public cfood::PoolableObject {
public:
  SpecialObj() {
    LOG(INFO) << "SpecialObj::SpecialObj()";
  }
  ~SpecialObj() {
    LOG(INFO) << "SpecialObj::~SpecialObj()";
  }
  void foo() {
    LOG(INFO) << "SpecialObj::foo()";
  }
  void* from_other_;
};

class SpecialObjFactory : public cfood::PoolableObjectFactory<SpecialObj> {
public:
  SpecialObjFactory(const size_t buf_size) : buf_size_(buf_size) {                                                              
  }                                                                                                                                 
  SpecialObj* create_object() {                                                                                                 
    LOG(INFO) << "PoolableObjectFactory<SpecialObj>::create_object()";                                                          
    void* buf = malloc(buf_size_);                                                                                                  
    SpecialObj* obj = new SpecialObj();                                                                                     
    obj->from_other_ = buf;                                                                                                         
    return obj;                                                                                                                     
  } 
  void destroy_object(SpecialObj* obj) {                                                                                        
    LOG(INFO) << "PoolableObjectFactory<SpecialObj>::destroy_object()";                                                         
    free(obj->from_other_);                                                                                                         
    delete obj;                                                                                                                     
  } 
private:
  size_t buf_size_;
};


int main(int argc, const char** argv) {
  typedef cfood::GenericObjectPool<SpecialObj> PoolType;
  size_t max_idle = 5;
  size_t max_active = 10;
  size_t buf_size = 128;
  boost::shared_ptr<SpecialObjFactory> factory(new SpecialObjFactory(buf_size));
  boost::shared_ptr<PoolType> pool(new PoolType(factory, max_idle, max_active));
  for (int i = 0; i < 40; ++i) {
    // some other code
    {
      boost::shared_ptr<SpecialObj> obj = pool->get_object(); 
      obj->foo(); 
      // if some error occur  
      // obj->set_reusable(false); 
    }
  }
  return 0;
}
