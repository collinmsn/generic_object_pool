#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <glog/logging.h>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "pool_object.h"
#include "object_factory.h"
#ifdef UNIT_TEST
#include <gflags/glfags.h>
DEFINE_int32(num_obj_directly_freed, 0, "number of objects that being directly freed because object pool has gone away");
#endif

// Note: ObjType must be derived from PoolObject
template<typename ObjType>
class ObjectPool : public boost::enable_shared_from_this<ObjectPool<ObjType> > {
  friend class Deleter;
private:
  class Deleter {
 public:
    Deleter (boost::weak_ptr<ObjectPool<ObjType> > pool) : pool_(pool) {
    }
    void operator()(ObjType* obj) {
      if (!obj) return;
      boost::shared_ptr<ObjectPool<ObjType> > pool = pool_.lock();
      if (!pool) {
        LOG(WARNING) << "object pool has gone away";
        delete obj;
        #ifdef UNIT_TEST
        ++FLAGS_num_obj_directly_freed;
        #endif
      }
      else {
        pool->return_object(obj);
      }
    }
 private:
    boost::weak_ptr<ObjectPool<ObjType> > pool_;
  };
public:
ObjectPool(const size_t max_obj_num, boost::shared_ptr<ObjectFactory> factory = boost::shared_ptr<ObjectFactory>()) : max_obj_num_(max_obj_num), obj_num_(0), factory_(factory) {
    BOOST_STATIC_ASSERT_MSG((boost::is_base_of<PoolObject, ObjType>::value), "ObjType must be derived class of PoolObject");
    if (!factory_) {
      factory_ = boost::shared_ptr<ObjectFactory>(new DefaultObjectFactory<ObjType>());
    }
  }
  ~ObjectPool() {
    for (size_t i = 0; i < objects_.size(); ++i) {
      delete objects_[i];
    }
  }
  boost::shared_ptr<ObjType> get_object() {
    boost::shared_ptr<ObjectPool<ObjType> > self_shared_ptr(this->shared_from_this());
    boost::weak_ptr<ObjectPool<ObjType> > self(self_shared_ptr);
    boost::mutex::scoped_lock lock(mutex_);
    if (!objects_.empty()) {
      ObjType* obj = objects_.back();
      objects_.pop_back();
      return boost::shared_ptr<ObjType>(obj, Deleter(self));
    }
    if (obj_num_ >= max_obj_num_) {
      return boost::shared_ptr<ObjType>();
    }
    ObjType* obj = dynamic_cast<ObjType*>(factory_->create_object());
    if (!obj) return boost::shared_ptr<ObjType>();
    ++obj_num_;
    return boost::shared_ptr<ObjType>(obj, Deleter(self));
  }

  size_t size() const {
    return obj_num_;
  }

 private:
  void return_object(ObjType* obj) {
    if (!obj) return;

    boost::mutex::scoped_lock lock(mutex_);
    if (!obj->reusable()) {
      delete obj;
      --obj_num_;
    }
    else {
      objects_.push_back(obj);
    }
  }
private:
  const size_t max_obj_num_;
  size_t obj_num_;
  boost::shared_ptr<ObjectFactory> factory_;
  std::vector<ObjType*> objects_;
  boost::mutex mutex_;
};
#endif
