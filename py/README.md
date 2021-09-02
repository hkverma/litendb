
## Liten packages into a wheel to be installed by pip or conda
V 0.0.1 - Created Demo branch 26 July 2021

Directions are available at the following link -
https://packaging.python.org/tutorials/packaging-projects/
### Demo notebooks
| ------  | ------ |
| Notebook | Description |
| ------  | ------ |
| LitenStandaloneQ5Q6.ipynb | Custom Query6 and Query5 |
| LitenRaySingleActorQ5Q6.ipynb | Ray remote with one node |
| LitenNYTaxiSQLXGBoost.ipynb | XGBoost on Liten data |
| LitenBasicSentimentAnalysis.ipynb | Sentiment analysis on Liten data |
| LitenPCapIPPktAnalyzer.ipynb | Liten pcap packet analyzer |

### ctype use to intgerate C++ code

##### Example setup directory structure is shown below

liten
 * LICENSE
 * README.md
 * liten_pkg
   * __init__.py
 * setup.py
 * tests

#### Commands
Use the following commands to package and upload.

```bash
python3 setup.py sdist
python3 setup.py bdist_wheel
python3 -m twine upload --repository dist/*
```
This is pip install from index-url
```bash
python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps liten-pkg-liten
```

Using ctypes to call C functions from Python. Here is an code in python.
```python
from ctypes import cdll
lib = cdll.LoadLibrary('./libliten.so')

class Foo(object):
    def __init__(self):
        self.obj = lib.Foo_new()

    def bar(self):
        lib.Foo_bar(self.obj)
```

Here is an example run in python
```bash
$ 
$ conda activate liten
$ pip install /mnt/c/Users/hkver/Documents/dbai/dbaistuff/py/liten_pkg/
(liten) $ python3
Python 3.9.0 | packaged by conda-forge | (default, Oct 14 2020, 22:59:50)
[GCC 7.5.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> exec(open("__init__.py").read())
>>> f = Foo()
>>> f.bar()
Hello
>>>
```
How to run on jupyter notebook locally.
```bash
$ pip install /mnt/c/Users/hkver/Documents/dbai/dbaistuff/py/liten_pkg/
```
Follow these
https://github.com/libdynd/dynd-python/blob/master/setup.py
https://docs.python.org/3/distutils/setupscript.html
https://packaging.python.org/guides/distributing-packages-using-setuptools/#package-data
https://cython.readthedocs.io/en/latest/src/userguide/wrapping_CPlusPlus.html

To save environments
```bash
conda env export > environment.yml
```
TODO - Clean notebooks. Reproduce on Azure web.
       Draw query plan, print query and result for each query.
       Complete ML and NLP training data predictions
