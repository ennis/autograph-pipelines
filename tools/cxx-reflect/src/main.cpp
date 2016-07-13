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
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

// Reflection DB: hierarchical (scope hierarchy)

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

using namespace bustache;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(bustache::array &reflectionDB) : RDB(reflectionDB) {}

  bool VisitDecl(Decl *D) {
    array attribs;
    for (auto A : D->attrs()) {
      if (auto AA = dyn_cast<AnnotateAttr>(A)) {
        // handle attributes?
      }
    }
    curobj["attributes"] = std::move(attribs);
    return true;
  }

  bool VisitNamedDecl(NamedDecl *D) {
	  fmt::print("VisitNamedDecl\n");
    curobj["name"] = D->getNameAsString();
	curobj["qualName"] = D->getQualifiedNameAsString();
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *F) {
    // Only function definitions (with bodies), not declarations.
    if (!F->hasBody())
      return true;
    curobj["declKind"] = "function";
    curobj["isFunction"] = true;
    return true;
  }

  bool VisitCXXRecordDecl(CXXRecordDecl *RD) {
    if (!RD->hasDefinition())
      return true;
    curobj["declKind"] = "struct";
    curobj["isStruct"] = true;
    return true;
  }

private:
  object curobj;
  // array of top-level decls
  array &RDB;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer() : Visitor(ReflectionDB) {}

  void Initialize(ASTContext &context) override { Context = &context; }

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
      Visitor.TraverseDecl(D);
    }
    return true;
  }

private:
  bustache::array ReflectionDB;
  ASTContext *Context = nullptr;
  MyASTVisitor Visitor;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";

    // Now emit the rewritten buffer.
    TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>();
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  // ClangTool::run accepts a FrontendActionFactory, which is then used to
  // create new objects implementing the FrontendAction interface. Here we use
  // the helper newFrontendActionFactory to create a default factory that will
  // return a new MyFrontendAction object every time.
  // To further customize this, we could create our own factory class.
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
