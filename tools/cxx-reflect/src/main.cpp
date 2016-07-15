//------------------------------------------------------------------------------
// Tooling sample. Demonstrates:
//
// * How to write a simple source tool using libTooling.
// * How to use RecursiveASTVisitor to find interesting AST nodes.
// * How to use the Rewriter API to rewrite the source code.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include <cstdbool>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include <bustache/model.hpp>
#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
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

// Reflection DB: hierarchical (scope hierarchy)

static llvm::cl::OptionCategory CxxReflectCategory("cxx-reflect");

using namespace nlohmann;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor() {}

  ~MyASTVisitor() {}

  using base = RecursiveASTVisitor<MyASTVisitor>;

  bool reflectAllInCurrentScope() {
    if (reflectionScopes.empty())
      return false;
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
    } else if (D->getAttr<CxxrNoReflectAttr>()) {
      return false;
    }
    return reflectAllInCurrentScope();
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
    json methods, fields, bases;

    for (auto MD : RD->methods())
      methods.push_back(EmitCXXMethodDecl(MD));
    if (methods.size()) {
      methods.front()["isFirst"] = true;
      methods.back()["isLast"] = true;
    }
    obj["numMethods"] = methods.size();
    obj["methods"] = std::move(methods);

    for (auto FD : RD->fields())
      fields.push_back(EmitFieldDecl(FD));
    if (fields.size()) {
      fields.front()["isFirst"] = true;
      fields.back()["isLast"] = true;
    }
    obj["numFields"] = fields.size();
    obj["fields"] = std::move(fields);

    for (auto B : RD->bases())
      bases.push_back(json{{"className", B.getType().getAsString()}});
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
    obj["qualType"] = FD->getType().getAsString();
    return obj;
  }

  json EmitVarDecl(VarDecl *VD) {
    json obj = EmitNamedDecl(VD);
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
    obj["numEnumerators"] = enumerators.size();
    obj["enumerators"] = std::move(enumerators);
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

  json EmitNamedDecl(NamedDecl *ND) {
    json obj;
    obj["name"] = ND->getNameAsString();
    obj["qualName"] = ND->getQualifiedNameAsString();
    obj["declID"] = (unsigned long long)ND;
    return obj;
  }

  json GenerateDatabase() {
    fmt::print("Statistics: \n");
    fmt::print("- {} record types\n", recordDecls.size());
    fmt::print("- {} file-scoped variable definitions with external linkage\n",
               globalVars.size());
    fmt::print("- {} file-scoped function definitions with external linkage\n",
               globalFunctionDecls.size());
    json db = json::array();

    for (auto &&RD : recordDecls) {
      db.push_back(EmitCXXRecordDecl(RD));
    }
    for (auto &&VD : globalVars) {
      db.push_back(EmitVarDecl(VD));
    }
    for (auto &&FD : globalFunctionDecls) {
      db.push_back(EmitFunctionDecl(FD));
    }
    for (auto &&ED : enumDecls) {
      db.push_back(EmitEnumDecl(ED));
    }
    return db;
  }

  void SetASTContext(ASTContext *Context) { Context_ = Context; }

private:
  std::vector<bool> reflectionScopes;
  ASTContext *Context_ = nullptr;
  std::vector<CXXRecordDecl *> recordDecls;
  std::vector<EnumDecl *> enumDecls;
  std::vector<VarDecl *> globalVars;
  std::vector<FunctionDecl *> globalFunctionDecls;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(MyASTVisitor &Visitor) : Visitor_{Visitor} {}

  void Initialize(ASTContext &context) override {
    Context = &context;
    Visitor_.SetASTContext(&context);
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
  MyASTVisitor &Visitor_;
};

// Reflection data for one translation unit
struct TUReflectionData {
  std::string fileName;
  json db;
};
// Use global variables, since I don't know how to get back data from
// an ASTFrontendAction
using ReflectionDatabase = std::vector<TUReflectionData>;

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction(ReflectionDatabase &reflectionDB)
      : reflectionDB_{reflectionDB} {
    fmt::print("*** Creating frontend action ***\n");
  }

  void EndSourceFileAction() override {
    auto jsonDatabase = Visitor.GenerateDatabase();
    // fmt::print("Database dump: {}\n", jsondb.dump(2));
    reflectionDB_.push_back(
        TUReflectionData{getCurrentFile(), std::move(jsonDatabase)});
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    return llvm::make_unique<MyASTConsumer>(Visitor);
  }

private:
  MyASTVisitor Visitor;
  ReflectionDatabase &reflectionDB_;
};

using namespace llvm;

/*static cl::opt<std::string> OutputFile("output", llvm::cl::desc(R"(
Output file
)"),
                                       cl::init("meta.gen.cpp"),
                                       cl::cat(CxxReflectCategory));*/

static cl::opt<std::string> OutputJsonDBFile("output-jsondb",
                                             llvm::cl::desc(R"(
Output the reflection database as a JSON file.
)"),
                                             cl::Optional,
                                             cl::cat(CxxReflectCategory));

static cl::list<std::string> InputTemplates("i", llvm::cl::desc(R"(
Specify an input template file. Each template will generate a corresponding .cpp source file.
)"),
                                            cl::ZeroOrMore,
                                            cl::cat(CxxReflectCategory));

//
bustache::value ConvertJsonToBustacheValue(const json &jsonObj) {
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
}

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, CxxReflectCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  struct : public FrontendActionFactory {
    clang::FrontendAction *create() override {
      return new MyFrontendAction(RDB);
    }
    ReflectionDatabase RDB;
  } factory;

  auto result = Tool.run(&factory);
  fmt::print("*** {} translation units parsed. Merging...\n",
             factory.RDB.size());

  // now merge all the databases
  std::unordered_map<std::string, json> mergedDecls;
  for (auto &&TU : factory.RDB) {
    for (auto &&D : TU.db) {
      auto &name = D["qualName"].get<std::string>();
      if (mergedDecls.count(name)) {
        // Same decl name in two TUs
        // fmt::print("Merging {}\n", name);
        // TODO do something here?
      } else {
        // name not found, copy decl data
        mergedDecls[name] = D;
      }
    }
  }
  // generate final JSON
  json databaseJson;
  {
    json mergedJsonArray = json::array();
    for (auto &&p : mergedDecls)
      mergedJsonArray.push_back(std::move(p.second));
    fmt::print("*** Merged DB: {} decls\n", mergedDecls.size());
    databaseJson = json{{"decls", std::move(mergedJsonArray)}};
  }

  if (!OutputJsonDBFile.empty()) {
    fmt::print("*** Writing JSON database file to {}...\n", OutputJsonDBFile);
    std::ofstream jsonFileOut{OutputJsonDBFile,
                              std::ios_base::out | std::ios_base::trunc};
    jsonFileOut << databaseJson.dump(2);
    jsonFileOut.close();
  }

  fmt::print("*** Generating template model...\n");
  bustache::value templateModel = ConvertJsonToBustacheValue(databaseJson);

  using namespace llvm::sys;
  //
  for (auto &&inputTemplateFileName : InputTemplates) {
    std::string generatedSourceFileName =
        path::stem(inputTemplateFileName); // remove .in
    fmt::print("*** Generating {} from template file {}\n",
               generatedSourceFileName, inputTemplateFileName);
    std::ifstream templateFileIn{inputTemplateFileName};
    std::stringstream templateFileBuf;
    templateFileBuf << templateFileIn.rdbuf();
    bustache::format templateStr{templateFileBuf.str()};
    auto generatedSource = templateStr(templateModel);
    std::ofstream generatedSourceFileOut{generatedSourceFileName};
    generatedSourceFileOut << generatedSource;
  }
}
