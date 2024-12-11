from distutils.core import setup
import Cython.Build
setup(
    ext_modules = Cython.Build.cythonize("../BlenderExport.py"))