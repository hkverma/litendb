from liten import openai
import json

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

        self.ids_=0
        self.data_={}
        self.code_={}

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

    def summarize(self, nbname):
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
                if (code_cell_type and key == "outputs"):
                    for output in value:
                        if ("text" in output.keys()):
                            for text in output["text"]:
                                if (self.stop_marker_ in text):
                                    liten_session_id=session_id
                                    session_id=0
                                if (self.start_marker_ in text):
                                    liten_session_id += 1
                                    session_id = liten_session_id
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
            session_data[session_id].append(cell)
            
        # Replace self code & data
        self.code_ = session_code
        self.data_ = session_data
        #print(f"Code={self.code_}")        
        return

    def explain(self, session_id):
        """
        Explain a session id
        """
        return

    def replay(self, session_id):
        """
        Replay the queries of a session
        """
        return

    def complete_chat(self, prompt):
        return self.openai_.complete_chat(prompt)

    def generate_sql(self, prompt):
        return self.openai_.generate_sql(prompt)    
