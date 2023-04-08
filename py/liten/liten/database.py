"""
Liten Database
"""
import pyspark
from pyspark.sql import SparkSession
from liten.openai import OpenAI
from liten.work import Work
from liten import utils

class Database:
    """
    Liten Database Class
    """
    def __init__(self):
        """
        Create and initialize Liten Cache
        """
        self.spark_ = SparkSession.builder.master("local[1]") \
                                    .appName('litendata.com') \
                                    .getOrCreate()
        self.work_ = Work()
        self.openai_ = OpenAI()
        pass

    @property
    def spark(self):
        return self.spark_

    @property
    def work(self):
        return self.work_

    def complete_chat(self, prompt):
        resp = self.openai_.complete_prompt_chat(prompt)
        return resp

    def generate_sql(self, prompt):
        sql_cmd = self.openai_.generate_sql(prompt)
        utils.create_new_cell(f"spark.sql(\"{sql_cmd}\").show()")
        return

    def run_query(self, prompt):
        sqlstr = self.openai_.generate_sql(prompt)
        sqlstr.strip()
        print(f"Running the following sql query.\n{sqlstr}\n")
        self.spark_.sql(sqlstr).show()
        return
