import colorama
import time
import atexit
from colorama import Fore, Style

colorama.init(strip=False)


def at_exit(message):
    r"""Prints elapsed time to execute the script.
        Add this in the beginning of the script before any work is done.
        .. note::
            You need to have colorama installed
        Args:
            message (str): Message to print at exit
        Examples::
            >>> def main():
            >>> 	at_exit("Setup time:");
            >>>     do_something()
            >>>
            >>> if __name__ == "__main__":
            >>>     main()
        .. _link:
            https://pypi.org/project/colorama/
        """

    time_start = time.time()
    atexit.register(__print_execution_time, time_start, message)


def __print_execution_time(start_time, message):
    time_delta = time.gmtime(time.time() - start_time)
    print("\n" + Style.BRIGHT + Fore.GREEN + time.strftime(message + " %M minutes %S seconds", time_delta) + Style.RESET_ALL)
