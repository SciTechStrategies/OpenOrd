#!/bin/bash

# This is a shell script to clean a recursive/online vxord project.
#
# S. Martin
# 5/9/06

# input to this program is the project name directory
PROJECT=$1

# input to this program is 
if [ "$PROJECT" == "" ]
then
  echo "This program cleans a recursive/online vxord project."
  echo "The format of the command is"
  echo ""
  echo "    clean_project.sh project_directory"
  echo ""
  echo "where project_directory is the directory containing the project."
fi

echo "Cleaning" $PROJECT "..."
rm $PROJECT/*coord
rm $PROJECT/*edges
rm $PROJECT/*int
rm $PROJECT/*.full
rm $PROJECT/*.real
rm $PROJECT/*.ind
rm $PROJECT/*.clust
