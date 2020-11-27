### tendb packages into a wheel to be installed by pip or conda
Directions are available at the following link -
https://packaging.python.org/tutorials/packaging-projects/

##### setup directory structure is shown below
tendb
├── LICENSE
├── README.md
├── tendb_pkg
│   └── __init__.py
├── setup.py
└── tests

#### Commands
Use the following commands to package and upload.

```
python3 setup.py sdist bdist_wheel
python3 -m twine upload --repository testpypi dist/*
```
This is pip install from index-url
```
python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps tendb-pkg-tendb
```

