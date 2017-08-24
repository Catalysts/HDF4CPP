//
// Created by patrik on 23.08.17.
//

#ifndef GRASP_SEGMENTER_HDFDESTROYER_H
#define GRASP_SEGMENTER_HDFDESTROYER_H

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <hdf/hdf.h>
#include <hdf/mfhdf.h>
#include <vector>

namespace hdf4cpp {


class HdfDestroyerBase {
  public:
    virtual ~HdfDestroyerBase() {}
  protected:
    HdfDestroyerBase() {}
};

class HdfFileDestroyer : public HdfDestroyerBase {
  public:
    HdfFileDestroyer(int32 sId, int32 vId) : sId(sId), vId(vId) {}
    ~HdfFileDestroyer() {
        SDend(sId);
        Vend(vId);
        Hclose(vId);
    }

  private:
    int32 sId;
    int32 vId;
};

class HdfDatasetItemDestroyer : public HdfDestroyerBase {
  public:
    HdfDatasetItemDestroyer(int32 id) : id(id) {}
    ~HdfDatasetItemDestroyer() {
        SDendaccess(id);
    }

  private:
    int32 id;
};

class HdfGroupItemDestroyer : public HdfDestroyerBase {
  public:
    HdfGroupItemDestroyer(int32 id) : id(id) {}
    ~HdfGroupItemDestroyer() {
        Vdetach(id);
    }

  private:
    int32 id;
};

class HdfDataItemDestroyer : public HdfDestroyerBase {
  public:
    HdfDataItemDestroyer(int32 id) : id(id) {}
    ~HdfDataItemDestroyer() {
        VSdetach(id);
    }

  private:
    int32 id;
};

class HdfDestroyer {
  public:
    HdfDestroyer(HdfDestroyerBase *destroyerBase) : destroyer(destroyerBase) {}
    HdfDestroyer(const HdfDestroyer& destroyerObject) : destroyer(destroyerObject.destroyer) {}

  private:
    std::shared_ptr<HdfDestroyerBase> destroyer;
};

typedef std::vector<HdfDestroyer> HdfDestroyerChain;

}

#endif //GRASP_SEGMENTER_HDFDESTROYER_H
