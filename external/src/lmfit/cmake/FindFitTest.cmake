# Find libFitTest
#
# Usage:
#   find_package(FitTest [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - FitTest_FOUND        .. true if library is found
#   - FitTest_LIBRARIES    .. full path to library
#   - FitTest_INCLUDE_DIR  .. full path to include directory
#
# Copyright 2017 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

find_path(FitTest_INCLUDE_DIR fittest.h)
find_library(FitTest_LIBRARIES NAMES fittest FitTest)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FitTest DEFAULT_MSG FitTest_LIBRARIES FitTest_INCLUDE_DIR)

include(AssertLibraryFunction)
assert_library_function(FitTest print_help "")
assert_library_function(FitTest run_tests "")

mark_as_advanced(FitTest_INCLUDE_DIR FitTest_LIBRARIES)
