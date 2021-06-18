#!/usr/bin/env bash

phpize --clean && phpize && ./configure && make -j4 && sudo make install