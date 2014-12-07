#!/usr/bin/env python

import subgraphGenClass

def getGraph(incoming):
  obj = subgraphGenClass.subgraphGen()  # create an instance of graphGenClass
  return obj.generate(incoming)  # return data to MainChare!
