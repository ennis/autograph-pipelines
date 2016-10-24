#include <cstdbool>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclVisitor.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Sema/Sema.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <cppformat/format.h>
#include <json.hpp>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace nlohmann;
using namespace llvm;

static cl::OptionCategory CxxReflectCategory("cxx-reflect");
cl::opt<std::string> OutputJsonDBFile("o", cl::desc("Specify output JSON database file"), cl::value_desc("filename"), cl::Required);

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class ReflectionASTVisitor : public RecursiveASTVisitor<ReflectionASTVisitor> {
public:
  ReflectionASTVisitor() {}

  ~ReflectionASTVisitor() {}

  using base = RecursiveASTVisitor<ReflectionASTVisitor>;

  bool reflectAllInCurrentScope() {
    if (reflectionScopes.empty())
      return true;
    return reflectionScopes.back();
  }

  // decide if we should emit reflection data for this decl
  bool shouldEmitReflectionData(const Decl *D) {
    auto &SM = Context_->getSourceManager();
    auto DLoc = D->getLocation();
    // Filter decls in system headers and compiler-generated decls
    if (DLoc.isInvalid() || SM.isInSystemHeader(DLoc))
      return false;

    // A decl is included in the reflection database if:
    // 1. The decl has the cxxr::reflect attribute
    // 2. The innermost parent scope of the decl that has a 'reflect' or
    // 'noreflect' attribute has a 'reflect' attribute
	if (D->getAttr<CxxrReflectAttr>()) {
		return true;
	}
	else if (D->getAttr<CxxrNoReflectAttr>()) {
		return false;
	}
    return reflectAllInCurrentScope();
  }

  void includeDeclSourceFile(NamedDecl *D) {
    // Check if the decl is in an anonymous namespace: fail if this is the case
    auto declName = D->getNameAsString();
    if (D->isInAnonymousNamespace()) {
      fmt::print(std::cerr, "error: Generating reflection data would expose "
                            "'{}' declared in an anonymous namespace.\n",
                 declName);
      return;
    }
    // Check if the decl has internal linkage: fail if this is the case
    if (!D->hasExternalFormalLinkage()) {
      fmt::print(std::cerr, "error: Generating reflection data would expose "
                            "'{}' that has internal linkage.\n",
                 declName);
      return;
    }
    auto &SM = Context_->getSourceManager();
    auto DLoc = D->getLocation();
    // Include the file containing the declaration. It is the responsibility
    // of the user to ensure that including this file in the generated sources
    // will not generate ODR-violations.
    // Typically, the only declarations that should be reflected should be in
    // header files.
    requiredHeaders.insert(SM.getFilename(DLoc));
  }

  void enterScope(Decl *D) {
    reflectionScopes.push_back(shouldEmitReflectionData(D));
  }

  void exitScope() { reflectionScopes.pop_back(); }

  bool TraverseDecl(Decl *D) {
    if (!D)
      return true;
    enterScope(D);
    base::TraverseDecl(D);
    exitScope();
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *FD) {
    if (!reflectAllInCurrentScope())
      return true;
    // Only function definitions (with bodies), not declarations.
    if (!FD->hasBody())
      return true;
    // We do not generate reflection data for functions with internal linkage
    // ('static inline')
    if (FD->hasExternalFormalLinkage() && FD->isGlobal())
      globalFunctionDecls.push_back(FD);
    return true;
  }

  bool VisitCXXRecordDecl(CXXRecordDecl *RD) {
    if (!reflectAllInCurrentScope())
      return true;
    if (!RD->isCompleteDefinition())
      return true;
    recordDecls.push_back(RD);
    RD->print(llvm::errs(), 2, false);
    return true;
  }

  bool VisitVarDecl(VarDecl *VD) {
    if (reflectAllInCurrentScope() && VD->isFileVarDecl())
      globalVars.push_back(VD);
    return true;
  }

  bool VisitEnumDecl(EnumDecl *ED) {
    if (reflectAllInCurrentScope())
      enumDecls.push_back(ED);
    return true;
  }

  json EmitCXXRecordDecl(CXXRecordDecl *RD) {
    json obj = EmitNamedDecl(RD);
    // also write the name under className, so that methods can find the
    // enclosing class name
    obj["className"] = RD->getNameAsString();
    obj["classQualName"] = RD->getQualifiedNameAsString();
    obj["declKind"] = "struct";
    obj["isStruct"] = true;
    obj["isType"] = true;
    json methods, fields, bases;

    for (auto MD : RD->methods())
      methods.push_back(EmitCXXMethodDecl(MD));
    if (methods.size()) {
      methods.front()["isFirst"] = true;
      methods.back()["isLast"] = true;
    }
    obj["numMethods"] = methods.size();
    obj["methods"] = std::move(methods);

    int numPublicFields = 0;
    for (auto FD : RD->fields()) {
      fields.push_back(EmitFieldDecl(FD));
      if (FD->getAccess() == AS_public)
        numPublicFields++;
    }
    if (fields.size()) {
      fields.front()["isFirst"] = true;
      fields.back()["isLast"] = true;
    }
    obj["numFields"] = fields.size();
    obj["numPublicFields"] = numPublicFields;
    obj["fields"] = std::move(fields);

	for (auto B : RD->bases()) {
		auto base_tyname = B.getType().getCanonicalType().getAsString(Context_->getPrintingPolicy());
		bases.push_back(json{ {"baseClassName", base_tyname } });
		//obj["base:" + base_tyname] = 
	}
    if (bases.size()) {
      bases.front()["isFirst"] = true;
      bases.back()["isLast"] = true;
    }
    obj["bases"] = std::move(bases);
    obj["numBases"] = RD->getNumBases();

    return obj;
  }

  json EmitFieldDecl(FieldDecl *FD) {
    json obj = EmitNamedDecl(FD);
    obj["qualType"] = FD->getType().getCanonicalType().getAsString(Context_->getPrintingPolicy());
    obj["fieldIndex"] = FD->getFieldIndex();
    if (FD->getAccess() == AS_public)
      obj["isPublic"] = true;
    if (FD->getAccess() == AS_private)
      obj["isPrivate"] = true;
    if (FD->getAccess() == AS_protected)
      obj["isProtected"] = true;
    return obj;
  }

  json EmitVarDecl(VarDecl *VD) {
    json obj = EmitNamedDecl(VD);
	obj["qualType"] = VD->getType().getCanonicalType().getAsString(Context_->getPrintingPolicy());
    return obj;
  }

  json EmitCXXMethodDecl(CXXMethodDecl *MD) {
    json obj = EmitFunctionDecl(MD);
    return obj;
  }

  json EmitParmVarDecl(ParmVarDecl *PVD) {
    json obj = EmitVarDecl(PVD);
    return obj;
  }

  json EmitEnumConstantDecl(EnumConstantDecl *ECD) {
    json obj = EmitNamedDecl(ECD);
    return obj;
  }

  json EmitEnumDecl(EnumDecl *ED) {
    json obj = EmitNamedDecl(ED);
    json enumerators = json::array();
    for (auto ECD : ED->enumerators()) {
      enumerators.push_back(EmitEnumConstantDecl(ECD));
    }
    if (enumerators.size()) {
      enumerators.front()["isFirst"] = true;
      enumerators.back()["isLast"] = true;
    }
	obj["enumName"] = ED->getNameAsString();
	obj["enumQualName"] = ED->getQualifiedNameAsString();
    obj["numEnumerators"] = enumerators.size();
    obj["enumerators"] = std::move(enumerators);
    obj["isType"] = true;
    obj["isEnum"] = true;
    obj["isScoped"] = ED->isScoped();
    obj["declKind"] = "enum";
    return obj;
  }

  json EmitFunctionDecl(FunctionDecl *FD) {
    json obj = EmitNamedDecl(FD);
    json args = json::array();
    int position = 0;
    for (auto P : FD->parameters()) {
      auto obj_p = EmitParmVarDecl(P);
      obj_p["index"] = position++;
      args.push_back(std::move(obj_p));
    }
    if (args.size()) {
      args.front()["isFirst"] = true;
      args.back()["isLast"] = true;
    }
    obj["args"] = std::move(args);
    obj["numParams"] = FD->getNumParams();
    obj["returnType"] = FD->getReturnType().getCanonicalType().getAsString(
        Context_->getPrintingPolicy());
    return obj;
  }

  json EmitMetaAttributes(NamedDecl *ND, json &base_decl) {
    auto &SM = Context_->getSourceManager();
    json obj = json::array();
	int i = 0;
    for (auto A : ND->attrs()) {
      if (auto AA = dyn_cast<CxxrMetaAttr>(A)) {
        for (auto &&metaObjectExpr : AA->additionalMetaObjects()) {
          std::string attr_init;
          llvm::raw_string_ostream ss{attr_init};
		  //metaObjectExpr->dumpPretty(*Context_);
          metaObjectExpr->printPretty(ss, nullptr,
                                      Context_->getPrintingPolicy());
		  ss.str();
          auto attr_type = metaObjectExpr->getType().getCanonicalType().getAsString(
              Context_->getPrintingPolicy());
          json attrJson;
          attrJson["attrID"] = i++;
          attrJson["qualType"] = attr_type;
          attrJson["initializer"] = attr_init;
          obj.push_back(std::move(attrJson));
          base_decl["attr:" + attr_type] = attr_init;
        }
      }
    }
    return obj;
  }

  json EmitNamedDecl(NamedDecl *ND) {
    json obj;
    obj["name"] = ND->getNameAsString();
    obj["qualName"] = ND->getQualifiedNameAsString();
    auto attrJson = EmitMetaAttributes(ND, obj);
    obj["numAttrs"] = attrJson.size();
    obj["attrs"] = std::move(attrJson);
    return obj;
  }

  json GenerateDatabase() {
    /*fmt::print("Statistics: \n");
    fmt::print("- {} record types\n", recordDecls.size());
    fmt::print("- {} file-scoped variable definitions with external linkage\n",
               globalVars.size());
    fmt::print("- {} file-scoped function definitions with external linkage\n",
               globalFunctionDecls.size());*/
    json db = json::array();
    for (auto &&RD : recordDecls)
      db.push_back(EmitCXXRecordDecl(RD));
    for (auto &&VD : globalVars)
      db.push_back(EmitVarDecl(VD));
    for (auto &&FD : globalFunctionDecls)
      db.push_back(EmitFunctionDecl(FD));
    for (auto &&ED : enumDecls)
      db.push_back(EmitEnumDecl(ED));
    json hdrs = json::array();
    for (auto &&reqHdr : requiredHeaders)
      hdrs.push_back(reqHdr);

    return json{{"decls", std::move(db)}};
  }

  void Initialize(CompilerInstance *Instance, ASTContext *Context) {
    CI_ = Instance;
    Context_ = Context;
  }

private:
  std::unordered_set<std::string> requiredHeaders;
  std::vector<bool> reflectionScopes;
  ASTContext *Context_ = nullptr;
  CompilerInstance *CI_ = nullptr;
  std::vector<CXXRecordDecl *> recordDecls;
  std::vector<EnumDecl *> enumDecls;
  std::vector<VarDecl *> globalVars;
  std::vector<FunctionDecl *> globalFunctionDecls;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class ReflectionASTConsumer : public ASTConsumer {
public:
  ReflectionASTConsumer(CompilerInstance &CI, ReflectionASTVisitor &Visitor)
      : CI_{CI}, Visitor_{Visitor} {}

  void Initialize(ASTContext &context) override {
    Context = &context;
    Visitor_.Initialize(&CI_, &context);
  }

  // Override the method that gets called for each parsed top-level
  // declaration.
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (auto D : DR) {
      // Traverse the declaration using our AST visitor.
      Visitor_.TraverseDecl(D);
    }

    return true;
  }

private:
  json outDB_;
  ASTContext *Context = nullptr;
  CompilerInstance &CI_;
  ReflectionASTVisitor &Visitor_;
};

void WriteJsonDatabase(json db, std::string fileName) {
  // fmt::print("*** Writing JSON database file to {}...\n",
  // outputJsonDBFileName);
  std::ofstream jsonFileOut{fileName,
                            std::ios_base::out | std::ios_base::trunc};
  jsonFileOut << db.dump(2);
  jsonFileOut.close();
}

json LoadJsonDatabase(std::string fileName) {
  json db;
  std::ifstream jsonFileIn{fileName};
  try {
    jsonFileIn >> db;
  } catch (std::invalid_argument ex) {
    fmt::format("Malformed JSON: {}.\n", fileName);
    throw;
  }
  return db;
}

// For each source file provided to the tool, a new FrontendAction is created.
class ReflectionFrontendAction : public ASTFrontendAction {
public:
  ReflectionFrontendAction() {}

  void EndSourceFileAction() override {
    auto outputJsonDBFileName =
        llvm::sys::path::filename(getCurrentFile()).str().append(".json");
    fmt::print("*** Writing JSON database file to {}...\n",
               outputJsonDBFileName);
    auto db = Visitor.GenerateDatabase();
    db["metaHeaders"] = json::array({getCurrentFile()});
    WriteJsonDatabase(std::move(db), outputJsonDBFileName);
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    return llvm::make_unique<ReflectionASTConsumer>(CI, Visitor);
  }

private:
  ReflectionASTVisitor Visitor;
};

//
/*bustache::value ConvertJsonToBustacheValue(const json &jsonObj) {
  bustache::value tmpValue;
  if (jsonObj.is_array()) {
    bustache::array tmpArray;
    for (auto &&v : jsonObj)
      tmpArray.push_back(ConvertJsonToBustacheValue(v));
    tmpValue = std::move(tmpArray);
  } else if (jsonObj.is_number_float())
    tmpValue = jsonObj.get<float>();
  else if (jsonObj.is_number_integer())
    tmpValue = jsonObj.get<int>();
  else if (jsonObj.is_number_unsigned())
    tmpValue = static_cast<int>(jsonObj.get<unsigned>());
  else if (jsonObj.is_object()) {
    bustache::object tmpObject;
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it)
      tmpObject[it.key()] = ConvertJsonToBustacheValue(it.value());
    tmpValue = std::move(tmpObject);
  } else if (jsonObj.is_null())
    tmpValue = nullptr;
  else if (jsonObj.is_boolean())
    tmpValue = jsonObj.get<bool>();
  else if (jsonObj.is_string())
    tmpValue = jsonObj.get<std::string>();

  return tmpValue;
}*/

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, CxxReflectCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  return Tool.run(newFrontendActionFactory<ReflectionFrontendAction>().get());
}
