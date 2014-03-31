generic_object_pool
===================

A generic object pool for c++ which is easy to use and high performance


GenericObjectPool是一个简单易用的对象池，你可以使用它轻松地pool那些创建和销毁成本比较高的对象。和java的对象池不一样，得益于c++的RAII特性，你不需要在borrow_object后还要去处理return_object，你只需要简单地从pool中去get_object就可以了，object在你使用完后会自动return到pool中。如果在使用中发生异常而使得这个object不能再重复使用, set_reusable(false)就行了，这样pool会自动销毁它。

Pool中的对象都是PoolableObject，也就是说你放到pool中的对象需要继承自PoolableObject。某些情况下，你还需要提供相应的factory。

Getting started:

首先定义你想pool的object

```cpp
class SomeObject : public cfood::PoolableObject {
};
```
然后你构造一个object pool

```cpp
typedef cfood::GenericObjectPool<SomeObject> PoolType;
boost::shared_ptr<PoolType> pool(new PoolType());
```
然后你就可以在程序的各个地方各个线程中使用pool取得object，没错，GenericObjectPool是线程安全的。

```cpp
{
      boost::shared_ptr<SomeObject> obj = pool->get_object(); 
      try {
        obj->foo(); 
      }
      catch (const std::exception& e) {
        obj->set_reusable(false);
      }
}
```

你可能需要设置这个pool中最多保留多少idle object, GenericObjectPool的max_idle就是控制它的，默认这个参数是-1，表示没有限制。如果你还想控制同时存在的object数，那max_active就是干这个的。

PoolableObjectFactory用来创建和销毁池中的对象，如果它的默认实现不能满足你的需要，你可以override相关方法，例如

```cpp
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
```

然后这样使用
```cpp
typedef SpecialObjFactory FactoryType;
typedef cfood::GenericObjectPool<SpecialObj> PoolType;
boost::shared_ptr<FactoryType> factory(new FactoryType(128));
const size_t max_idle = 10;
boost::shared_ptr<PoolType> pool(new PoolType(factory, max_idle));
pool->get_object();
```
