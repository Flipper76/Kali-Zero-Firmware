import argparse
import json
import logging
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

log = logging.getLogger(__name__)

VERSION_FILENAME = "VERSION"


def main():
    logging.basicConfig(
        format="%(asctime)s.%(msecs)03d [%(levelname).1s] %(message)s",
        level=logging.INFO,
        datefmt="%H:%M:%S",
    )

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument(
        "--cube-version",
        "-v",
        help="STM32CubeWB version to use. Must be a tag or branch name",
        default="v1.15.0",
        required=False,
    )
    arg_parser.add_argument(
        "--cube-git-url",
        help="STM32CubeWB git repo path",
        default="https://github.com/STMicroelectronics/STM32CubeWB.git",
        required=False,
    )
    arg_parser.add_argument(
        "--config",
        "-p",
        help="Print paths to used files",
        default=Path(__file__).parent / "config.json",
        required=False,
    )
    arg_parser.add_argument(
        "target_dir",
        help="Output directory",
        default=os.getcwd(),
        nargs="?",
    )
    arg_parser.add_argument(
        "--force",
        "-f",
        help="Skip target directory checks",
        action="store_true",
        default=False,
        required=False,
    )

    args = arg_parser.parse_args()

    target_dir = Path(args.target_dir)
    log.info(f"Target directory: {target_dir}")
    if not (target_dir / VERSION_FILENAME).exists():
        if args.force:
            log.warning("Target directory does not contain a valid checkout")
        else:
            log.error(
                "Target directory does not contain a valid checkout. Use --force to continue"
            )
            return 1

    log.info(f"Loading config file {args.config}")
    with open(args.config, "r") as f:
        path_config = json.load(f)

    log.info(f"Cloning STM32CubeWB, {args.cube_version} from {args.cube_git_url}")
    with tempfile.TemporaryDirectory() as tmpdir:
        subprocess.check_call(
            [
                "git",
                "clone",
                "--depth=1",
                "--branch",
                args.cube_version,
                args.cube_git_url,
                tmpdir,
            ],
        )

        for patch_file in path_config.get("patches", []):
            patch_file_path = Path(args.config).parent / patch_file
            if not patch_file_path.exists():
                log.error(f"Patch {patch_file} not found")
                return 1

            log.info(f"Applying patch {patch_file_path}")
            subprocess.check_call(["git", "apply", patch_file_path], cwd=tmpdir)

        for dest, src in path_config.get("paths", {}).items():
            dest = target_dir / dest
            src = Path(tmpdir) / src
            if not src.exists():
                log.error(f"Source '{src}' not found")
                return 1

            log.info(f"Moving '{src}' to '{dest}'")
            shutil.rmtree(dest, ignore_errors=True)
            os.makedirs(dest.parent, exist_ok=True)
            # shutil.copytree(src, dest)
            shutil.move(src, dest)

        with open(target_dir / VERSION_FILENAME, "wt") as f:
            f.write(args.cube_version)

        log.info("Done")


if __name__ == "__main__":
    sys.exit(main() or 0)
