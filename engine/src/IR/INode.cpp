
#include "IR/INode.h"
#include "Function/BuiltinFunction.h"
#include "IR/ClassDef.h"
#include "IR/FuncDef.h"
#include "IR/Module.h"
#include "Tools/Terminal/IntermediateRepresentationTerminal.h"

namespace kaubo::IR {
Object::PyCodePtr
GetCodeFromList(const Object::PyObjPtr& codeList, const INodePtr& node) {
  if (node->is(ModuleKlass::Self())) {
    return codeList->as<Object::PyList>()
      ->GetItem(node->as<Module>()->CodeIndex())
      ->as<Object::PyCode>();
  }
  if (node->is(FuncDefKlass::Self())) {
    return codeList->as<Object::PyList>()
      ->GetItem(node->as<FuncDef>()->CodeIndex())
      ->as<Object::PyCode>();
  }
  if (node->is(ClassDefKlass::Self())) {
    return codeList->as<Object::PyList>()
      ->GetItem(node->as<ClassDef>()->CodeIndex())
      ->as<Object::PyCode>();
  }
  return GetCodeFromList(codeList, node->Parent());
}
void PrintNode(const Object::PyObjPtr& node, const Object::PyStrPtr& text) {
  IntermediateRepresentationTerminal::get_instance().info(
    Function::Identity(Object::CreatePyList({node}))
      ->str()
      ->as<Object::PyString>()
      ->ToCppString()
  );
  IntermediateRepresentationTerminal::get_instance().info("[");
  IntermediateRepresentationTerminal::get_instance().info(text->ToCppString());
  IntermediateRepresentationTerminal::get_instance().info("]");
}

void PrintEdge(
  const Object::PyObjPtr& parent,
  const Object::PyObjPtr& child,
  const Object::PyStrPtr& text
) {
  IntermediateRepresentationTerminal::get_instance().info(
    Function::Identity(Object::CreatePyList({parent}))
      ->str()
      ->as<Object::PyString>()
      ->ToCppString()
  );
  if (text == nullptr) {
    IntermediateRepresentationTerminal::get_instance().info("--->");
  } else {
    IntermediateRepresentationTerminal::get_instance().info("-- ");
    IntermediateRepresentationTerminal::get_instance().info(text->ToCppString()
    );
    IntermediateRepresentationTerminal::get_instance().info(" -->");
  }
  IntermediateRepresentationTerminal::get_instance().info(
    Function::Identity(Object::CreatePyList({child}))
      ->str()
      ->as<Object::PyString>()
      ->ToCppString()
  );
}

}  // namespace kaubo::IR