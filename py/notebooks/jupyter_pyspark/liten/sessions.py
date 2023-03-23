import openai
openai.api_key="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"
import nbformat as nbf
import time
from wrapt_timeout_decorator import *

%%capture cap

session_id=0
ongoing_session=False

def start_session(desc="New interactive session"):
    """ Start an interactive session """
    global ongoing_session, session_id, liten_session
    if (ongoing_session):
        return "First end current session using end_session"
    print(f"_liten_session_start={session_id} desc={desc}")
    session_file = "liten_session_" + str(session_id) + ".py"
    %logstop
    %logstart -o -r -t -q {session_file}
    ongoing_session=True
    session_id = session_id+1

def stop_session():
    """ End current interactive session """
    global ongoing_session, session_id
    print(f"_liten_session_end={session_id}")
    ongoing_session=False
    %logstop

# Set up the model and prompt
# Pick a correct model for the response
# More details at 
#   https://platform.openai.com/docs/models/gpt-3-5
#
import nbformat
from nbformat import v4 as nbf
from nbformat import current as nbc

def create_new_cell(contents):
    from IPython.core.getipython import get_ipython
    shell = get_ipython()
    payload = dict(
        source='set_next_input',
        text=contents,
        replace=False,
    )
    shell.payload_manager.write_payload(payload, single=False)

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

# openai.Model.list()
@timeout(timeout_secs)
def completeChat(prompt):
    # Generate a response
    completion = openai.Completion.create(
        engine=models[1],
        prompt=prompt,
        max_tokens=max_tokens,
        n=n,
        stop=stop,
        temperature=temp,
    )
    response = completion.choices[0].text
    return response

# Generate SQL prompt from given prompt
@timeout(timeout_secs)
def generateSql(prompt):
    summarize_prompt = "Summarize the following request:\n"
    summarize_prompt += prompt
    summary = completeChat(summarize_prompt)
    sql_prompt = "Convert the following text to a SQL statement.\n"
    sql_prompt += prompt
    sql = completeChat(sql_prompt)
    create_new_cell(sql)
    return summary
