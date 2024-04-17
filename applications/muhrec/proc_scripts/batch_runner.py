from subprocess import call
import json
import sys
import argparse

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
            call_info.append(arg+"="+str(task['arguments'][arg]))
 
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Runs a set of reconstruction batches using MuhRec.")
    parser.add_argument('-b','--batchfile', help="A json file with batch descriptions.")
    parser.add_argument('-m','--muhrecpath', help="Path to the muhrec executable.")

    args = parser.parse_args()

    run_batch(batch_file=args.batchfile,muhrec_path=args.muhrecpath)

