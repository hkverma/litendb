import openai
import nbformat as nbf
import time
from wrapt_timeout_decorator import *

# Set up the model and prompt
# Pick a correct model for the response
# More details at 
#   https://platform.openai.com/docs/models/gpt-3-5
#
import nbformat
from nbformat import v4 as nbf
from nbformat import current as nbc

from IPython.core.getipython import get_ipython

# %%capture cap

class Session:
    """
    Liten debug session
    """
    session_id=0
    ongoing_session=False

    openai.api_key="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"
    models = ["gpt-3.5-turbo","text-davinci-003", "text-davinci-002", "code-davinci-002"]
    # max tokens must be < 4096
    max_tokens=1024
    # Temperature - higher temperature means more variations
    temp=0.5
    # n = number of answers to generate
    n=1
    # stop 
    stop=None
    # timeout secs - wait before the call timeouts
    timeout_secs=30    
    
    def __init__(self):
        """
        Create and initialize sessions
        """
        pass
    
    def start(self, desc="New interactive session"):
        """
        Start an interactive session 
        """
        if (self.ongoing_session):
            return "First end current session using end_session"
        self.ongoing_session=True
        self.session_id = self.session_id+1        
        print(f"_liten_session_start={self.session_id} desc={desc}")
        session_file = f"liten_session_{self.session_id}.py"
        #    %logstop
        #    %logstart -o -r -t -q f"{self.session_file}"
        return f"Started Session={self.session_id}"

    def stop(self):
        """ 
        End current interactive session 
        """
        if (False == self.ongoing_session):
            return "Session not yet started!"
        print(f"_liten_session_end={self.session_id}")
        self.ongoing_session=False
        return f"Stopped Session={self.session_id}"        
    
    def create_new_cell(self, contents):
        shell = get_ipython()
        payload = dict(
            source='set_next_input',
            text=contents,
            replace=False,
        )
        shell.payload_manager.write_payload(payload, single=False)
        return

    @timeout(timeout_secs)
    def complete_chat(prompt):
        # Generate a response
        completion = openai.Completion.create(
            engine=self.models[1],
            prompt=self.prompt,
            max_tokens=self.max_tokens,
            n=self.n,
            stop=self.stop,
            temperature=self.temp,
        )
        response = completion.choices[0].text
        return response

    # Generate SQL prompt from given prompt
    @timeout(timeout_secs)
    def generate_sql(prompt):
        summarize_prompt = "Summarize the following request:\n"
        summarize_prompt += prompt
        summary = complete_chat(summarize_prompt)
        sql_prompt = "Convert the following text to a SQL statement.\n"
        sql_prompt += prompt
        sql = complete_chat(sql_prompt)
        create_new_cell(sql)
        return summary
