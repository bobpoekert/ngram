from distutils.core import setup
from Cython.Build import cythonize

setup(
        name='Count-Min',
        ext_modules=cythonize('countmin.pyx'))
