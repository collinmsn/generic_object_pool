#ifndef _POOL_OBJECT_H_
#define _POOL_OBJECT_H_

class PoolObject {
public:
PoolObject() : reusable_(true) {
  }
  virtual ~PoolObject() {
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

#endif
