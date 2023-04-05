from liten import openai
from liten import utils
import json
import re

# %%capture cap

class Session:
    """
    Liten debug session
    """
    def __init__(self):
        """
        Create and initialize sessions
        """

        self.id_=0
        self.active_=False
        self.start_marker_="_liten_session_start"
        self.stop_marker_="_liten_session_end"

        self.data_={}
        self.code_={}
        self.summary_={}

        self.openai_ = openai.OpenAI()
        self.start("Default session")

    @property
    def id(self):
        return self.id_

    @property
    def code(self):
        """
        return code for all session ids
        """
        return self.code_

    @property
    def data(self):
        """
        return data for all session ids
        """
        return self.data_
    
    def new(self, desc="New interactive session"):
        """
        Start a new interactive session. Stops if an ongoing session.
        """
        if (self.active_):
            self.stop()
        return self.start(desc)
        
    def start(self, desc="New interactive session"):
        """
        Start an interactive session 
        """
        if (self.active_):
            return "First end current session using end_session"
        self.active_=True
        self.id_ = self.id_+1        
        print(f"Started {self.start_marker_}={self.id_} desc={desc}")
        #session_file = f"liten_session_{self.id_}.py"
        #    %logstop
        #    %logstart -o -r -t -q f"{self.session_file}"
        return

    def stop(self):
        """ 
        End current interactive session 
        """
        if (False == self.active_):
            return "Session not yet started!"
        print(f"Stopped {self.stop_marker_}={self.id_}")
        self.active_=False
        return

    def if_image_cell(self, cell):
        """
        return true if cell contains an image
        """
        for k1,v1 in cell.items():
            if 'outputs' == k1:
                for e1 in v1:
                    for k2,v2 in e1.items():
                        if 'data' == k2 and 'image/png' in v2.keys():
                            return True
        return False
        
    def load(self, nbname):
        """
        Read all sessions and summarize each of them
        """
        with open(nbname) as datafile:
            data = json.load(datafile)

        liten_session_id=0
        session_id=0
        session_ids=[0]
        session_data={}
        session_code={}

        # format
        # { cell_type: code|markdown, source:[..], outputs:[{name:stdout,text:[liten..]}]} }
        for cell in data["cells"]:
            code =[]
            code_cell_type = (cell['cell_type'] == 'code')
            
            # session can change only across cells, see if this cell changes session
            # if session_start called in the cell it will add this cell in new session
            for key, value in cell.items():
                if (key == "outputs"):
                    for output in value:
                        if ("text" in output.keys()):
                            for text in output["text"]:
                                if (self.stop_marker_ in text):
                                    liten_session_id=session_id
                                    session_id=0
                                if (self.start_marker_ in text):
                                    m = re.search(f"{self.start_marker_}=([0-9]+)", text)
                                    if (m.groups()):
                                        session_id = int(m.group(1))
                                    else:
                                        session_id = 0
                                        print("Error extracting session id from liten start marker")
                                
                                            
                                            
                if (code_cell_type and key == "source"):
                    code.append(value)
            # Extract code in session_code session_id
            if session_id not in session_code.keys():
                session_code[session_id] = []
            for instrs in code:
                for instr in instrs:
                    session_code[session_id].append(instr)
            # Append to session data
            if session_id not in session_data.keys():
                session_data[session_id] = []
            if not self.if_image_cell(cell):
                session_data[session_id].append(cell)
            
        # Replace self code & data
        self.code_ = session_code
        self.data_ = session_data
        #print(f"Code={self.code_}")        
        return

    def reduce_prompt_size(self, prompt):
        """
        remove image etc to reduce prompt size
        """
        words = prompt.split();
        max_tokens = self.openai_.max_tokens
        rprompt = prompt
        if (len(words) > max_tokens):
            #rprompt = ' '.join(words[0:max_tokens])
            rprompt = prompt[0:5*max_tokens]
        return rprompt
        
    def summarize(self):
        for k,v in self.data_.items():
            prompt = "Following are cells from a python notebook from jupyter in json format. Please summarize what it is trying to do?\n"
            prompt += json.dumps(v)
            rprompt = self.reduce_prompt_size(prompt)
            self.summary_[k] = self.openai_.complete_chat(rprompt).strip()
            print(f"Session {k}: {self.summary_[k]}\n")
        return
        
    def explain(self, id):
        """
        Explain a session id
        """
        if (id not in self.data_):
            print(f"Session {id}: does not exist")
        prompt = "Following are cells of a python notebook from jupyter is json format. Please explain what it is trying to do?\n"+json.dumps(self.data_[id])
        rprompt = self.reduce_prompt_size(prompt)
        s = self.openai_.complete_chat(rprompt)
        print(f"Session {id}: {s.strip()}\n")
        return

    def replay(self, id):
        """
        Replay the queries of a session
        """
        if (id not in self.code_):
            print(f"Session {id}: does not exist")
        utils.create_new_cell(self.code_[id])
        return

    def complete_chat(self, prompt):
        return self.openai_.complete_chat(prompt)

    def generate_sql(self, prompt):
        return self.openai_.generate_sql(prompt)

    def find_similar(self, prompt):
        chatprompt = "Find a list of sessions below. Each session starts with session keyword followed by a number. It is followed by a short summary.\n"
        for k,v in self.summary_.items():
            chatprompt += f"Session {k} = {v}\n"
        chatprompt += "List the session closest to the following session summary.\n"
        chatprompt += prompt
        rchatprompt = self.reduce_prompt_size(chatprompt)
        answer = self.openai_.complete_chat(rchatprompt)
        print(f"{answer.strip()}\n")
        return
        
