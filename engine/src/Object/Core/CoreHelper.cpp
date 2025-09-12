#include "Object/Core/CoreHelper.h"
#include "Function/BuiltinFunction.h"
#include "Object/Container/PyDictionary.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyPromise.h"
#include "Object/Core/PyType.h"
#include "Object/Function/PyIife.h"
#include "Object/Iterator/Iterator.h"
#include "Object/Iterator/PyGenerator.h"
#include "Object/Matrix/PyMatrix.h"
#include "Object/Number/PyFloat.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyBytes.h"
#include "Runtime/VirtualMachine.h"
#include "gsl/pointers"

namespace kaubo::Object {
void LoadClass(const PyStrPtr& name, const KlassPtr& klass) {
  klass->SetName(name);
  klass->SetAttributes(PyDictionary::Create());
  klass->SetType(CreatePyType(klass)->as<PyType>());
  auto objectType = CreatePyType(ObjectKlass::Self());
  klass->SetSuper(PyList::Create<Object::PyObjPtr>({objectType}));
  klass->SetMro(
    PyList::Create<Object::PyObjPtr>({CreatePyType(klass), objectType})
  );
  klass->SetNative();
}

void ConfigureBasicAttributes(const KlassPtr& klass) {
  klass->AddAttribute(
    PyString::Create("__name__")->as<PyString>(), klass->Name()
  );
  klass->AddAttribute(
    PyString::Create("__class__")->as<PyString>(), klass->Type()
  );
  klass->AddAttribute(
    PyString::Create("__bases__")->as<PyString>(), PyIife::Create(GetBases)
  );
  klass->AddAttribute(
    PyString::Create("__mro__")->as<PyString>(), PyIife::Create(GetMro)
  );
  klass->AddAttribute(
    PyString::Create("__dict__")->as<PyString>(), PyIife::Create(GetDict)
  );
}

void InitKlass(const Object::PyStrPtr& name, const Object::KlassPtr& klass) {
  LoadClass(name, klass);
  ConfigureBasicAttributes(klass);
}

PyObjPtr
Invoke(const PyObjPtr& obj, const PyObjPtr& methodName, const PyListPtr& args) {
  auto func = obj->getattr(methodName);
  if (func == nullptr) {
    auto errorMessge = PyString::Create("AttributeError: ")
                         ->add(obj->str())
                         ->add(PyString::Create(" has no attribute "))
                         ->add(methodName->str())
                         ->as<PyString>();
    throw std::runtime_error(errorMessge->ToCppString());
  }
  return Runtime::Evaluator::InvokeCallable(func, args);
}

PyObjPtr GetAttr(const PyObjPtr& obj, const PyStrPtr& attrName) noexcept {
  // Check instance's class attributes
  auto* klass = obj->Klass();
  auto value = klass->Attributes()->TryGet(attrName);
  if (value != nullptr) {
    return value;
  }

  // Traverse the MRO list (starting from index 1 to skip the current class)
  auto mro = klass->Mro();
  auto mroLength = mro->Length();

  for (Index i = 1; i < mroLength; ++i) {
    auto* baseKlass = mro->GetItem(i)->as<PyType>()->Owner();
    value = baseKlass->Attributes()->TryGet(attrName);
    if (value != nullptr) {
      return value;
    }
  }

  return nullptr;
}

PyObjPtr BindSelf(const PyObjPtr& obj, const PyObjPtr& attr) {
  if (attr->is(NativeFunctionKlass::Self())) {
    return PyMethod::Create(obj, attr);
  }
  if (attr->is(FunctionKlass::Self())) {
    return PyMethod::Create(obj, attr);
  }
  if (attr->is(IifeKlass::Self())) {
    return attr->as<PyIife>()->Call(PyList::Create<Object::PyObjPtr>({obj}));
  }
  return attr;
}

void CacheAttr(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& key,
  const Object::PyObjPtr& attr
) {
  if (attr->is(FunctionKlass::Self())) {
    obj->Methods()->Put(key, attr);
    return;
  }
  if (attr->is(NativeFunctionKlass::Self())) {
    obj->Methods()->Put(key, attr);
    return;
  }
  if (attr->is(IifeKlass::Self())) {
    obj->Attributes()->Put(key, attr);
    return;
  }
  obj->Attributes()->Put(key, attr);
}

PyObjPtr GetBases(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return args->as<PyList>()->GetItem(0)->Klass()->Type()->Owner()->Super();
}
PyObjPtr GetMro(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return args->as<PyList>()->GetItem(0)->as<PyType>()->Owner()->Mro();
}
PyObjPtr GetDict(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto obj = args->as<PyList>()->GetItem(0);
  return obj->Attributes()->Add(obj->Methods())->as<PyDictionary>();
}
PyListPtr ComputeMro(const PyTypePtr& type) {
  auto oldMro = type->Owner()->Mro();
  if (oldMro) {
    //    Function::DebugPrint(StringConcat(PyList::Create(
    //      {PyString::Create("Mro for "), type->Owner()->Name(),
    //       PyString::Create(" already computed: "), oldMro->str()}
    //    )));
    return oldMro;
  }
  //  Function::DebugPrint(StringConcat(PyList::Create(
  //    {PyString::Create("Computing Mro for "), type->Owner()->Name(),
  //     PyString::Create(" with super: "), type->Owner()->Super()->str()}
  //  )));
  auto bases = type->Owner()->Super();
  PyListPtr mros = PyList::Create();
  for (Index i = 0; i < bases->Length(); i++) {
    auto base = bases->GetItem(i)->as<PyType>();
    auto mro = base->Owner()->Mro();
    mros->Append(mro->Copy());
  }
  //  Function::DebugPrint(StringConcat(
  //    PyList::Create({PyString::Create("Mros needed to merge: "),
  //    mros->str()})
  //  ));
  //  ForEach(mros, [&](const PyObjPtr& mro) {
  //    mro->str()->as<PyString>()->Print();
  //  });
  //  PyString::Create("")->as<PyString>()->PrintLine();
  auto mro = MergeMro(mros);
  //  Function::DebugPrint(
  //    StringConcat(PyList::Create({PyString::Create("Merged Mro: "),
  //    mro->str()}))
  //  );
  //  ForEach(mros, [&](const PyObjPtr& mro) {
  //    mro->str()->as<PyString>()->Print();
  //  });
  //  PyString::Create("")->as<PyString>()->PrintLine();
  mro = PyList::Create<Object::PyObjPtr>({type})
          ->as<PyList>()
          ->Add(mro)
          ->as<PyList>();
  //  Function::DebugPrint(StringConcat(PyList::Create(
  //    {PyString::Create("Mro for "), type->Owner()->Name(),
  //    PyString::Create(":"),
  //     mro->str()}
  //  )));
  return mro;
}

void CleanMros(const PyListPtr& mros) {
  for (Index i = 0; i < mros->Length();) {
    if (mros->GetItem(i)->as<PyList>()->Length() == 0) {
      mros->RemoveAt(i);
    } else {
      i++;
    }
  }
}

std::optional<std::tuple<Index, PyTypePtr>> FindCandidateBase(
  const PyListPtr& mros
) {
  for (Index i = 0; i < mros->Length(); i++) {
    auto head = mros->GetItem(i)->as<PyList>()->GetItem(0)->as<PyType>();
    if (CouldTypePlaceAhead(mros, head, i)) {
      return std::make_tuple(i, head);
    }
  }
  return std::nullopt;
}

void RemoveHeadFromMros(
  const PyListPtr& mros,
  Index excludeIndex,
  const PyTypePtr& head
) {
  for (Index j = 0; j < mros->Length(); j++) {
    if (j == excludeIndex) {
      continue;  // 改成使用传入的 excludeIndex
    }

    auto list = mros->GetItem(j)->as<PyList>();
    if (IsTrue(list->GetItem(0)->eq(head))) {
      list->RemoveAt(0);
      if (list->Length() == 0) {
        mros->RemoveAt(j);
      }
    }
  }
}

PyListPtr MergeMro(const PyListPtr& mros) {
  if (mros->Length() == 0) {
    return PyList::Create();
  }
  PyListPtr result = PyList::Create();
  while (mros->Length() > 1) {
    //    Function::DebugPrint(StringConcat(
    //      PyList::Create({PyString::Create("Mros to merge: "), mros->str()})
    //    ));
    CleanMros(mros);
    auto candidate = FindCandidateBase(mros);
    if (candidate == std::nullopt) {
      break;
    }
    Index candidateIndex = std::get<0>(candidate.value());
    auto head = std::get<1>(candidate.value());
    result->Append(head);
    RemoveHeadFromMros(mros, candidateIndex, head);
    mros->GetItem(candidateIndex)->as<PyList>()->RemoveAt(0);
    if (mros->GetItem(candidateIndex)->as<PyList>()->Length() == 0) {
      mros->RemoveAt(candidateIndex);
    }

    //    Function::DebugPrint(StringConcat(
    //      PyList::Create({PyString::Create("Mros after merge: "),
    //      mros->str()})
    //    ));
  }
  if (mros->Length() == 0) {
    //    Function::DebugPrint(StringConcat(
    //      PyList::Create({PyString::Create("the mro is: "), result->str()})
    //    ));
    return result;
  }
  auto last_mro = mros->GetItem(0)->as<PyList>();
  for (Index i = 0; i < last_mro->Length(); i++) {
    result->Append(last_mro->GetItem(i));
  }
  //  Function::DebugPrint(
  //    StringConcat(PyList::Create({PyString::Create("the mro is: "),
  //    result->str()}))
  //  );
  return result;
}

bool FirstOrNotInMro(const PyListPtr& mro, const PyTypePtr& type) {
  auto head = mro->GetItem(0)->as<PyType>();
  if (IsTrue(head->eq(type))) {
    return true;
  }
  for (Index i = 1; i < mro->Length(); i++) {
    if (IsTrue(mro->GetItem(i)->as<PyType>()->eq(type))) {
      return false;
    }
  }
  return true;
}

bool CouldTypePlaceAhead(
  const PyListPtr& mros,
  const PyTypePtr& type,
  Index ignore
) {
  for (Index i = 0; i < mros->Length(); i++) {
    if (i == ignore) {
      continue;
    }
    if (!FirstOrNotInMro(mros->GetItem(i)->as<PyList>(), type)) {
      return false;
    }
  }
  return true;
}

PyObjPtr KlassStr(const PyObjPtr& args) {
  return StringConcat(
    PyList::Create<PyObjPtr>(
      {PyString::Create("<"), args->as<PyList>()->GetItem(0)->Klass()->Name(),
       PyString::Create(" object at "), Function::Identity(args),
       PyString::Create(">")}
    )
  );
}

PyObjPtr Repr(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return args->as<PyList>()->GetItem(0)->repr();
}

PyObjPtr KlassRepr(const PyObjPtr& args) {
  return StringConcat(
    PyList::Create<PyObjPtr>(
      {PyString::Create("<"), args->as<PyList>()->GetItem(0)->Klass()->Name(),
       PyString::Create(" object at "), Function::Identity(args),
       PyString::Create(">")}
    )
  );
}

PyObjPtr Bool(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return args->as<PyList>()->GetItem(0)->boolean();
}

PyObjPtr KlassBool(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return PyBoolean::Create(true);
}

KlassPtr CreatePyKlass(
  const PyStrPtr& name,
  const PyDictPtr& attributes,
  const PyListPtr& super
) {
  auto* klass =
    gsl::owner<Klass*>{new Klass()};  // NOLINT(readability-redundant-casting)
  auto type = CreatePyType(klass)->as<PyType>();
  klass->SetName(name);
  klass->SetAttributes(attributes);
  klass->SetType(type);
  klass->SetSuper(super);
  klass->SetMro(ComputeMro(type));
  ConfigureBasicAttributes(klass);
  return klass;
}

PyObjPtr Str(const PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto argList = args->as<PyList>();
  if (argList->Length() == 0) {
    return PyString::Create("");
  }
  if (argList->Length() != 1) {
    throw std::runtime_error("str() takes at most 1 argument");
  }
  return args->as<PyList>()->GetItem(0)->str();
}

void LoadBootstrapClasses() {
  StringKlass::Self()->Initialize();
  IntegerKlass::Self()->Initialize();
  ListKlass::Self()->Initialize();
  DictionaryKlass::Self()->Initialize();
  TypeKlass::Self()->Initialize();
  BooleanKlass::Self()->Initialize();

  NativeFunctionKlass::Self()->Initialize();
  IifeKlass::Self()->Initialize();
  ListIteratorKlass::Self()->Initialize();
  IterDoneKlass::Self()->Initialize();
}

// 加载运行时支持类
void LoadRuntimeSupportClasses() {
  NoneKlass::Self()->Initialize();
  ObjectKlass::Self()->Initialize();
  FunctionKlass::Self()->Initialize();
  MethodKlass::Self()->Initialize();
  ListReverseIteratorKlass::Self()->Initialize();
  StringIteratorKlass::Self()->Initialize();
  DictItemIteratorKlass::Self()->Initialize();
  GeneratorKlass::Self()->Initialize();
  FloatKlass::Self()->Initialize();
  CodeKlass::Self()->Initialize();
  FrameKlass::Self()->Initialize();
  InstKlass::Self()->Initialize();
  BytesKlass::Self()->Initialize();
  MatrixKlass::Self()->Initialize();
  SliceKlass::Self()->Initialize();
  PromiseKlass::Self()->Initialize();
}

}  // namespace kaubo::Object