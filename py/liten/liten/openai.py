import openai
import time
from wrapt_timeout_decorator import *
from liten import utils

class OpenAI:
    """
    Liten openai interface - setup model and prompt
    """
    # timeout secs - wait before the call timeouts
    timeout_secs_=30    
    def __init__(self):
        """
        Initialize openai variables
        """
        openai.api_key="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"
        # Pick a model - https://platform.openai.com/docs/models/gpt-3-5
        self.models_ = [
            "gpt-3.5-turbo",
            "text-davinci-003",
            "text-davinci-002",
            "code-davinci-002"]
        # max tokens must be < 4096
        self.max_tokens_=1024
        # Temperature - higher temperature means more variations
        self.temp_=0.5
        # n = number of answers to generate
        self.n_=1
        # stop 
        self.stop_=None
        pass

    @timeout(timeout_secs_)
    def complete_chat(self, prompt):
        # Generate a response
        completion = openai.Completion.create(
            engine=self.models_[1],
            prompt=prompt,
            max_tokens=self.max_tokens_,
            n=self.n_,
            stop=self.stop_,
            temperature=self.temp_,
        )
        response = completion.choices[0].text
        return response

    # Generate SQL prompt from given prompt
    @timeout(timeout_secs_)
    def generate_sql(self,prompt):
        summarize_prompt = "Summarize the following request:\n"
        summarize_prompt += prompt
        summary = self.complete_chat(summarize_prompt)
        sql_prompt = "Convert the following text to a SQL statement.\n"
        sql_prompt += prompt
        sql = self.complete_chat(sql_prompt)
        spark_sql = "sqlDf=spark.sql(\""+sql.strip()+"\")\nsqlDf.show()"
        utils.create_new_cell(spark_sql)
        return summary
