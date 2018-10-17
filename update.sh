#!/bin/bash
set -euo pipefail

readonly NAME="abrasive"

readonly tmpDir=/tmp/deliver-$NAME-$$
trap "rm -rf $tmpDir" EXIT
mkdir -p $tmpDir

export BIN=bin/asmjs
./scripts/asmjs-make clean
./scripts/asmjs-make -j`nproc`

#------------------------------------------------------------------------------
# create game directory
#------------------------------------------------------------------------------
readonly gameDir=$tmpDir/$NAME
mkdir -p $gameDir

cp -a bin/asmjs/rel/* $gameDir
#cp index.html $gameDir/index.html

#------------------------------------------------------------------------------
# archive it
#------------------------------------------------------------------------------
pushd $tmpDir
zip $NAME.zip -r $NAME
popd

mv $tmpDir/$NAME.zip .

#------------------------------------------------------------------------------
# upload it to code.alaiwan.org
#------------------------------------------------------------------------------
rsync \
  --compress \
  --delete \
  -vr $gameDir/* alaiwans@code.alaiwan.org:public_html/games/$NAME

