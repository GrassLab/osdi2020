#!/bin/sh

sudo kill $(ps -aux | grep picocom | grep -v sudo | grep -v grep | awk '{print $2}')

