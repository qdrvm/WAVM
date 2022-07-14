

hunter_config(
    LLVM
    VERSION 12.0.1-p1
    CMAKE_ARGS
      LLVM_ENABLE_PROJECTS="compiler-rt"
    KEEP_PACKAGE_SOURCES
)

