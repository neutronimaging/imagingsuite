import os
import sys

cur_file_dir = os.path.dirname(os.path.realpath(__file__))

sys.path.append(cur_file_dir)

# On Windows, add the package directory to DLL search path
# so that pybind11 extensions can find their dependency DLLs.
if sys.platform == "win32" and hasattr(os, "add_dll_directory"):
    os.add_dll_directory(cur_file_dir)

from advfilters import *
from muhrectomo import *
from imgalg import *

