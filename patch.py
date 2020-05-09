import os
import subprocess


def main():
    patch_list = []
    for root, dirs, files in os.walk("patches"):
        for f in files:
            patch_list.append((os.path.splitext(os.path.basename(f))[0], f))
    run(patch_list)


def run(patches_list):
    for directory, patch_path in patches_list:
        patch(directory, patch_path)


def patch(directory, patch_path):
    patch_path = os.path.join('patches', patch_path)
    directory = os.path.join('libs', directory)
    try:
        subprocess.check_output(
            ['git', 'submodule', 'deinit', '-f', directory])
        subprocess.check_output(
            ['git', 'submodule', 'update', '--init', directory])
        subprocess.check_output(
            ['git', 'apply', "-v", '--check', '--whitespace=fix', '--directory', directory, patch_path])
        subprocess.check_output(
            ['git', 'apply', '--whitespace=fix', '--directory', directory, patch_path])
    except:
        print("\n\nFailed to apply patch {0}".format(patch_path))


if __name__ == "__main__":
    main()
