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
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <fstream>

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

  bool VisitFunctionDecl(FunctionDecl *FD) {
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
    if (!RD->isCompleteDefinition())
      return true;
    recordDecls.push_back(RD);
    return true;
  }

  bool VisitVarDecl(VarDecl *VD) {
    if (VD->isFileVarDecl())
      globalVars.push_back(VD);
    return true;
  }

  bool VisitEnumDecl(EnumDecl *ED) {
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
        Ctx->getPrintingPolicy());
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

  void SetASTContext(ASTContext *Context) { Ctx = Context; }

private:
  ASTContext *Ctx = nullptr;
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
      // Filter decls in system headers and compiler-generated decls
      auto &SM = Context->getSourceManager();
      auto DLoc = D->getLocation();
      if (DLoc.isInvalid() || SM.isInSystemHeader(DLoc))
        continue;

      // Traverse the declaration using our AST visitor.
      // D->dump();
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
  MyFrontendAction(ReflectionDatabase &RDB) : RDB_{RDB} {
    fmt::print("*** Creating frontend action ***\n");
  }

  void EndSourceFileAction() override {
    auto jsondb = Visitor.GenerateDatabase();
    //fmt::print("Database dump: {}\n", jsondb.dump(2));
    RDB_.push_back(TUReflectionData{getCurrentFile(), std::move(jsondb)});
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    return llvm::make_unique<MyASTConsumer>(Visitor);
  }

private:
  MyASTVisitor Visitor;
  ReflectionDatabase &RDB_;
};

using namespace llvm;

static cl::opt<std::string> OutputFile("output", llvm::cl::desc(R"(
Output file
)"),
                                       cl::init("meta.gen.cpp"),
                                       cl::cat(CxxReflectCategory));

static cl::opt<std::string> OutputJsonDBFile("output-jsondb", llvm::cl::desc(R"(
Output the reflection database as a JSON file 
)"), cl::Optional, cl::cat(CxxReflectCategory));

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
        //fmt::print("Merging {} (no ODR check)\n", name);
        // TODO do something here?
      } else {
        // name not found, copy decl data
        mergedDecls[name] = D;
      }
    }
  }

  fmt::print("*** Merged DB: {} decls\n", mergedDecls.size());

  if (!OutputJsonDBFile.empty()) {
	  fmt::print("*** Writing JSON database file to {}...\n", OutputJsonDBFile);
	  std::ofstream jsonFileOut{ OutputJsonDBFile, std::ios_base::out | std::ios_base::trunc };
	  json merged = json::array();
	  for (auto &&p : mergedDecls) 
		  merged.push_back(std::move(p.second));
	  jsonFileOut << merged.dump(2);
	  jsonFileOut.close();
  }
  
}
