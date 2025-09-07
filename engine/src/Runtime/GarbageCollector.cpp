// #include "Runtime/GarbageCollector.h"

// #include "Object/Core/PyObject.h"
// namespace kaubo::Runtime {

// GarbageCollector& GarbageCollector::GetInstance() {
//   static GarbageCollector instance;
//   return instance;
// }

// void GarbageCollector::AddRoot(Object::PyObject* obj) {
//   roots.insert(obj);
// }

// void GarbageCollector::RemoveRoot(Object::PyObject* obj) {
//   roots.erase(obj);
// }

// void GarbageCollector::OnAllocation(Object::PyObject* obj) {
//   allocatedObjects.insert(obj);
// }

// void GarbageCollector::Collect() {
//   Mark();
//   Sweep();
// }

// void GarbageCollector::Mark() {
//   for (auto* root : roots) {
//     root->Mark();
//   }
// }

// bool GarbageCollector::IsMarked(Object::PyObject* obj) {
//   return obj != nullptr && obj->IsMarked();
// }

// void GarbageCollector::Sweep() {
//   auto allocIter = allocatedObjects.begin();
//   while (allocIter != allocatedObjects.end()) {
//     Object::PyObject* obj = *allocIter;
//     if (!IsMarked(obj)) {
//       //
//       delete obj;
//       allocIter = allocatedObjects.erase(allocIter);
//     } else {
//       ++allocIter;
//     }
//   }
// }

// }  // namespace kaubo::Runtime