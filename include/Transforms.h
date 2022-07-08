#ifndef __TRANSFORMS_H__
#define __TRANSFORMS_H__

#include <glm/glm.hpp>


class Transforms
{
private:
public:
  static glm::dmat4 eastNorthUpToFixedFrame(glm::dvec3 origin/*, ellipsoid, result*/);
};


#endif // __TRANSFORMS_H__