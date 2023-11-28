"""
Flask routes for litenDB API
To start the Flask server do the following -
flask --app=litendbapp.py run
See the docstring below for the api routes. These Restful calls can be made from the browser or from a client.
"""

import sys
import json
import litendb as tendb
import os

from flask import Flask
from flask import request

app = Flask(__name__)
service = None
tcache = None


@app.route("/v1/status")
def status():
    """
    Send a current status for litendb service
    Example:
    http://localhost:5560/status
    """
    status = ""
    if not service:
        return f"Service did not start", 300
    if not cache:
        return f"Liten Cache did not start", 300
    return f"OK", 200

@app.route("/v1/version")
def version():
    """
    Send the version for litendb service
    Example:
    http://localhost:5560/version
    """
    return cache.version, 200


    @property
    def FactTable(self):
        return Cache.tcache.FactTable

    @property
    def DimensionTable(self):
        return Cache.tcache.DimensionTable
    
    @property
    def DimensionField(self):
        return Cache.tcache.DimensionField

    @property
    def MetricField(self):
        return Cache.tcache.MetricField

    @property
    def FeatureField(self):
        return Cache.tcache.FeatureField

    @property
    def EmbeddingField(self):
        return Cache.tcache.EmbeddingField

    def info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.info()

    def compute_info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.compute_info()

    def table_info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.table_info()

    def get_table_pyarrow(self, name):
        """
        Returns
          Arrow table
        """
        return Cache.tcache.get_table_pyarrow(name)

    def get_table_type(self, name):
        """
        Returns
          Dimension or Fact Table
        """
        return Cache.tcache.get_table_type(name)

    def schema_info(self):
        """
        return cache information including compute and storage 
        Returns
          string containing cache information
        """
        return Cache.tcache.schema_info()
    
    def get_schema_info(self, name):
        """
        return schema information
        Parameter
          name schema name
        Returns
          string containing schema information
        """
        return Cache.tcache.get_schema_info(name)

    def add_schema(self, name, ttype, pa_schema):
        """
        Add arrow table in cache by name
        Parameters
           name: name of schema
           ttype: type of table must be DimensionTable or FactTable
           schema: arrow schema to be added in liten cache
        Returns
           schema name or exception if failed to add
        """
        return Cache.tcache.add_schema(name, ttype, pa_schema)
    
    def if_valid_schema(self, name):
        """
        Is a valid schema by name
        Parameters
           name: name of schema
        Returns
           True if valid, else false
        """
        return Cache.tcache.if_valid_schema(name)

    def add_schema_from_ttable(self, table):
        """
        Add schema associated with ttable by name. If a schema exists by name, that is returned
        Parameters
           ttable: schema in liten table ttable
        Returns
           schema name
        """
        return Cache.tcache.add_schema_from_ttable(table.ttable)

    def get_schema_pyarrow(self, name):
        """
        Get pyarrow schema from Liten schema
        """
        return self.tcache.get_schema_pyarrow(name)
    
    def get_schema_type(self, name):
        """
        Returns
          Dimension or Fact Table
        """
        return self.tcache.get_schema_type(name)

    def get_schema_field_type(self, name, field_name):
        """
        Get field type for field_name
        Parameters
          field_name name of field
        Returns
          Dimension or Metric or Feature or Embedding Field types. None if failed to get it.
        """
        return self.tcache.get_schema_field_type(name, field_name)

    def set_schema_field_type(self, schema_name, field_name, field_type):
        """
        Get field type for field_name
        Parameters
          schema_name
          field_names name of field can be list or a value
          field_types can be list or value, Dimension or Metric or Feature or Embedding Field types
        Returns
          True if set else False or exceptions
        """
        return Cache.tcache.set_schema_field_type(schema_name, field_name, field_type)
    
    def add_table(self, name, pa_table, ttype, schema_name=""):
        """
        Create arrow table in cache by name
        Parameters
           name: name of table
           table: arrow table to be added in liten cache
           ttype: type of table must be DimensionTable or FactTable
        Returns
           table name or exception if not added
        """
        return Cache.tcache.add_table(name, pa_table, ttype, schema_name)

    def if_valid_table(self, name):
        """
        if table by name exists
        Parameters
           name: name of table
        Returns
           True if exists else False
        """
        return Cache.tcache.if_valid_table(name)
        
    def make_tensor_table(self, name):
        """
        Create data-tensor for name table
        Parameters
           name: Name of table 
        Returns
           true if create successfully else false
        """
        map_result = Cache.tcache.make_maps(name, False)
        ten_result = Cache.tcache.make_tensor(name)
        return map_result or ten_result
    
    def make_tensor(self):
        """
        Create n-dimensional data tensor for all n dimension tables in cache
        Returns
           true if create successfully else false
        """
        map_result = Cache.tcache.make_maps(False)
        ten_result = Cache.tcache.make_tensor()
        return map_result or ten_result
    
    def make_maps_table(self, name, if_reverse_map):
        """
        Create data-tensor for name table
        Parameters
           name: Name of table 
        Returns
           true if create successfully else false
        """
        return Cache.tcache.make_maps_table(name, if_reverse_map)
    
    def make_maps(self, if_reverse_map):
        """
        Create n-dimensional data tensor for all n dimension tables in cache
        Returns
           true if create successfully else false
        """
        return Cache.tcache.make_maps(if_reverse_map)
    
    def query6(self):
        """
        Run Tpch query 6
        Returns
           query 6 result
        """
        return Cache.tcache.query6()

    def query5(self):
        """
        Run Tpch query 5
        Returns
           query 5 result
        """
        return Cache.tcache.query5()

    def join(self, child_schema_name, child_field_name, parent_schema_name, parent_field_name):
        """
        joints child field with parent field which creates data tensor dimensionality
        Parameters
           child_schema name of child schema
           child_field_name name of child field
           parent_schema TSchema of parent
           parent_field_name name of parent field
        Returns
           True if success else False
        """
        return Cache.tcache.join(child_schema_name, child_field_name, parent_schema_name, parent_field_name)
        
    def slice(self, table_name, offset, length):
        """
        Parameters
          table_name: name of table
          offset: offset from beginning
          length: number for rows to be sliced
        Returns:
          arrow table with the given slice, None if table not found
        """
        return Cache.tcache.slice(table_name, offset, length)

    def read_csv(self, input_file, parse_options, table_name, ttype, schema_name=""):
        """
        read csv file input_file using pyarrow reader. Add it as a table_name in Liten.
        Create arrow table in cache by name
        Parameters
           input_file: Name of input_file to be passed to arrow
           parse_options: parse_options for arrow reader
           table_name: name of table
           ttype: type of table must be DimensionTable or FactTable
           schema_name: if schema already exists
        Returns
           Added table_name
        """
        pa_table = pa.csv.read_csv(input_file=input_file, parse_options=parse_options)
        ttable = Cache.tcache.add_table(table_name, pa_table, ttype, schema_name)
        return table_name

## TBD

@app.route("/v1/tpchquery6")
def append_user_message(session_name, prompt):
    """
    get a session with the given session_name
    If creating use the config file_name
    """
    session = None
    try:
        session = tenai.Session.get(session_name)
    except Exception as exc:
        error_message = f"Failed to get session {session_name}. Exception={exc}"
        return error_message, 406
    if not session:
        return f"Failed to get session {session_name}", 406
    try:
        session.context.user(prompt)
    except Exception as exc:
        error_message = f"Failed to append user prompt {prompt}. Exception={exc}"
        return error_message, 500
    return "Added user prompt successfully", 200


@app.route("/ask/<session_name>/<prompt>")
def ask_liten(session_name, prompt):
    """
    Ask to complete prompt for session_name
    Example:
    http://127.0.0.1:5000/ask/test/%22What%20are%20status%20code%20errors%22
    """
    session = None
    try:
        session = tenai.Session.get(session_name)
    except Exception as exc:
        error_message = f"Failed to get session {session_name}. Exception={exc}"
        return error_message, 406
    if not session:
        return f"Failed to get session {session_name}", 406
    chat_response = ""
    try:
        chat_response= session._openai.complete_prompt_chat(prompt)
    except Exception as exc:
        error_message = f"Failed ask for user prompt {prompt}. Exception={exc}"
        return error_message, 500
    return chat_response, 200

@app.route("/send/<session_name>/<prompt>")
def send_liten(session_name, prompt):
    """
    Send to complete prompt for session_name. Master agent identifies the action from the prompt and completes it using the appropriate agent.
    Example:
    http://127.0.0.1:5000/send/xxx/%22Generate%20sql%20for%20the%20following.%20Select%20top%20100%20rows.%22
    """
    session = None
    try:
        session = tenai.Session.get(session_name)
    except Exception as exc:
        error_message = f"Failed to get session {session_name}. Exception={exc}"
        return error_message, 406
    if not session:
        return f"Failed to get session {session_name}", 406    
    resp = session.get_response_for_send(prompt)
    if resp.derr:
        return f"Failed to send user prompt with error {resp.derr}", 406
    resp_str = ""
    if resp.dout:
        resp_str = resp.dout + "\n"
    resp_str = resp_str + resp.d
    return resp_str, 200

if __name__ == '__main__':
    HOST = os.environ.get('LITEN_SERVER_HOST','localhost')
    PORT = 5560
    try:
        PORT = int(os.environ.get('LITENDB_SERVER_PORT','5560'))
    except ValueError:
        PORT = 5560
    try:
        service = tendb.Service()
        service.start()
        cache = tendb.Cache()
        app.run(HOST, PORT)
    except Exception:
        service = None
        cache = None
    if service:
        service.stop()