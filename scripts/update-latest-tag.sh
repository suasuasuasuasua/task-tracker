#!/usr/bin/env bash

git describe --tags --abbrev=0 >VERSION
git add VERSION
