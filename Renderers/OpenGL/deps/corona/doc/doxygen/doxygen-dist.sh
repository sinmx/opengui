#!/bin/sh

die() {
  echo "Aborting..."
  exit 1
}

VERSION=1.0.2
BASE=corona-$VERSION-doxygen

rm -rf html $BASE $BASE.tar $BASE.tar.gz $BASE.zip \
       $BASE.tar.bz2 $BASE.chm || die
doxygen corona.doxy          || die
cp -r html $BASE              || die
zip -r $BASE.zip $BASE        || die
tar cfvj $BASE.tar.bz2 $BASE  || die

"C:/Program Files/HTML Help Workshop/hhc" html/index.hhp
mv html/index.chm $BASE.chm || die
