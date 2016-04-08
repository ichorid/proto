#!/bin/bash
grep KEY $1 | sed 's/KEY\[.*\]: //'
