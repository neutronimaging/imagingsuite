# CMake generated Testfile for 
# Source directory: /Users/chiaracarminati/Downloads/code-4.4.3/test
# Build directory: /Users/chiaracarminati/Downloads/code-4.4.3/build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(NAPI-C-HDF5-test "/Users/chiaracarminati/Downloads/code-4.4.3/build/test/napi_test_hdf5")
add_test(NAPI-C-HDF5-attra-test "/Users/chiaracarminati/Downloads/code-4.4.3/build/test/napi_attra_test_hdf5")
add_test(NAPI-C-leak-test-1 "/Users/chiaracarminati/Downloads/code-4.4.3/build/test/leak_test1")
add_test(NAPI-C-test-nxunlimited "/Users/chiaracarminati/Downloads/code-4.4.3/build/test/test_nxunlimited")
add_test(NAPI-C++-HDF5-test "/Users/chiaracarminati/Downloads/code-4.4.3/build/test/napi_test_cpp-hdf5")
add_test(NAPI-C++-leak-test-2 "/Users/chiaracarminati/Downloads/code-4.4.3/build/test/leak_test2")
add_test(NAPI-C++-leak-test-3 "/Users/chiaracarminati/Downloads/code-4.4.3/build/test/leak_test3")
subdirs(data)
