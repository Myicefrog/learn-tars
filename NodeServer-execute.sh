#!/bin/sh

bin="./HelloServer"

ulimit -c 409600
ulimit -a

$bin &
