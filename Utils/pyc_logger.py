"""
This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT>.

Developed by NISLAB - Network and Information Security Laboratory
at George Emil Palade University of Medicine, Pharmacy, Science and
Technology of Târgu Mureş <https://nislab.umfst.ro/>

Contributors: Roland Bolboaca
"""

import logging

import sys
sys.path.insert(0, '/etc/diasfw/')
from config import LOGFILE,LOGLEVEL

levels = {
    'critical': logging.CRITICAL,
    'error': logging.ERROR,
    'warn': logging.WARNING,
    'warning': logging.WARNING,
    'info': logging.INFO,
    'debug': logging.DEBUG
}

loglvl = levels.get(LOGLEVEL.lower())

logger = logging.getLogger(__name__)
logger.setLevel(loglvl)

file = logging.FileHandler(LOGFILE)
file.setLevel(loglvl)
fileformat = logging.Formatter("%(asctime)s - %(message)s")
file.setFormatter(fileformat)

logger.addHandler(file)

stream = logging.StreamHandler()
stream.setLevel(loglvl)
streamformat = logging.Formatter("%(asctime)s - %(message)s")
stream.setFormatter(streamformat)

logger.addHandler(stream)