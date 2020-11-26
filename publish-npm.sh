#!/bin/bash
mv ./README.MD ./README.MD.bak
cp ./node/README.MD ./
npm publish --registry http://registry.npmjs.org
mv ./README.MD.bak ./README.MD

