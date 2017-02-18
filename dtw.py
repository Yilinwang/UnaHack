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
        with open('putty.log', 'rb') as fp:
            try:
                raw_data = dump_putty_log.preprocess(fp.read().split(b'\n'))[dump_putty_log.pointer:]
            except:
                continue
            data = []
            for x in raw_data:
                try:
                    data.append(int(x.strip()))
                except:
                    continue
        if len(data) > 0:
            out = True
            dump_putty_log.pointer += len(data)
        time.sleep(1)
    return data

dump_putty_log.pointer = 0
dump_putty_log.preprocess = lambda x: x[5:]

def is_falling(query_pattern):
    if min(query_pattern) < -200:
        if min(query_pattern[-10:]) > -200 and max(query_pattern[-10:]) < 200:
            return True
    return False
def main():
    patterns = load_pattern()
    state = 'startup'
    pre_data = []
    pre_decision = 'idle'
    count = 0
    while(True):
        data = dump_putty_log()
        #print(data[-new_ins_count:])
        print(len(data))
        if len(data) > 1000:
            continue
        draw_data = pre_data + data
        decision = set([voting(draw_data[i: i+50], patterns) for i in range(0, len(draw_data) - 50, 10)])
        if pre_decision == 'fallen':
            decision = 'fallen'
        elif 'angry' in decision:
            decision = 'angry'
            if pre_decision == 'angry':
                count += 1
            else:
                count = 0
        elif 'walk' in decision:
            decision = 'walk'
            if pre_decision == 'walk':
                count += 1
            else:
                count = 0
        elif 'stay' in decision:
            if pre_decision == 'walk':
                if count <= 2:
                    decision = 'walk'
                    count += 1
                else:
                    count = 0
                    decision = 'stay'
            elif pre_decision == 'stay':
                if count >= 50:
                    decision = 'idle'
                    count = 0
                else:
                    decision = 'stay'
                    count += 1
            elif pre_decision == 'idle':
                decision = 'idle'
            else:
                decision = 'stay'
                count = 0
        if is_falling(draw_data) and pre_decision != 'idle':
            decision = 'fallen'
        pre_data = data
        pre_decision = decision
        print(pre_decision, decision)

        send_decision = decision
        if send_decision == 'walk':
            send_decision = 'walking'
        if send_decision == 'stay':
            send_decision = 'staying'

        r = requests.get('http://linux7.csie.ntu.edu.tw:8000', params={'acc_x': data, 'action': send_decision})

if __name__ == '__main__':
    main()
