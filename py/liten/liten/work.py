from liten.openai import OpenAI
from liten import utils
import json
import re

# %%capture cap

class Work:
    """
    Liten debug work
    """
    def __init__(self):
        """
        Create and initialize works
        """

        self.item_=0
        self.active_=False
        self.start_marker_="_liten_work_start"
        self.stop_marker_="_liten_work_end"

        self.data_={}
        self.code_={}
        self.summary_={}

        self.openai_ = OpenAI()
        self.start("Default work")

    @property
    def item(self):
        return self.item_

    @property
    def code(self):
        """
        return code for all work ids
        """
        return self.code_

    @property
    def data(self):
        """
        return data for all work ids
        """
        return self.data_
    
    def new(self, desc="New interactive work"):
        """
        Start a new interactive work. Stops if an ongoing work.
        """
        if (self.active_):
            self.stop()
        return self.start(desc)
        
    def start(self, desc="New interactive work"):
        """
        Start an interactive work 
        """
        if (self.active_):
            return "First end current work using end_work"
        self.active_=True
        self.item_ = self.item_+1        
        print(f"Started {self.start_marker_}={self.item_} desc={desc}")
        #work_file = f"liten_work_{self.item_}.py"
        #    %logstop
        #    %logstart -o -r -t -q f"{self.work_file}"
        return

    def stop(self):
        """ 
        End current interactive work 
        """
        if (False == self.active_):
            return "Work not yet started!"
        print(f"Stopped {self.stop_marker_}={self.item_}")
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
        Read all works and summarize each of them
        """
        with open(nbname) as datafile:
            data = json.load(datafile)

        liten_work_id=0
        work_id=0
        work_ids=[0]
        work_data={}
        work_code={}

        # format
        # { cell_type: code|markdown, source:[..], outputs:[{name:stdout,text:[liten..]}]} }
        for cell in data["cells"]:
            code =[]
            code_cell_type = (cell['cell_type'] == 'code')
            
            # work can change only across cells, see if this cell changes work
            # if work_start called in the cell it will add this cell in new work
            for key, value in cell.items():
                if (key == "outputs"):
                    for output in value:
                        if ("text" in output.keys()):
                            for text in output["text"]:
                                if (self.stop_marker_ in text):
                                    liten_work_id=work_id
                                    work_id=0
                                if (self.start_marker_ in text):
                                    m = re.search(f"{self.start_marker_}=([0-9]+)", text)
                                    if (m.groups()):
                                        work_id = int(m.group(1))
                                    else:
                                        work_id = 0
                                        print("Error extracting work id from liten start marker")
                if (code_cell_type and key == "source"):
                    code.append(value)
            # Extract code in work_code work_id
            if work_id not in work_code.keys():
                work_code[work_id] = []
            for instrs in code:
                for instr in instrs:
                    work_code[work_id].append(instr)
            # Append to work data
            if work_id not in work_data.keys():
                work_data[work_id] = []
            if not self.if_image_cell(cell):
                work_data[work_id].append(cell)
            
        # Replace self code & data
        self.code_ = work_code
        self.data_ = work_data
        #print(f"Code={self.code_}")        
        return

    def reduce_prompt_size(self, prompt):
        """
        remove image etc to reduce prompt size
        """
        return self.openai_.reduce_prompt_size(prompt, self.openai_.max_input_tokens)
        
    def summarize(self):
        for k,v in self.data_.items():
            prompt = "Following are cells from a python notebook from jupyter in json format. Please summarize what it is trying to do?\n"
            prompt += json.dumps(v)
            rprompt = self.reduce_prompt_size(prompt)
            self.summary_[k] = self.openai_.summarize(rprompt).strip()
            print(f"Work {k}: {self.summary_[k]}\n")
        return
        
    def explain(self, id):
        """
        Explain a work id
        """
        if (id not in self.data_):
            print(f"Work {id}: does not exist")
            return
        
        prompt = self.reduce_prompt_size(json.dumps(self.data_[id]))
        msg = [
            {"role": "system", "content" : "Following are cells of a python notebook from jupyter is json format. Can you explain what it is trying to do?"},
            {"role": "user", "content" : prompt}
        ]
        s = self.openai_.complete_chat(msg)
        print(f"Work {id}: {s.strip()}\n")
        return

    def replay(self, id):
        """
        Replay the queries of a work
        """
        if (id not in self.code_):
            print(f"Work {id}: does not exist")
        utils.create_new_cell(self.code_[id])
        return

    def find_similar(self, prompt):
        syscontent = "Find a list of works below. Each work starts with work keyword followed by a number. It is followed by a short summary.\n"
        for k,v in self.summary_.items():
            syscontent += f"Work {k} = {v}\n"
        rsyscontent = self.reduce_prompt_size(syscontent)
        chatprompt = "List the work closest to the following work summary.\n"
        chatprompt += prompt
        rchatprompt = self.reduce_prompt_size(chatprompt)
        msg = [
            {"role": "system", "content" : rsyscontent},
            {"role": "user", "content" : rchatprompt}
        ]        
        answer = self.openai_.complete_chat(msg).strip()
        print(answer)
        return
        
    def complete_chat(self, prompt):
        return self.openai_.complete_chat(prompt)

    def generate_sql(self, prompt):
        return self.openai_.generate_sql(prompt)
