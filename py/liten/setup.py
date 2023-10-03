import contextlib
import os
import os.path
from os.path import join as pjoin
import re
import shlex
import shutil
import sys
from pathlib import Path

from Cython.Distutils import build_ext as _build_ext
import Cython
from setuptools import setup, Extension, Distribution
from distutils import sysconfig

def get_version() :
    fp = open('litendb/version.py')
    version = fp.readline().strip()
    fp.close()
    return version
VERSION = get_version()

import numpy

#from distutils.command.build_ext import build_ext

# Based on https://github.com/libdynd setup.py & arrow setup.py

# Check if we're running 64-bit Python
is_64_bit = sys.maxsize > 2**32

if Cython.__version__ < '0.29':
    raise Exception('Please upgrade to Cython 0.29 or newer')

setup_dir = os.path.abspath(os.path.dirname(__file__))

try:
    liten_root_dir = os.environ.get('LITEN_ROOT_DIR')
except KeyError:
    print("Must set LITEN_ROOT_DIR")

ext_suffix = sysconfig.get_config_var('EXT_SUFFIX')
if ext_suffix is None:
    # https://bugs.python.org/issue19555
    ext_suffix = sysconfig.get_config_var('SO')

class BinaryDistribution(Distribution):
    def has_ext_modules(foo):
        return True
    
@contextlib.contextmanager
def changed_dir(dirname):
  oldcwd = os.getcwd()
  os.chdir(dirname)
  try:
    yield
  finally:
    os.chdir(oldcwd)

class cmake_build_ext(_build_ext):
  description = "Build the C-extension for dynd-python with CMake"
  user_options = [('extra-cmake-args=', None, 'extra arguments for CMake')]

  CYTHON_MODULE_NAMES = [
    'lib']

  def build_extensions(self):
    self.extensions = [ext for ext in self.extensions
                      if ext.name != '__dummy__']
    for ext in self.extensions:
      if (hasattr(ext, 'include_dirs') and
               self.pyarrow_incl not in ext.include_dirs):
         ext.include_dirs.append(numpy_incl)
    print(f"built extenstion {self.extensions}")
    _build_ext.build_extensions(self)

  def initialize_options(self):
    _build_ext.initialize_options(self)
    self.build_type = os.environ.get('LITEN_BUILD_TYPE','debug').lower()
    # check if build_type is correctly passed / set
    if self.build_type.lower() not in ('release', 'debug'):
      raise ValueError("--build-type needs to be 'release' or 'debug'")
    self.pyarrow_incl = os.environ.get('LITEN_ROOT_DIR') + "/cpp/opensource/arrow/python/pyarrow"
    if not Path(self.pyarrow_incl).is_dir():
      raise Exception(f"liten pyarrow dir {pyarrow_incl} does not exist")
    print(f"initialized options {self.pyarrow_incl}")

  def run(self):
    self._run_cmake()
    _build_ext.run(self)

  def get_ext_built(self, name):
    return pjoin(name + ext_suffix)

  def _failure_permitted(self, name):
    return False

  def _bundle_liten_cpp(self, build_prefix, build_lib):
    liten_lib = pjoin(liten_root_dir,'cpp','build', self.build_type.lower(), 'bin','libliten.so')
    if not liten_lib:
      raise Exception("Could not find " + liten_lib)
    shutil.copyfile(liten_lib, pjoin(build_prefix, build_lib, 'litendb','libliten.so'))
                    
  def _run_cmake(self):

    # The directory containing this setup.py
    source = os.path.dirname(os.path.abspath(__file__))

    # The staging directory for the module being built
    build_cmd = self.get_finalized_command('build')
    build_temp = pjoin(os.getcwd(), build_cmd.build_temp)
    build_lib = pjoin(os.getcwd(), build_cmd.build_lib)
    saved_cwd = os.getcwd()

    if not os.path.isdir(build_temp):
      self.mkpath(build_temp)

    # Change to the build directory
    with changed_dir(build_temp):
      # Detect if we built elsewhere
      if os.path.isfile('CMakeCache.txt'):
        cachefile = open('CMakeCache.txt', 'r')
        cachedir = re.search('CMAKE_CACHEFILE_DIR:INTERNAL=(.*)',
                             cachefile.read()).group(1)
        cachefile.close()
        if (cachedir != build_temp):
          return

      static_lib_option = ''

      cmake_options = [
        '-DPYTHON_EXECUTABLE=%s' % sys.executable,
        '-DPython3_EXECUTABLE=%s' % sys.executable,
        static_lib_option,
      ]

      def append_cmake_bool(value, varname):
        cmake_options.append('-D{0}={1}'.format(
          varname, 'on' if value else 'off'))

      cmake_options.append('-DCMAKE_BUILD_TYPE={0}'
                           .format(self.build_type.lower()))

      cmake_options.append('-DLITEN_BIN_DIR={0}'
                           .format(os.getcwd()))
      
      extra_cmake_args = []
      build_tool_args = []

      # Generate the build files
      print("-- Running cmake for liten")
      self.spawn(['cmake'] + extra_cmake_args + cmake_options + [source])
      print("-- Finished cmake for liten")

      print("-- Running cmake --build for liten")
      self.spawn(['cmake', '--build', '.', '--config', self.build_type] +
                 build_tool_args)
      print("-- Finished cmake --build for liten")

      if self.inplace:
        # a bit hacky
        build_lib = saved_cwd

      # Move the libraries to the place expected by the Python build
      try:
        os.makedirs(pjoin(build_lib, 'litendb'))
      except OSError:
        pass

      build_prefix = ''

      # Move the built C-extension to the place expected by the Python build
      self._found_names = []
      for name in self.CYTHON_MODULE_NAMES:
        built_path = self.get_ext_built(name)
        if not os.path.exists(built_path):
          print('Did not find {0}'.format(built_path))
          if self._failure_permitted(name):
            print('Cython module {0} failure permitted'
                  .format(name))
            continue
          raise RuntimeError('liten C-extension failed to build:',
                             os.path.abspath(built_path))

        # The destination path to move the built C extension to
        ext_path = pjoin(build_lib, 'litendb', self.get_ext_built(name))
        if os.path.exists(ext_path):
          os.remove(ext_path)
        self.mkpath(os.path.dirname(ext_path))

        print('Moving built C-extension', built_path,
              'to build path', ext_path)
        shutil.move(built_path, ext_path)
        self._found_names.append(name)

      print('Bundling liten C-libs prefix=', build_prefix, ' lib=', build_lib)
      self._bundle_liten_cpp(build_prefix, build_lib)

with open("README.md", "r") as fh:
  long_description = fh.read()

setup(
  name='litendb',
  version=VERSION,
  author='HK Verma',
  author_email='hkverma@gmail.com',
  description='Big Data Analytics Toolset',
  long_description=long_description,
  long_description_content_type='text/markdown',
  url='https://github.com/liten/',
  # pkg data
  packages=['litendb'],
  package_dir={'litendb': 'litendb'},
  zip_safe=False,
  package_data={'litendb': ['*.pxd','*.pyx','includes/*.pxd']},
  include_package_data=True,
#  distclass=BinaryDistribution,
  # Dummy extenstion to trigger build_ext
  ext_modules=[Extension('__dummy__', sources=[])],
  # This includes both build and install requirements. Setuptools' setup_requires
  # option does not actually install things, so isn't actually helpful...
  install_requires = ['cython==3.0.2',
                      'pyarrow==13.0.0',
                      'numpy',
                      'graphviz',
                      'tbb-devel'],
  cmdclass={
    'build_ext': cmake_build_ext
  },

  classifiers=[
    "Programming Language :: Python :: 3",
    "License :: OSI Approved :: MIT License",
    "Operating System :: OS Independent",
  ],
  python_requires='>=3.10',
)
