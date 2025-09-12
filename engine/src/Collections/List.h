#pragma once
#include "Common.h"

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <random>
#include <stdexcept>
namespace kaubo::Collections {
const Index INIT_CAPACITY = 10;
template <typename T>
class List {
 private:
  /**
   * 当前列表中元素的个数
   */
  Index size;
  /**
   * 列表的容量，即列表中最多可以存放的元素个数
   * 内存分配时，会分配capacity个元素的空间
   */
  Index capacity;
  /**
   * 列表中的元素
   */
  std::unique_ptr<T[]> elements;
  /**
   * @brief 扩容
   * @details 默认扩容为原来的两倍
   */
  void Expand();

 public:
  void Expand(Index newCapacity);
  void ExpandWithElement(Index newCapacity, T element);
  explicit List(Index capacity);
  explicit List();
  explicit List(Index count, T element);
  List(std::initializer_list<T> list);
  explicit List(Index count, std::unique_ptr<T[]> data)
    : size(count), capacity(count), elements(std::move(data)) {}
  explicit List(Index count, T* stream)
    : size(count), capacity(count), elements(std::make_unique<T[]>(count)) {
    std::copy(stream, stream + count, elements.get());
  }
  explicit List(Index count, const T* stream)
    : size(count), capacity(count), elements(std::make_unique<T[]>(count)) {
    std::copy(stream, stream + count, elements.get());
  }
  List(const List<T>& other);
  List(List<T>&& other) noexcept;
  List<T>& operator=(const List<T>& other);
  List<T>& operator=(List<T>&& other) noexcept;
  /**
   * 析构函数
   */
  ~List();
  /**
   * 获取列表中元素的个数
   * @return 元素个数
   */
  [[nodiscard]] Index Size() const;
  /**
   * 获取列表的容量
   * @return 容量
   */
  [[nodiscard]] Index Capacity() const;
  T* Data();
  const T* Data() const { return elements.get(); }
  /**
   * @brief 返回列表末尾的元素
   * @return 第一个元素
   */
  T First() const;
  /**
   * @brief 移除列表首元素
   * @return 被移除的元素
   */
  T Shift();
  /**
   * @brief 在列表前端添加元素
   * @param element 要添加的元素
   */
  void Unshift(T element);
  /**
   * @brief 返回列表末尾的元素
   * @return 最后一个元素
   */
  T Last() const;
  /**
   * @brief 在列表末端添加元素
   * @param element 要添加的元素
   */
  void Push(T element);
  /**
   * @brief 移除列表末尾的元素
   * @return 被移除的元素
   */
  T Pop();
  /**
   * @brief 移除列表末尾 k 个元素
   * @param k 要移除的元素个数
   * @return 被移除的元素组成的列表
   */
  List<T> Pop(Index k);
  /**
   * @brief 合并列表
   * @details 本身列表不会改变，返回一个新的列表
   * @param list 要合并的列表
   * @return 合并后的新列表
   */
  List<T> Add(const List<T>& list);
  /**
   * @brief 将新的列表拷贝添加到列表末尾
   * @param list 要添加的列表
   */
  void Concat(const List<T>& list);
  /**
   * @brief 清空列表
   */
  void Clear();
  /**
   * @brief 返回列表的切片, 是浅拷贝
   * @param start 切片的起始索引（包含）
   * @param end 切片的结束索引（不包含）
   * @return 列表在 [start, end) 之间的元素组成的列表
   */
  [[nodiscard]] List<T> Slice(Index start, Index end) const;
  /**
   * @brief 移除指定位置的元素
   * @param index 要移除的元素的索引
   */
  void RemoveAt(Index index);
  /**
   * @brief 移除指定区间[start, end)的元素
   * @param start 移除区间的起始索引（包含）
   * @param length 移除区间的长度
   */
  void RemoveRange(Index start, Index length);
  /**
   * @brief 移去[start, end)区间的元素，并用list替换
   * @param start 要插入的位置
   * @param end 要插入的元素
   * @param list 要插入的元素列表
   */
  void InsertAndReplace(Index start, Index end, const List<T>& list);
  /**
   * @brief 在指定位置插入元素
   * @param index 要插入的位置
   * @param element 要插入的元素
   */
  void Insert(Index index, T element);
  /**
   * @brief 反转列表
   */
  void Reverse();
  /**
   * @brief 减小列表的容量以匹配其元素数量
   */
  void TrimExcess();
  /**
   * @brief 列表是否为空
   * @return 如果列表为空，则为 true; 否则为 false
   */
  [[nodiscard]] bool Empty() const noexcept;
  /**
   * @brief 列表是否已满
   * @return 如果列表已满，则为 true; 否则为 false
   */
  [[nodiscard]] bool Full() const noexcept;
  /**
   * @brief 判断索引是否有效
   * @param index 要检查的索引
   * @return 如果索引有效，则为 true; 否则为 false
   */
  [[nodiscard]] bool ValidIndex(Index index) const noexcept;
  /**
   * @brief 获取指定位置的元素
   * @param index 要获取的元素的索引
   * @return 指定位置的元素
   */
  [[nodiscard]] T Get(Index index) const;
  [[nodiscard]] bool Contains(T element) const;
  [[nodiscard]] Index IndexOf(T element) const;
  T& operator[](Index index);
  const T& operator[](Index index) const;
  /**
   * @brief 设置指定位置的元素
   * @param index 要设置的元素的索引
   * @param element 要设置的新元素
   */
  void Set(Index index, T element);
  /**
   * @brief 填充列表为指定元素，
   * @details 保证执行完后列表是一个size大小的元素全为element的列表
   * @param element 要填充的元素
   */
  void Fill(T element);
  /**
   * @brief 浅拷贝当前列表
   * @return 浅拷贝后的列表
   */
  [[nodiscard]] List<T> Copy() const;

  void Shuffle();
};
template <typename T>
List<T>::~List() = default;
template <typename T>
List<T>::List(Index _capacity) : size(0), capacity(_capacity) {
  if (_capacity > 0) {
    elements = std::make_unique<T[]>(_capacity);
  }
}
template <typename T>
List<T>::List() : size(0), capacity(0), elements(nullptr) {}
template <typename T>
List<T>::List(Index count, T element) : size(count) {
  if (count == 0) {
    throw std::runtime_error("List::List: count is 0");
  }
  capacity = count;
  elements = std::make_unique<T[]>(count);
  std::fill(elements.get(), elements.get() + count, element);
}
template <typename T>
List<T>::List(std::initializer_list<T> list)
  : size(list.size()),
    capacity(list.size()),
    elements(std::make_unique<T[]>(list.size())) {
  std::copy(list.begin(), list.end(), elements.get());
}
template <typename T>
List<T>::List(const List<T>& other) : size(other.size), capacity(other.size) {
  if (size == 0) {
    return;
  }
  elements = std::make_unique<T[]>(size);
  std::copy(other.elements.get(), other.elements.get() + size, elements.get());
}
template <typename T>
List<T>::List(List<T>&& other) noexcept
  : size(other.size),
    capacity(other.capacity),
    elements(std::move(other.elements)) {
  other.size = 0;
  other.capacity = 0;
}
template <typename T>
List<T>& List<T>::operator=(List<T>&& other) noexcept {
  if (this != &other) {
    size = other.size;
    capacity = other.capacity;
    elements = std::move(other.elements);
    other.size = 0;
    other.capacity = 0;
  }
  return *this;
}
template <typename T>
List<T>& List<T>::operator=(const List<T>& other) {
  if (this != &other) {
    size = other.size;
    capacity = other.capacity;
    elements = std::make_unique<T[]>(other.capacity);
    std::copy(
      other.elements.get(), other.elements.get() + size, elements.get()
    );
  }
  return *this;
}
template <typename T>
Index List<T>::Size() const {
  return size;
}
template <typename T>
Index List<T>::Capacity() const {
  return capacity;
}
template <typename T>
T List<T>::First() const {
  if (Empty()) {
    throw std::runtime_error("List::First: List is empty");
  }
  return elements[0];
}
template <typename T>
T List<T>::Shift() {
  if (Empty()) {
    throw std::runtime_error("List::Shift: List is empty");
  }
  T element = std::move(elements[0]);
  std::move(elements.get() + 1, elements.get() + size, elements.get());
  size--;
  return element;
}
template <typename T>
void List<T>::Unshift(T element) {
  if (Full()) {
    Expand();
  }
  std::move(elements.get(), elements.get() + size, elements.get() + 1);
  elements[0] = element;
  size++;
}
template <typename T>
T List<T>::Last() const {
  if (Empty()) {
    throw std::runtime_error("List::Last: List is empty");
  }
  return elements[size - 1];
}
template <typename T>
void List<T>::Push(T element) {
  if (Full()) {
    Expand();
  }
  elements[size] = element;
  size++;
}
template <typename T>
T List<T>::Pop() {
  if (Empty()) {
    throw std::runtime_error("List::Pop: List is empty");
  }
  size--;
  return elements[size];
}
template <typename T>
List<T> List<T>::Pop(Index k) {
  if (k > size) {
    throw std::runtime_error("List::Pop: k is greater than size");
  }
  if (k == 0) {
    return List<T>();
  }
  List<T> list(k);
  std::copy(
    elements.get() + size - k, elements.get() + size, list.elements.get()
  );
  list.size = k;
  size -= k;
  return list;
}
template <typename T>
List<T> List<T>::Add(const List<T>& list) {
  List<T> newList(size + list.size);
  std::copy(elements.get(), elements.get() + size, newList.elements.get());
  std::copy(
    list.elements.get(), list.elements.get() + list.size,
    newList.elements.get() + size
  );
  newList.size = size + list.size;
  return newList;
}
template <typename T>
void List<T>::Concat(const List<T>& list) {
  if (size + list.size > capacity) {
    Expand(size + list.size);
  }
  std::copy(
    list.elements.get(), list.elements.get() + list.size, elements.get() + size
  );
  size += list.size;
}
template <typename T>
void List<T>::Clear() {
  size = 0;
}
template <typename T>
List<T> List<T>::Slice(Index start, Index end) const {
  if (start >= size || end > size || start >= end) {
    throw std::runtime_error("List::Slice::Index out of range");
  }
  List<T> list(end - start);
  std::copy(elements.get() + start, elements.get() + end, list.elements.get());
  list.size = end - start;
  return list;
}
template <typename T>
void List<T>::Insert(Index index, T element) {
  if (index > size) {
    throw std::runtime_error("List::Insert::Index out of range");
  }
  if (Full()) {
    Expand();
  }
  auto rawElements = elements.get();
  std::copy_backward(
    rawElements + index, rawElements + size, rawElements + size + 1
  );
  elements[index] = element;
  size++;
}
template <typename T>
void List<T>::RemoveAt(Index index) {
  if (!ValidIndex(index)) {
    throw std::runtime_error("List::RemoveAt::Index out of range");
  }
  std::move(
    elements.get() + index + 1, elements.get() + size, elements.get() + index
  );
  size--;
}
template <typename T>
void List<T>::RemoveRange(Index start, Index length) {
  if (start >= size || start + length > size) {
    throw std::runtime_error("List::RemoveRange::Index out of range");
  }
  if (length == 0) {
    return;
  }
  std::move(
    elements.get() + start + length, elements.get() + size,
    elements.get() + start
  );
  size -= length;
}
template <typename T>
void List<T>::InsertAndReplace(Index start, Index end, const List<T>& list) {
  // 检查索引是否越界
  if (start > size || end > size || start > end) {
    throw std::runtime_error("List::InsertAndReplace::Index out of range");
  }

  auto removeLength = end - start;  // 要移除的元素个数
  auto insertLength = list.size;    // 要插入的元素个数

  // 如果插入的元素个数小于移除的元素个数
  if (insertLength < removeLength) {
    // 将 [end, size-1] 的元素移动到 [start + insertLength, size - removeLength
    // + insertLength - 1]
    std::move(
      elements.get() + end, elements.get() + size,
      elements.get() + start + insertLength
    );
    // 将 list 的元素复制到 [start, start + insertLength - 1]
    std::copy(
      list.elements.get(), list.elements.get() + insertLength,
      elements.get() + start
    );
    // 更新列表大小
    size = size - removeLength + insertLength;
  }
  // 如果插入的元素个数大于移除的元素个数
  else if (insertLength > removeLength) {
    // 计算需要的新容量
    auto newCapacity = size + insertLength - removeLength;
    // 如果当前容量不足以容纳新元素，则扩展容量
    if (newCapacity > capacity) {
      Expand(newCapacity);
    }
    // 将 [end, size-1] 的元素移动到 [start + insertLength, size + insertLength
    // - removeLength - 1]
    std::move(
      elements.get() + end, elements.get() + size,
      elements.get() + start + insertLength
    );
    // 将 list 的元素复制到 [start, start + insertLength - 1]
    std::copy(
      list.elements.get(), list.elements.get() + insertLength,
      elements.get() + start
    );
    // 更新列表大小
    size += insertLength - removeLength;
  }
  // 如果插入的元素个数等于移除的元素个数
  else {
    // 直接将 list 的元素复制到 [start, start + insertLength - 1]
    std::copy(
      list.elements.get(), list.elements.get() + insertLength,
      elements.get() + start
    );
  }
}
template <typename T>
void List<T>::Reverse() {
  if (size <= 1) {
    return;
  }
  std::reverse(elements.get(), elements.get() + size);
}
template <typename T>
void List<T>::TrimExcess() {
  if (size == 0) {
    capacity = 0;
    elements.reset();
    return;
  }
  if (size == capacity) {
    return;
  }
  std::unique_ptr<T[]> newElements = std::make_unique<T[]>(size);
  std::copy(elements.get(), elements.get() + size, newElements.get());
  elements = std::move(newElements);
  capacity = size;
}
template <typename T>
bool List<T>::Empty() const noexcept {
  return size == 0;
}
template <typename T>
bool List<T>::ValidIndex(Index index) const noexcept {
  return size > 0 && index < size;
}
template <typename T>
bool List<T>::Full() const noexcept {
  return size >= capacity;
}
template <typename T>
T List<T>::Get(Index index) const {
  if (!ValidIndex(index)) {
    throw std::out_of_range("List::Get: Index out of range");
  }
  return elements[index];
}
template <typename T>
bool List<T>::Contains(T element) const {
  for (Index i = 0; i < size; i++) {
    if (elements[i] == element) {
      return true;
    }
  }
  return false;
}
template <typename T>
Index List<T>::IndexOf(T element) const {
  for (Index i = 0; i < size; i++) {
    if (elements[i] == element) {
      return i;
    }
  }
  throw std::runtime_error("List::IndexOf: Element not found");
}
template <typename T>
T& List<T>::operator[](Index index) {
  if (!ValidIndex(index)) {
    throw std::out_of_range("List::operator[]: Index out of range");
  }
  return elements[index];
}
template <typename T>
const T& List<T>::operator[](Index index) const {
  if (!ValidIndex(index)) {
    throw std::out_of_range("List::const operator[]: Index out of range");
  }
  return elements[index];
}
template <typename T>
void List<T>::Set(Index index, T element) {
  if (!ValidIndex(index)) {
    throw std::runtime_error("List::Set::Index out of range");
  }
  elements[index] = element;
}
template <typename T>
void List<T>::Expand() {
  Index newCapacity = std::max(capacity + (capacity >> 1), INIT_CAPACITY);
  std::unique_ptr<T[]> newElements = std::make_unique<T[]>(newCapacity);
  std::copy(elements.get(), elements.get() + size, newElements.get());
  elements = std::move(newElements);
  capacity = newCapacity;
}
template <typename T>
void List<T>::Expand(Index newCapacity) {
  std::unique_ptr<T[]> newElements = std::make_unique<T[]>(newCapacity);
  std::copy(elements.get(), elements.get() + size, newElements.get());
  elements = std::move(newElements);
  capacity = newCapacity;
}
template <typename T>
void List<T>::ExpandWithElement(Index newCapacity, T element) {
  std::unique_ptr<T[]> newElements = std::make_unique<T[]>(newCapacity);
  std::fill(newElements.get(), newElements.get() + newCapacity, element);
  std::copy(elements.get(), elements.get() + size, newElements.get());
  elements = std::move(newElements);
  capacity = newCapacity;
  size = newCapacity;
}
template <typename T>
void List<T>::Fill(T element) {
  if (capacity == 0) {
    throw std::runtime_error("List::Fill: List is empty");
  }
  std::fill(elements.get(), elements.get() + capacity, element);
  size = capacity;
}
template <typename T>
List<T> List<T>::Copy() const {
  List<T> newList(size);
  std::copy(elements.get(), elements.get() + size, newList.elements.get());
  newList.size = size;
  return newList;
}
template <typename T>
T* List<T>::Data() {
  return elements.get();
}

template <typename T>
void List<T>::Shuffle() {
  std::random_device randomDevice;
  std::mt19937 gen(randomDevice());
  std::shuffle(elements.get(), elements.get() + size, gen);
}

}  // namespace kaubo::Collections