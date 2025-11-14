#!/bin/bash

POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
  case $1 in
    -b|--build)
      python3 setup.py bdist_wheel
      shift # past argument
      ;;
    -u|--upload)
      python3 -m twine upload dist/litendb-*-manylinux*.whl
      shift # past argument
      ;;
    -t|--testload)
      python3 -m twine upload --repository testpypi dist/litendb-*-manylinux*.whl
      shift # past argument
      ;;
    -r|--repair)
      pushd dist
      auditwheel repair litendb-*-linux_x86_64.whl -w . --plat manylinux_2_39_x86_64
      popd
      shift # past argument
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

