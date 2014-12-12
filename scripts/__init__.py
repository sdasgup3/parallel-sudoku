#!/usr/bin/env python

#import subgraphGenClass
import graphGenClass

def getGraph(incoming):
  #obj = subgraphGenClass.subgraphGen()  # create an instance of graphGenClass
  obj = graphGenClass.graphGen()  # create an instance of graphGenClass
  return obj.generate(incoming)  # return data to MainChare!
