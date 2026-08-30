#include "fortran_handler.h"
#include "../llvm_compiler.h"
#include <iostream>
namespace sdgs {
void FortranHandler::execute(const std::string& code, const nlohmann::json& context) {
    std::cout << "[FORTRAN] Executing (stub -> LLVM JIT)" << std::endl;
    LLVMCompiler compiler;
    auto func = compiler.compileExpression("x + y * 2");
    if (func) {
        double result = func(5.0, 3.0);
        std::cout << "[FORTRAN] JIT result: " << result << std::endl;
    }
}
}
