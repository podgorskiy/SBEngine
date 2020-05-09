import subprocess
from os import listdir
from os.path import isdir, isfile, join


class PatchingError(RuntimeError):
    pass


def get_patches(path):
    libs_to_patch = [f for f in listdir(path) if isdir(join(path, f))]

    for lib in libs_to_patch:
        patches = [f for f in listdir(join(path, lib)) if isfile(join(path, lib, f)) and f.endswith('.patch')]
        for patch in patches:
            yield lib, patch


def patch_lib(lib_name, patch_filename):
    patch_filename = join('patches', lib_name, patch_filename)
    lib_name = join('libs', lib_name)
    try:
        subprocess.check_output(
            ['git', 'apply', "-v", '--check', '--whitespace=fix', '--directory', lib_name, patch_filename])
        subprocess.check_output(
            ['git', 'apply', '--whitespace=fix', '--directory', lib_name, patch_filename])
    except subprocess.CalledProcessError as error:
        raise PatchingError("\n\nCouldn't patch lib %s with patch %s" % (lib_name, patch_filename)) from error


def revert_lib(lib_name):
    lib_name = join('libs', lib_name)
    try:
        subprocess.check_output(
            ['git', 'submodule', 'deinit', '-f', lib_name])
        subprocess.check_output(
            ['git', 'submodule', 'update', '--init', lib_name])
    except subprocess.CalledProcessError as error:
        raise PatchingError("\n\nCouldn't revert lib %s" % lib_name) from error
