#ifndef _CFOOD_POOLABLE_OBJECT_H_
#define _CFOOD_POOLABLE_OBJECT_H_

namespace cfood {
  class PoolableObject {
  public:
  PoolableObject() : reusable_(true), reuse_count_(0) {
    }
    virtual ~PoolableObject() {
    }
    void set_reusable(bool reusable) {
      reusable_ = reusable;
    }
    bool reusable() const {
      return reusable_;
    }
    uint32_t reuse_count() const {
      return reuse_count_;
    }
    void inc_reuse_count() {
      ++reuse_count_;
    }
  private:
    bool reusable_;
    uint32_t reuse_count_;
  };
}

#endif
