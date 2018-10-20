#ifndef ArrayList_hpp

#define ArrayList_hpp

#include <vector>

template <class Type>
class ArrayList: public std::vector<Type> {
public:
  void remove(const Type item){// remove by content
    typename ArrayList<Type>::iterator it;
    it = std::find(this->begin(), this->end(), item);
    if (it != this->end()){
      this->erase(it);
    }
  }
  void add(const Type item){
    this->push_back(item);
  }
  void Insert(int Dex, Type item){// insert by index
    this->insert(this->begin() + Dex, item);
  }
  Type get(int Dex){
    return this->at(Dex);
  }
};

#endif // ArrayList_hpp
