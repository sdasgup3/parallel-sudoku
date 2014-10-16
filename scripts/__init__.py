#!/usr/bin/env python

import graphGenClass

def getGraph(incoming):
  obj = graphGenClass.graphGen()  # create an instance of graphGenClass
  return obj.generate(incoming)  # return data to MainChare!
