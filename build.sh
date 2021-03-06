#!/usr/bin/env sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

(
    cd $DIR
    mkdir -p build
    cd build
    
    if [[ "$OSTYPE" == "darwin*" ]]; then
        cmake -D CMAKE_PREFIX_PATH=$(brew --prefix qt5) -D CMAKE_BUILD_TYPE=Release ../
    else 
        cmake -D CMAKE_BUILD_TYPE=Release ../
    fi

    make
)
