//------------------------------------------------------------------------------------
// Two possible implementations for BoolArray depending on template parameter SIZE:
// For small size we use [] array
// For big size we use unordered_set
// (note that this is just an implementation example, there are other better strategies to manage array of bools)
// We derive from a different base class based on template parameter
// There is private inheritance in this example
// @author Amir Kirsh
//------------------------------------------------------------------------------------

#include <iostream>
#include <unordered_set>
#include <cassert>

enum class SIZE_RANGE {SMALL, BIG = 1000};
constexpr bool operator<(int size, SIZE_RANGE s) {
  return size < (int)s;
}

template<size_t SIZE, SIZE_RANGE SizeRange>
class BoolArrayBase;

template<size_t SIZE>
class BoolArrayBase<SIZE, SIZE_RANGE::SMALL> {
  bool arr[SIZE] = {};
public:
  bool& get(size_t index) {
      return arr[index];
  }
  auto begin() { return arr; }
  auto end() { return arr + SIZE; }
};

template<size_t SIZE>
class BoolArrayBase<SIZE, SIZE_RANGE::BIG> {
  std::unordered_set<size_t> s;
  class BoolProxy {
  protected:
    std::unordered_set<size_t>& s_ref;
    size_t index;
  public:
    BoolProxy(std::unordered_set<size_t>& s, size_t index): s_ref(s), index(index) {}
    operator bool() const { return s_ref.contains(index); }
    bool operator=(bool val) {
      bool cur_val = s_ref.contains(index);
      if(val != cur_val) {
        if(val) s_ref.insert(index);
        else s_ref.erase(index);
      }
      return val;
    }
  };
  class iterator: private BoolProxy {
  public:
    using BoolProxy::BoolProxy;
    // iterator traits
    using difference_type = long;
    using value_type = bool;
    using pointer = bool*;
    using reference = bool&;
    using iterator_category = std::forward_iterator_tag;
    auto operator++() {
        ++BoolProxy::index;
        return *this;
    }
    bool operator!=(iterator other) const {
        return &(BoolProxy::s_ref) != &(other.s_ref) || BoolProxy::index != other.index;
    }
    BoolProxy operator*() {
        return *this;
    }
  };
public:
  BoolProxy get(size_t index) {
      return {s, index};
  }
  iterator begin() { return {s, 0}; }
  iterator end() { return {s, SIZE}; }
};

template<size_t SIZE>
using BoolArrayBaseT = BoolArrayBase< SIZE, SIZE < SIZE_RANGE::BIG? SIZE_RANGE::SMALL  :
                                                  SIZE_RANGE::BIG>;

template<size_t SIZE>
class BoolArray: BoolArrayBaseT<SIZE> {
public:
    BoolArray() {}
    // below decltype(auto) allows to deduce the relevant type, rvalue or lvalue
    decltype(auto) operator[](size_t index) { return BoolArrayBaseT<SIZE>::get(index); }
    bool operator[](size_t index) const { return const_cast<BoolArray&>(*this).BoolArrayBaseT<SIZE>::get(index); }
    auto begin() { return BoolArrayBaseT<SIZE>::begin(); }
    auto end() { return BoolArrayBaseT<SIZE>::end(); }
    // TODO: need to implement the const version of begin and end
};

int main() {
	BoolArray<5> b1;
	BoolArray<1005> b2;
	std::cout << sizeof(b1) << std::endl;
	std::cout << sizeof(b2) << std::endl;
	assert(b1[0] == false);
	assert(b2[0] == false);
	b1[0] = true;
	b2[0] = true;
	assert(b1[0] == true);
	assert(b2[0] == true);
	assert(std::count_if(b1.begin(), b1.end(), [](bool val){return !val;}) == 4);	
	assert(std::count_if(b2.begin(), b2.end(), [](bool val){return !val;}) == 1004);	
	b1[0] = false;
	b2[0] = false;
	assert(b1[0] == false);
	assert(b2[0] == false);
	auto b3 = b1;
	auto b4 = b2;
	assert(b3[0] == false);
	assert(b4[0] == false);
  b3[0] = true;
	b4[0] = true;
	assert(std::count_if(b1.begin(), b1.end(), [](bool val){return !val;}) == 5);	
	assert(std::count_if(b2.begin(), b2.end(), [](bool val){return !val;}) == 1005);	
}