import random
import time
import os
import openai
from liten import utils

# define a retry decorator
def retry_with_exponential_backoff(
        func,
        initial_delay: float = 1,
        exponential_base: float = 2,
        jitter: bool = True,
        max_retries: int = 4,
        errors: tuple = (openai.error.RateLimitError,),
):
    """Retry a function with exponential backoff."""
 
    def wrapper(*args, **kwargs):
        # Initialize variables
        num_retries = 0
        delay = initial_delay
 
        # Loop until a successful response or max_retries is hit or an exception is raised
        while True:
            try:
                return func(*args, **kwargs)
 
            # Retry on specific errors
            except errors as e:
                # Increment retries
                num_retries += 1
 
                # Check if max retries has been reached
                if num_retries > max_retries:
                    raise Exception(
                        f"Maximum number of retries ({max_retries}) exceeded."
                    )
 
                # Increment the delay
                delay *= exponential_base * (1 + jitter * random.random())
 
                # Sleep for the delay
                time.sleep(delay)
 
            # Raise exceptions for any errors not specified
            except Exception as e:
                raise e
 
    return wrapper
    
@retry_with_exponential_backoff
def completions_with_backoff(**kwargs):
    return openai.Completion.create(**kwargs)

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
        openai.api_key= os.environ["OPENAI_API_KEY"]
        # Pick a model - https://platform.openai.com/docs/models/gpt-3-5
        self.models_ = [
            "gpt-3.5-turbo",
            "text-davinci-003",
            "text-davinci-002",
            "code-davinci-002"]
        self.model_ = self.models_[1]
        # max tokens must be < 4096
        self.max_tokens_=1024
        # Temperature - higher temperature means more variations
        self.temp_=0.5
        # n = number of answers to generate
        self.n_=1
        # stop 
        self.stop_=None
        pass

    @property
    def max_tokens(self):
        return self.max_tokens_;

    def complete_chat(self, prompt):
        # Generate a response
        completion = completions_with_backoff(
            engine=self.model_,
            prompt=prompt,
            max_tokens=self.max_tokens_,
            n=self.n_,
            stop=self.stop_,
            temperature=self.temp_,
        )
        response = completion.choices[0].text
        return response

    # Generate SQL prompt from given prompt
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
