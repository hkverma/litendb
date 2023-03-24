from liten import openai

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
        self.openai_ = openai.OpenAI()
        self.start("Default session")
        pass

    @property
    def id(self):
        self.id_
    
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
        print(f"_liten_session_start={self.id_} desc={desc}")
        #session_file = f"liten_session_{self.id_}.py"
        #    %logstop
        #    %logstart -o -r -t -q f"{self.session_file}"
        return f"Started Session={self.id_}"

    def stop(self):
        """ 
        End current interactive session 
        """
        if (False == self.active_):
            return "Session not yet started!"
        print(f"_liten_session_end={self.id_}")
        self.active_=False
        return f"Stopped Session={self.id_}"        
    

    def complete_chat(self, prompt):
        return self.openai_.complete_chat(prompt)

    def generate_sql(self, prompt):
        return self.openai_.generate_sql(prompt)    
