from collections import Counter
import time
import requests
def dtw(my_pattern, template_pattern, start=0):
    d = [[2147483647 for _ in range(50)] for __ in range(2)]
    cur = 0
    d[1][0] = abs(my_pattern[start] - template_pattern[0])
    for i in range(1, 50):
        d[1][i] = abs(my_pattern[start] - template_pattern[i]) + d[1][i-1]
    for i in range(start+1, start+50):
        for j in range(50):
            d[cur][j] = abs(my_pattern[i%50] - template_pattern[j])
            if j > 0:
                d[cur][j] += min(d[cur][j-1], d[(cur+1)%2][j], d[(cur+1)%2][j-1])
            else:
                d[cur][j] += d[(cur+1)%2][j]
        cur = (cur + 1) % 2
    return d[(cur+1)%2][49]

def voting(query_pattern, model_patterns):
    score = []
    for model, m_patterns in model_patterns.items():
        for m_p in m_patterns:
            score.append((model, dtw(query_pattern, m_p)))
    score = sorted(score, key=lambda x: x[1])
    decision = max(Counter([x[0] for x in score[:10]]).items(), key=lambda x: x[1])[0]
    return decision




def load_pattern():
    with open('patterns.csv', 'r') as fp:
        fp.readline()
        tmp = list(zip(*[[int(x) for x in line.strip().split(',')] for line in fp]))
    patterns = dict()
    patterns['angry'] = tmp[:10]
    patterns['walk'] = tmp[10:20]
    patterns['stay'] = tmp[20:30]
    return patterns

def dump_putty_log():
    out = False
    while not out:
        with open('action.log', 'rb') as fp:
            raw_data = fp.read().split(b'\n')
            data = []
            for x in dump_putty_log.preprocess(raw_data):
                try:
                    data.append(int(x.strip()))
                except:
                    continue
        new_lines = len(data) - dump_putty_log.pointer
        if new_lines > 0:
            out = True
            dump_putty_log.pointer = len(data)
        time.sleep(0.5)
    return data, new_lines

dump_putty_log.pointer = 0
dump_putty_log.preprocess = lambda x: x[5:]

def main():
    patterns = load_pattern()
    state = 'startup'
    while(True):
        data, new_ins_count = dump_putty_log()
        print(data, new_ins_count)

        r = requests.get('http://linux7.csie.ntu.edu.tw:8000', params={'acc_x': data[new_ins_count:]})

if __name__ == '__main__':
    main()
