generic_object_pool
===================

A generic object pool for c++ which is easy to use and high performance


GenericObjectPool是一个简单易用的对象池，你可以使用它轻松地pool那些创建和销毁成本比较高的对象。和java的对象池不一样，得益于c++的RAII特性，你不需要在borrow_object后还要去处理return_object，你只需要简单地从pool中去get_object就可以了，object在你使用完后会自动return到pool中。如果在使用中发生异常而使得这个object不能再重复使用, set_reusable(false)就行了，这样pool会自动销毁它。

Pool中的对象都是PoolableObject，也就是说你放到pool中的对象需要继承自PoolableObject。某些情况下，
你还需要提供相应的factory。

Getting started:

首先定义你想pool的object

class SomeObject : public cfood::PoolableObject<SomeObject> {
};

然后你构造一个object pool

typedef cfood::GenericObjectPool<SomeObject> PoolType;
boost::shared_ptr<PoolType> pool(new PoolType());

然后你就可以在程序的各个地方各个线程中使用pool取得object，没错，GenericObjectPool是线程安全的。

{
      boost::shared_ptr<SomeObject> obj = pool->get_object(); 
      try {
        obj->foo(); 
      }
      catch (const std::exception& e) {
        obj->set_reusable(false);
      }
}



