
template<typename T>
struct Array {
  T* mem;
  unsigned size;
};

template<typename T>
T* alloc(Array<T>* allocator){
  T* x = allocator->mem + allocator->size;
  allocator->size++;
  return x;
}

template<typename T>
struct List {
  T data;
  List<T>* next;
};

template<typename T>
List<T>* cons(Array<List<T>>* allocator, T data, List<T>* next){
  List<T>* node = alloc(allocator);
  node->data = data;
  node->next = next;
  return node;
}

template<typename T>
List<T>* copyList(Array<List<T>>* allocator, List<T>* list){
  if(list == nullptr){
    return nullptr;
  } else {
    return cons(allocator, list->data, copyList(allocator, list->next));
  }
}
 
template<typename T>
List<T>* findList(List<T>* list, bool (*f)(T)){
  for(auto p = list; p; p = p->next){
    if(f(list->data)) return list;
  }
  return nullptr;
}