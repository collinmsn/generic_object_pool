#ifndef _CFOOD_GENERIC_OBJECT_POOL_H_
#define _CFOOD_GENERIC_OBJECT_POOL_H_
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <glog/logging.h>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/enable_shared_from_this.hpp>

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
  GenericObjectPool(boost::shared_ptr<FactoryType> factory, const size_t max_active, const size_t max_idle)
    : factory_(factory), max_active_(max_active), max_idle_(max_idle), active_obj_num_(0) {
      BOOST_STATIC_ASSERT_MSG((boost::is_base_of<PoolableObject<ObjType>, ObjType>::value), 
			      "Object type must be derived class of PoolObject");
      if (!factory_) {
	factory_.reset(new FactoryType());
      }
    }
    ~GenericObjectPool() {
      for (ObjContainer::iterator it = objects_.begin(); it != objects_.end(); ++it) {
	factory_->destroy_object(*it);
      }
    }
    boost::shared_ptr<ObjType> get_object() {
      boost::shared_ptr<PoolType> self(this->shared_from_this());
      boost::lock_guard<boost::mutex> lock(mutex_);
      if (!objects_.empty()) {
	ObjType* obj = objects_.front();
	objects_.pop_front();
	return boost::shared_ptr<ObjType>(obj, Deleter(self));
      }
      if (active_obj_num_ >= max_active_) {
	return boost::shared_ptr<ObjType>();
      }
      ObjType* obj = factory_->create_object();
      if (!obj) {
	return boost::shared_ptr<ObjType>();
      }

      ++active_obj_num_;
      return boost::shared_ptr<ObjType>(obj, Deleter(self));
    }
    
    size_t active_object() const {
      return active_obj_num_;
    }
  private:
    void return_object(ObjType* obj) {
      if (!obj) return;

      boost::lock_guard<boost::mutex> lock(mutex_);
      if (!obj->reusable() || objects_.size() >= max_idle_) {
	factory_->destroy_object(obj);
	--active_obj_num_;
	return;
      }
      objects_.push_back(obj);
    }
  private:
    const size_t max_active_;
    const size_t max_idle_;
    size_t active_obj_num_;
    boost::shared_ptr<FactoryType> factory_;
    ObjContainer objects_;
    boost::mutex mutex_;
  };

  }
#endif
