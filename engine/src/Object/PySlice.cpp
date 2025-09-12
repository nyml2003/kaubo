#include "Object/PySlice.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyType.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
namespace kaubo::Object {

void PySlice::BindLength(Index length) {
  auto length64 = static_cast<int64_t>(length);  // 明确转换
  int64_t startValue = 0;
  int64_t stopValue = 0;
  int64_t stepValue = 0;

  if (start->is(NoneKlass::Self())) {
    startValue =
      step->is(NoneKlass::Self()) || step->as<PyInteger>()->ToI64() > 0
        ? 0
        : length64 - 1;
  } else {
    startValue = start->as<PyInteger>()->ToI64();
    startValue = startValue >= 0 ? startValue : length64 + startValue;
    startValue =
      std::max<int64_t>(0, std::min<int64_t>(startValue, length64 - 1));
  }

  if (stop->is(NoneKlass::Self())) {
    stopValue =
      step->is(NoneKlass::Self()) || step->as<PyInteger>()->ToI64() > 0
        ? length64
        : -1;
  } else {
    stopValue = stop->as<PyInteger>()->ToI64();
    stopValue = stopValue >= 0 ? stopValue : length64 + stopValue;
    stopValue = std::max<int64_t>(0, std::min<int64_t>(stopValue, length64));
  }

  if (step->is(NoneKlass::Self())) {
    stepValue = 1;
  } else {
    stepValue = step->as<PyInteger>()->ToI64();
  }

  if (stepValue == 0) {
    throw std::runtime_error("step 不能为 0。");
  }
  if (stepValue > 0) {
    if (startValue >= stopValue) {
      throw std::runtime_error("start 不能大于等于 stop。");
    }
  } else {
    if (startValue <= stopValue) {
      throw std::runtime_error("start 不能小于等于 stop。");
    }
  }
  start = CreatePyInteger(Collections::CreateIntegerWithI64(startValue));
  stop = CreatePyInteger(Collections::CreateIntegerWithI64(stopValue));
  step = CreatePyInteger(Collections::CreateIntegerWithI64(stepValue));
}

PyObjPtr SliceKlass::init(const PyObjPtr& type, const PyObjPtr& args) {
  if (type->as<PyType>()->Owner() != Self()) {
    throw std::runtime_error("PySlice::init(): klass is not a slice");
  }
  if (args->as<PyList>()->Length() == 3) {
    return CreatePySlice(
      args->as<PyList>()->GetItem(0), args->as<PyList>()->GetItem(1),
      args->as<PyList>()->GetItem(2)
    );
  }
  throw std::runtime_error("slice() 函数参数个数错误。");
}

PyObjPtr SliceKlass::str(const PyObjPtr& obj) {
  if (!obj->is(SliceKlass::Self())) {
    throw std::runtime_error("PySlice::str(): obj is not a slice");
  }
  auto slice = obj->as<PySlice>();
  return StringConcat(CreatePyList(
    {PyString::Create("slice("), slice->GetStart()->str(),
     PyString::Create(", "), slice->GetStop()->str(), PyString::Create(", "),
     slice->GetStep()->str(), PyString::Create(")")}
  ));
}

}  // namespace kaubo::Object