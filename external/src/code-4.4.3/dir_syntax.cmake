#
# syntax:    cmake -DNATIVE:PATH="something" -P dir_syntax.cmake
#
file(TO_CMAKE_PATH "${NATIVE}" TRANS)
file(WRITE "dir_syntax.txt" "${TRANS}")
