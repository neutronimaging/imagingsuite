import subprocess
import json
import sys
import argparse
import copy

from batch_runner import load_batch

def build_batch(template,step,first,last,output):
    '''Builds a batch file from a template file and a range of projections.
    
    Arguments:
        template - a json file with batch descriptions.
        step     - block step.
        first    - first projection.
        last     - last projection.
        output   - output file name.
    '''
    
    templ = load_batch(template)[0]

    data = []
    for idx in range(int(first),int(last)+1,int(step)):
        frame = (idx-int(first))//int(step)
        print(f"Processing frame {idx} - {frame}")
        task = copy.deepcopy(templ)
        task['name'] = f"frame_{frame:04d}"
        task['arguments']['projections:firstindex'] = idx
        task['arguments']['projections:lastindex']  = idx+int(step)-1
        task['arguments']['matrix:path'] = task['arguments']['matrix:path']+f"/frame_{frame:04d}"

        parts = task['arguments']['matrix:matrixname'].split('_')
        if len(parts) > 2:
            mname = ''.join(parts[:-2])+f"_frame{frame:02d}"+'_'+parts[-1]
        else:
            mname = parts[0]+f"_frame{frame:02d}"+'_'+parts[-1]
        task['arguments']['matrix:matrixname'] = mname
        data.append(task)


    with open(output, 'w') as f:
        json.dump(data, f, indent=4)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Runs a set of reconstruction batches using MuhRec.")
    parser.add_argument('-t','--template', help="A json file with batch descriptions.")
    parser.add_argument('-s','--step', help="Block step.")
    parser.add_argument('-f','--first', help="First projection.")
    parser.add_argument('-l','--last', help="Last projection.")
    parser.add_argument('-o','--output', help="Output file name.")

    args = parser.parse_args()

    build_batch(template=args.template,step=args.step,first=args.first,last=args.last,output=args.output)

