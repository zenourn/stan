#ifndef __STAN__AGRAD__REV__OP__VVV_VARI_HPP__
#define __STAN__AGRAD__REV__OP__VVV_VARI_HPP__

#include <stan/agrad/rev/vari.hpp>

namespace stan {
  namespace agrad {
    
    class op_vvv_vari : public vari {
    protected:
      vari* avi_;
      vari* bvi_;
      vari* cvi_;
    public:
      op_vvv_vari(double f, vari* avi, vari* bvi, vari* cvi) :
        vari(f),
        avi_(avi),
        bvi_(bvi),
        cvi_(cvi) {
      }
    };

  }
}
#endif
