import os
import sys
import shutil
import subprocess
import colorama
from colorama import Fore, Style

colorama.init(strip=False)


def make_clean_dir(path):
    r"""Creates a directory if it does not exist yet. If exists deletes it and creates again.
        Args:
            path : directory name
        """
    if os.path.isdir(path):
        shutil.rmtree(path)
    os.mkdir(path)


def make_dir(path):
    r"""Creates a directory if it does not exist yet. If exists nothing is done.
        Args:
            path : directory name
        """
    if not os.path.isdir(path):
        os.mkdir(path)


def error_print(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def execute(cmd, quiet=False, working_dir='.'):
    r"""Executes a command in shell.
        Args:
            cmd : list of arguments
            quiet : if false, won't print to the console any of the output from the commend being executed.
                    Default: False.
            working_dir : working directory. Default : current working directory
        """
    console_out = None
    console_error = None
    if quiet:
        console_out = subprocess.PIPE
        console_error = subprocess.STDOUT
    process = subprocess.Popen(cmd, stdout=console_out, stderr=console_error, cwd=working_dir)
    process.communicate()
    if process.wait() != 0:
        error_print(Style.BRIGHT + Fore.RED + 'Error occurred while executing following command:' + Style.RESET_ALL)
        for argument in cmd:
            sys.stderr.write(argument + " ")
        sys.stderr.flush()
        sys.exit()
