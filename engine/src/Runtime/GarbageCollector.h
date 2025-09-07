// #pragma once

// #include "Object/Object.h"

// #include <unordered_set>

// namespace kaubo::Runtime {

// class GarbageCollector {
//  public:
//   static GarbageCollector& GetInstance();

//   // 注册根对象
//   void AddRoot(Object::PyObject* obj);
//   void RemoveRoot(Object::PyObject* obj);

//   // 执行垃圾回收
//   void Collect();

//   // 对象分配回调
//   void OnAllocation(Object::PyObject* obj);

//   // 标记管理
//   static bool IsMarked(Object::PyObject* obj);

//  private:
//   GarbageCollector() = default;

//   // 标记阶段
//   void Mark();

//   // 清除阶段
//   void Sweep();

//   std::unordered_set<Object::PyObject*> roots;
//   std::unordered_set<Object::PyObject*> allocatedObjects;
// };

// }  // namespace kaubo::Runtime
