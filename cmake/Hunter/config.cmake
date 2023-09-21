# Template for a custom hunter configuration Useful when there is a need for a
# non-default version or arguments of a dependency, or when a project not
# registered in soramitsu-hunter should be added.
#
# hunter_config(
#     package-name
#     VERSION 0.0.0-package-version
#     CMAKE_ARGS
#      CMAKE_VARIABLE=value
# )
#
# hunter_config(
#     package-name
#     URL https://repo/archive.zip
#     SHA1 1234567890abcdef1234567890abcdef12345678
#     CMAKE_ARGS
#       CMAKE_VARIABLE=value
# )

hunter_config(
    LLVM
    URL  https://github.com/qdrvm/kagome-llvm/archive/refs/tags/v13.0.1.tar.gz
    SHA1 a36890190449798e6bbec1e6061544d7016859d8
    CONFIGURATION_TYPES
      Release
    CMAKE_ARGS
      LLVM_ENABLE_PROJECTS=clang;clang-tools-extra;compiler-rt
      LLVM_ENABLE_ZLIB=OFF
      LLVM_INCLUDE_EXAMPLES=OFF
      LLVM_INCLUDE_TESTS=OFF
      LLVM_INCLUDE_DOCS=OFF
      LLVM_PARALLEL_LINK_JOBS=1
)
