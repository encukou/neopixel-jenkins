
"""
Usage:
    feed-data.py JENKINS_URL CONSOLE [JOB_PREFIX] [INTERVAL]

JENKINS_URL: Jenkins URL, e.g. jenkins.example.com:8080
CONSOLE: The serial sonsole file, e.g. dev/tty/ACM0
JOB_PREFIX: Prefix for filtering jobs
INTERVAL: seconds between updates
"""

import time

import docopt
import serial
from jenkins import Jenkins

def clamp(mini, val, maxi):
    if val < mini:
        return mini
    if val > maxi:
        return maxi
    return val

def main(jenkins, out_file, prefix, interval):
    while True:
        jobs = jenkins.get_jobs()
        last_letter = None
        for job in jobs:
            if job['name'].startswith(prefix):
                info = jenkins.get_job_info(job['name'])
                health = None
                healths = [h['score'] for h in info['healthReport']]
                if healths:
                    health = sum(healths) / len(healths)
                else:
                    health = 1
                color, sep, ani = info['color'].lower().partition('_')
                if color in ('red', 'green', 'yellow'):
                    letter = color[0]
                elif color == 'blue':
                    letter = 'g'
                else:
                    letter = 'x'
                if ani:
                    letter = letter.upper()
                number = '9876543210'[clamp(0, health // 10, 9)]
                print letter, number, info['name'], info['color'], healths
                out_file.write(letter)
                out_file.write(number)
        print
        out_file.write('\n')
        if not interval:
            return
        else:
            time.sleep(interval)


if __name__ == '__main__':
    opts = docopt.docopt(__doc__)
    jenkins = Jenkins(opts['JENKINS_URL'])
    interval = int(opts['INTERVAL'] or 60)
    with serial.Serial(opts['CONSOLE'], 9600, timeout=1) as console:
        main(jenkins, console, opts['JOB_PREFIX'] or '', interval)
