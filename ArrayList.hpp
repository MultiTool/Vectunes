#ifndef ArrayList_hpp

#define ArrayList_hpp

#include <vector>

template <class Type>
class ArrayList: public std::vector<Type> {
public:
  void remove(Type item){// remove by content
    typename ArrayList<Type>::iterator it;
    it = std::find(this->begin(), this->end(), item);
    if (it != this->end()){
      this->erase(it);
    }
  }
  void add(Type item){
    this->push_back(item);
  }
  void Insert(int Dex, Type item){// insert by index
    this->insert(this->begin(), Dex, item);
  }
  Type get(int Dex){
    return this->at(Dex);
  }
};

#endif // ArrayList_hpp


// from Voice.hpp:
//    std::vector<VoicePoint*>::iterator it;
//    it = std::find(this->CPoints.begin(), this->CPoints.end(), &pnt);
//    if (it != this->CPoints.end()){
//      //std::cout << "Element found in CPoints: " << *it << '\n';
//      this->CPoints.erase(it);
//    } else {
//      //std::cout << "Element not found in CPoints\n";
//    }

#if false
from GroupBox.hpp:
{// to do: create an ArrayList with remove()
  std::vector<SingerBase*>::iterator it;
  it = std::find(this->NowPlaying.begin(), this->NowPlaying.end(), player);
  if (it != this->NowPlaying.end()){
    this->NowPlaying.erase(it);
  }
}
#endif // false
