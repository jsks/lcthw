#!/bin/zsh
# Script to setup our environment for lcthw-ex26
setopt err_return

# Just to make absolute sure that we're in the right directory
EX_DIR=$HOME/libris/c-thehardway/ex26

APR_VERSION=1.4.6
APRUTIL_VERSION=1.4.1
BSTRLIB_VERSION=05122010

cd $EX_DIR
if [[ ${(L)1} == "clean" ]]; then
    rm -rf $EX_DIR/usr
    rm -rf $EX_DIR/apr-$APR_VERSION
    rm -rf $EX_DIR/apr-util-$APRUTIL_VERSION
    rm bstrlib.h
    rm bstrlib.c

    return
fi

mkdir -p usr/local &>/dev/null

if [[ ! -d apr-$APR_VERSION ]]; then
    wget http://archive.apache.org/dist/apr/apr-$APR_VERSION.tar.gz

    tar -xzvf apr-$APR_VERSION.tar.gz
    rm apr-$APR_VERSION.tar.gz
fi

cd apr-$APR_VERSION

./configure --prefix=$EX_DIR/usr/local
make
make install

cd -

if [[ ! -d apr-util-$APRUTIL_VERSION ]]; then
    wget http://archive.apache.org/dist/apr/apr-util-$APRUTIL_VERSION.tar.gz

    tar -xzvf apr-util-$APRUTIL_VERSION.tar.gz
    rm apr-util-$APRUTIL_VERSION.tar.gz
fi

cd apr-util-$APRUTIL_VERSION

./configure --with-apr=$EX_DIR/usr/local/ --prefix=$EX_DIR/usr/local
make
make install

cd -

if [[ ! -f bstrlib.h || ! -f bstrlib.c ]]; then
    wget http://downloads.sourceforge.net/project/bstring/bstring/rc31/bstrlib-$BSTRLIB_VERSION.zip

    unzip bstrlib-$BSTRLIB_VERSION.zip -d bstrlib-$BSTRLIB_VERSION
    mv bstrlib-$BSTRLIB_VERSION/bstrlib.h $EX_DIR/
    mv bstrlib-$BSTRLIB_VERSION/bstrlib.c $EX_DIR/
    rm -rf $EX_DIR/bstrlib-$BSTRLIB_VERSION

    rm bstrlib-$BSTRLIB_VERSION.zip
fi

# I hate hard carriage returns and OSX sed
sed -i '' -e "2759s/.*/\#if defined(__GNU__) \&\& \!defined(__APPLE__)/" bstrlib.c
tr -d '' < bstrlib.c > bstrlib.out
mv bstrlib.out bstrlib.c

touch Makefile README
cp $EX_DIR/../dbg.h .
