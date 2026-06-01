#ifndef CIRCULAR_STORAGE_H
#define CIRCULAR_STORAGE_H

//using namespace std;


// class T must have a default constructor

/**
 * A storage that automatically wraps to the beginning
 */
template <class T, int capacity>
class CircularStorage {

public:

  typedef unsigned long length_t;

  // default constructor: initialize to empty
  CircularStorage() : 
    head_( store_ ),                 // head_ points to beginning of store_
    tail_( store_+(capacity-1) ), // tail_ points to last slot
    length_(0)                      // no elements
  {
  }

  ~CircularStorage(){}

  /**
   * Adds element at the tail. Automatically wraps around to
   * the beginning (of store_) when it hits the end (of the
   * store_). It also automatically discards (overwrites) 
   * element at the head if necessary.
   *
   * @param item The item to append at the tail
   * @returns none
   */
  void push( const T& item ) {

    if ( length_ >= capacity )  // its full, make space
      head_ = advance( head_ ); // by advancing the head

    // advance 'tail_' pointer
    tail_ = advance( tail_ );

    *tail_ = item;

    if ( length_ < capacity )
      ++length_;
  }


  /**
   * Remove an element at the head. Automatically wraps
   * around to the beginning (of the store_) when it hits the
   * end (of the store_).
   *
   * @returns Element that was removed from the head
   */
  T* pop() {

    if ( length_ < 1 )
      return 0;

    // save element to pop
    T* item = head_;

    // advance 'head_' pointer
    head_ = advance( head_ );

    --length_;

    return item;
  }


  /**
   * Get the element at index where the value of index is from
   * 0 up to (length-1) inclusive.
   *
   * @param index The cursor to the element to be retrieved
   * @returns The element retrieved
   */
  T* get( length_t index ) const {
  	T* ptr = head_ + index;
  	if ( ptr >= &store_[capacity] )
  		ptr -= capacity;
  	return ptr;
  }


  /**
   * Return pointer to the head element
   */
  T *getHead() const {
    if ( length_ < 1 )
      return 0;

    return head_;
  }


  /**
   * Return pointer to the tail element
   */
  T *getTail() const {
    if ( length_ < 1 )
      return 0;

    return tail_;
  }


  /**
   * Get the number of elements in storage
   *
   * @returns The number of elements in storage
   */
  length_t length() { return length_; }


private:
  // declared but not defined...
  CircularStorage(const CircularStorage&);
  CircularStorage& operator=(const CircularStorage&);

  T* advance( T* pointer ) {
    if ( pointer >= store_+capacity-1 )
      return store_;
    return ++pointer;
  }


  // the 'store' is where items are kept
  T store_[capacity];

  // points to the beginning of the storage
  T* head_;

  // points to the end of the storage
  T* tail_;

  // number of elements in storage
  length_t length_;
};


#endif // CIRCULAR_STORAGE_H
