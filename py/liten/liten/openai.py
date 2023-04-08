import random
import time
import os
import openai
from liten import utils
from liten.utils import Suite

def retry_with_exponential_backoff(
        func,
        initial_delay: float = 1,
        exponential_base: float = 2,
        jitter: bool = True,
        max_retries: int = 4,
        errors: tuple = (openai.error.RateLimitError,),
):
    """Retry a function with exponential backoff with delay with retry multiplies like
      delay *= exponential_base * (1 + jitter * random.random())
    func -- wrapper function
    initial_delay -- in secs (default=1)
    exponential_base -- in sec (default=2)
    jitter -- True/False for varying delay
    max_retries -- Number of retries (default=4)
    errors -- Rate limit error for backoff
    """
 
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
def chat_completions_with_backoff(**kwargs):
    return openai.ChatCompletion.create(**kwargs)

# GPT3.5 models https://platform.openai.com/docs/models/gpt-3-5
class GPT35Model:
    gpt_3_5_turbo = 'gpt-3.5-turbo'
    text_davinci_003 = 'text-davinci-003'
    text_davinci_002 = 'text-davinci-002'
    code_davinci_002 = 'code-davinci-002'

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
        # max tokens must be < 4096
        self.max_tokens_=1024
        # Temperature - higher temperature means more variations
        self.temp_=0.5
        # n = number of answers to generate
        self.n_=1
        # stop 
        self.stop_=None
        # model name is like models_.data{id:"modelname"}
        # self.models_=openai.Model.list()
        pass

    @property
    def max_tokens(self):
        return self.max_tokens_;

    def complete_chat(self, messages):
        """
        Generate a response from GPT model
        """
        response = chat_completions_with_backoff(
            model=GPT35Model.gpt_3_5_turbo,
            messages=messages,
            max_tokens=self.max_tokens_,
            n=self.n_,
            stop=self.stop_,
            temperature=self.temp_,
        )
        content = ""
        try:
            content = response['choices'][0]['message']['content']
        except:
            raise Excpetion('Could not get content from OpenAI response')
        return content

    # Generate SQL prompt from given prompt
    
    def summarize(self,prompt):
        msg = [
            {"role": "system", "content" : "Summarize the given text"},
            {"role": "user", "content" : prompt}
        ]
        summary = self.complete_chat(msg)
        return summary

    def generate_sql(self,prompt):
        """
        Generate sql from the given prompt
        """
        msg = [
            {"role": "system", "content" : "Convert given user content to SQL"},
            {"role": "user", "content" : "Count number of rows from weblog table where Status column has 500 errors"},
            {"role": "assistant", "content" : "SELECT COUNT(*) FROM weblog WHERE Status = 500;"},
            {"role": "user", "content": prompt}
        ]
        sql = self.complete_chat(msg)
        return sql
