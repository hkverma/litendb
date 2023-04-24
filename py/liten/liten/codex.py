import random
import time
import os
import openai
import tiktoken
from liten import utils
from liten.utils import Suite
from liten.openai import OpenAI
from liten import utils

class Story:
    def __init__(self, raw):
        self.raw_= raw
        self.openai_ = OpenAI()
        pass

    @property
    def raw(self):
        return self.raw_

    @property
    def java_code(self):
        return self.java_code_

    @property
    def junit_code(self):
        return self.junit_code_
    
    def generate_java_code(self, class_name):
        self.class_name_= class_name
        self.java_code_ = self.openai_.generate_java_code(self.raw_, class_name)
        return self.java_code_

    def generate_junit_code(self):
        self.junit_code_ = self.openai_.generate_junit_code(self.raw_, self.java_code_)
        return self.junit_code_

    def write_java(self):
        java_file = self.class_name_+".java"
        jf = open(java_file, "w")
        jf.write(code)
        jf.close()

    def write_junit(self):
        junit_file = self.class_name_+"Test.java"
        jf = open(junit_file, "w")
        jf.write(code)
        jf.close()
        
class Codex:
    """
    Generate Code
    """
    def __init__(self):
        """
        Create and initialize works
        """
        self.stories_=[]

    def add_story(self, story):
        """
        Add an story to the code
        """
        st = Story(story)
        self.stories_.append(st)
        return st
