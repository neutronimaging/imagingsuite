//<LICENCE>
#include <map>
#include <utility>

std::map<float,float> diff(std::map<float,float> data)
{
  std::map<float,float> dmap;

  auto itA=data.begin();
  auto itB=itA;
  ++itA;

  for ( ; itA!=data.end(); ++itA, ++itB)
  {
      dmap.insert(std::make_pair((itA->first+itB->first)/2.0f,
                  (itA->second-itB->second)/(itA->first-itB->first)));
  }
  return dmap;
}
