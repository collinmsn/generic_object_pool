#ifndef _CFOOD_POOLABLE_OBJECT_H_
#define _CFOOD_POOLABLE_OBJECT_H_

namespace cfood {
class PoolableObject {
public:
PoolableObject() : reusable_(true) {
  }
  virtual ~PoolableObject() {
  }
  void set_reusable(bool reusable) {
    reusable_ = reusable;
  }
  bool reusable() const {
    return reusable_;
  }
private:
  bool reusable_;
};
}

#endif
