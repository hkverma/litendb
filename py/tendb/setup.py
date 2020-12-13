import setuptools

from Cython.Distutils import build_ext as _build_ext
import Cython
from setuptools import setup, Extension

#from distutils.command.build_ext import build_ext

# Based on https://github.com/libdynd setup.py & arrow setup.py

class cmake_build_ext(_build_ext):
  description = "Build the C-extension for dynd-python with CMake"
  user_options = [('extra-cmake-args=', None, 'extra arguments for CMake')]

  def run(self):
    global build_type
    
with open("README.md", "r") as fh:
  long_description = fh.read()

setuptools.setup(
  name='tendb',
  version='0.0.1',
  author='HK Verma',
  author_email='hkverma@gmail.com',
  description='Big Data Analytics Toolset',
  long_description=long_description,
  long_description_content_type='text/markdown',
  url='https://github.com/tendb/',
  # pkg data
    packages=['tendb'],
  zip_safe=False,
  package_data={'tendb': ['*.pxd','*.pyx','includes/*.pxd']},
  include_package_data=True,
  #    distclass=BinaryDistribution,
  # build_ext is overridden to call cmake, the Extension is just
  # needed so things like bdist_wheel understand what's going on
  ext_modules=[Extension('tendb', sources=["includes/__init__.pxd","includes/common.pxd","includes/libarrow.pxd"])],
  # This includes both build and install requirements. Setuptools' setup_requires
  # option does not actually install things, so isn't actually helpful...
    install_requires = ['cython'],
  cmdclass={
    'build_ext': cmake_build_ext
  },
  
  classifiers=[
    "Programming Language :: Python :: 3",
    "License :: OSI Approved :: MIT License",
    "Operating System :: OS Independent",
  ],
  python_requires='>=3.6',
)
