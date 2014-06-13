#ifndef _CFOOD_GENERIC_OBJECT_POOL_H_
#define _CFOOD_GENERIC_OBJECT_POOL_H_
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <glog/logging.h>

#include "poolable_object.h"
#include "poolable_object_factory.h"

namespace cfood {

  template<typename T>
    class GenericObjectPool : public boost::enable_shared_from_this<GenericObjectPool<T> > {
  public:
    typedef T ObjType;
    typedef GenericObjectPool<ObjType> PoolType;
    typedef PoolableObjectFactory<ObjType> FactoryType;
    typedef std::list<ObjType*> ObjContainer;
  private:
    friend class Deleter;
    class Deleter {
    public:
      Deleter (boost::shared_ptr<PoolType> pool) : pool_(pool) {
      }
      void operator()(ObjType* obj) {
	pool_->return_object(obj);
      }
    private:
      boost::shared_ptr<PoolType> pool_;
    };
  public:
    /**
     * @param max_idle controls the maximum number of objects that can sit idle in the pool at any time. 
     * @param max_active controls the maximum number of objects that can be allocated by the pool 
     *        (checked out to clients, or idle awaiting checkout) at a given time.
     *
     */
  GenericObjectPool(boost::shared_ptr<FactoryType> factory = boost::shared_ptr<FactoryType>(), 
		    const size_t max_idle = -1,
		    const size_t max_active = -1)
    : factory_(factory), max_idle_(max_idle), max_active_(max_active), 
      active_obj_num_(0), idle_obj_num_(0) {
      BOOST_STATIC_ASSERT_MSG((boost::is_base_of<PoolableObject, ObjType>::value), 
			      "Object type must be derived class of PoolObject");
      if (!factory_) {
	factory_.reset(new FactoryType());
      }
    }
    ~GenericObjectPool() {
      for (typename ObjContainer::iterator it = objects_.begin(); it != objects_.end(); ++it) {
	factory_->destroy_object(*it);
      }
    }
    boost::shared_ptr<ObjType> get_object() {
      ObjType* obj = NULL;
      boost::shared_ptr<PoolType> self(this->shared_from_this());
      boost::lock_guard<boost::mutex> lock(mutex_);
      if (idle_obj_num_ > 0) {
	obj = objects_.front();
	objects_.pop_front();
	--idle_obj_num_;
      } else {
	if (active_obj_num_ < max_active_) {
	  obj = factory_->create_object();
	  if (obj) {
	    ++active_obj_num_;
	  }
	}
      }

      if (obj) {
	factory_->activate_object(obj);
	DLOG(INFO) << "obj reuse count: " << obj->reuse_count();
      }
      return boost::shared_ptr<ObjType>(obj, Deleter(self));
    }
    
    size_t active_object() const {
      return active_obj_num_;
    }
  private:
    void return_object(ObjType* obj) {
      if (!obj) return;
      factory_->passivate_object(obj);

      boost::lock_guard<boost::mutex> lock(mutex_);
      if (!obj->reusable() || idle_obj_num_ >= max_idle_) {
	factory_->destroy_object(obj);
	--active_obj_num_;
	return;
      }
      objects_.push_back(obj);
      ++idle_obj_num_;
    }
  private:
    boost::shared_ptr<FactoryType> factory_;
    const size_t max_idle_;
    const size_t max_active_;
    volatile size_t active_obj_num_;
    volatile size_t idle_obj_num_;
    ObjContainer objects_;
    boost::mutex mutex_;
  };

}
#endif
