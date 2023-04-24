import random
import time
import os
import openai
import tiktoken
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
        # max tokens must be < 4096, this is number of input tokens
        self.max_tokens_=4096
        self.max_output_tokens_=1024
        self.max_input_tokens_=2*1024
        # Temperature - higher temperature means more variations
        self.temp_=0.5
        # n = number of answers to generate
        self.n_=1
        # stop 
        self.stop_=None
        # model name is like models_.data{id:"modelname"}
        # self.models_=openai.Model.list()
        self.model_ = GPT35Model.gpt_3_5_turbo
        #self.encoding = tiktoken.get_encoding('cl100k_base')
        self.encoding_ = tiktoken.encoding_for_model(self.model_)
        pass

    @property
    def max_tokens(self):
        return self.max_tokens_;

    @property
    def max_input_tokens(self):
        return self.max_input_tokens_;

    @property
    def max_output_tokens(self):
        return self.max_output_tokens_;

    def reduce_prompt_size(self, prompt, num_tokens):
        """
        remove image etc to reduce prompt size
        """
        tokens=self.encoding_.encode(prompt)
        reduced_prompt=prompt
        if (len(tokens) > num_tokens):
            reduced_prompt = self.encoding_.decode(tokens[:num_tokens-1])
        return reduced_prompt
    
    def complete_chat(self, messages):
        """
        Generate a response from GPT model
        """
        response = chat_completions_with_backoff(
            model=self.model_,
            messages=messages,
            max_tokens=self.max_output_tokens_, # Max tokens to generate in output
            n=self.n_,  # Number of responses
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
            {"role": "system", "content" : "Convert given user content to SQL. The output should contain only SQL code."},
            {"role": "user", "content" : "Count number of rows from weblog table where Status column has 500 errors"},
            {"role": "assistant", "content" : "SELECT COUNT(*) FROM weblog WHERE Status = 500;"},
            {"role": "user", "content": prompt}
        ]
        sql = self.complete_chat(msg)
        return sql

    def complete_prompt_chat(self, prompt):
        msg = [
            {"role": "system", "content" : "Complete the given prompt and its directives"},
            {"role": "user", "content" : prompt}
        ]
        resp = self.complete_chat(msg)
        return resp

    def generate_java_code(self, prompt, class_name):
        """
        Generate java code from the prompt
        """
        msg = [
            {"role": "system", "content" : "Generate java code with a class that performs the given task. The class name must be " + class_name},
            {"role": "user", "content": prompt}
        ]
        java_code = self.complete_chat(msg)
        return java_code

    def generate_junit_code(self, java_code, desc):
        """
        Generate junit code for the given java code
        """
        content_prompt = "Generate java junit test for the given java code. The test code class name should be name of the class with Test appended to the name. "
        if (desc):
            content_prompt = content_prompt+ "This code performs the following operations." + desc
        content_prompt = content_prompt + " This is the actual java code for which you have to generate junit code." + java_code
        msg = [
            {"role": "system", "content" : "Generate java unit junit code with a class that tests the given code."},
            {"role": "user", "content" : content_prompt}
        ]
        junit_code = self.complete_chat(msg)
        return junit_code

