from subprocess import call
import json
import sys

def load_batch(batch_file):
    """Load batch from json formatted file and return a dictionary"""
    with open(batch_file, 'r') as f:
        data = json.load(f)
    return data

def run_batch(batch_file,muhrec_path):

    for task in load_batch(batch_file):
        print("Processing project {0}".format(task['name']))

        call_info = [muhrec_path, "-f", task['config']]
        for arg in task['arguments']:
            # print("Adding argument {0} with value {1}".format(arg,task['arguments'][arg]))
            call_info.append(arg+"="+str(task['arguments'][arg]))
        
        # print("Calling muhrec with arguments: {0}".format(call_info))
        #call([muhrec_path, "-f", tasks['cfgpath'], tasks['firstindex'], tasks['lastindex'], tasks['matrixname'], tasks['scanarc']])  

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python batch_runner.py batch_file.json muhrec_path")
        sys.exit(1)
    
    batch_file = sys.argv[1]
    muhrec_path = sys.argv[2]

    run_batch(batch_file=batch_file,muhrec_path=muhrec_path)

