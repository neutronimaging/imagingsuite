#!/bin/bash
base_path=/opt/kiptool

KIPTOOL_BASE_PATH=$base_path
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$base_path
PATH=$PATH:$base_path

export KIPTOOL_BASE_PATH
export LD_LIBRARY_PATH
export PATH
