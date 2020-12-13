### Tendb packages into a wheel to be installed by pip or conda
Directions are available at the following link -
https://packaging.python.org/tutorials/packaging-projects/

##### Setup directory structure is shown below

tendb
 * LICENSE
 * README.md
 * tendb_pkg
   * __init__.py
 * setup.py
 * tests

#### Commands
Use the following commands to package and upload.

```bash
python3 setup.py sdist bdist_wheel
python3 -m twine upload --repository testpypi dist/*
```
This is pip install from index-url
```bash
python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps tendb-pkg-tendb
```

Using ctypes to call C functions from Python. Here is an code in python.
```python
from ctypes import cdll
lib = cdll.LoadLibrary('./libtendb.so')

class Foo(object):
    def __init__(self):
        self.obj = lib.Foo_new()

    def bar(self):
        lib.Foo_bar(self.obj)
```

Here is an example run in python
```bash
$ 
$ conda activate tendb
$ pip install /mnt/c/Users/hkver/Documents/dbai/dbaistuff/py/tendb_pkg/
(tendb) $ python3
Python 3.9.0 | packaged by conda-forge | (default, Oct 14 2020, 22:59:50)
[GCC 7.5.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> exec(open("__init__.py").read())
>>> f = Foo()
>>> f.bar()
Hello
>>>
```
TODO 
How to run on jupyter notebook locally.
```bash
$ pip install /mnt/c/Users/hkver/Documents/dbai/dbaistuff/py/tendb_pkg/
```
Follow these
https://github.com/libdynd/dynd-python/blob/master/setup.py
https://docs.python.org/3/distutils/setupscript.html
https://packaging.python.org/guides/distributing-packages-using-setuptools/#package-data
https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html