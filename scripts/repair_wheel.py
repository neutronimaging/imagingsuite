"""Cross-platform wheel repair script that locates Conan dependency libraries
and passes their directories to the platform-specific repair tool.

Usage:
    python repair_wheel.py <wheel> <dest_dir> linux
    python repair_wheel.py <wheel> <dest_dir> macos [<delocate_archs>]
    python repair_wheel.py <wheel> <dest_dir> windows
"""

import os
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
import zipfile


def find_conan_lib_dirs(patterns):
    """Find all directories in the Conan cache containing files matching the given glob patterns."""
    conan_home = pathlib.Path(os.path.expanduser("~/.conan2/p"))
    dirs = set()
    if conan_home.exists():
        for pattern in patterns:
            for f in conan_home.rglob(pattern):
                if f.is_file():
                    dirs.add(str(f.parent))
    return dirs


def repair_linux(wheel, dest_dir):
    # Set LD_LIBRARY_PATH so auditwheel can find Conan shared libraries
    lib_dirs = find_conan_lib_dirs(["*.so", "*.so.*"])
    env = os.environ.copy()
    existing = env.get("LD_LIBRARY_PATH", "")
    env["LD_LIBRARY_PATH"] = ":".join(lib_dirs) + (":" + existing if existing else "")
    print(f"LD_LIBRARY_PATH set to {len(lib_dirs)} Conan directories", flush=True)
    subprocess.run(
        ["auditwheel", "repair", "-w", dest_dir, wheel],
        env=env,
        check=True,
    )


def repair_macos(wheel, dest_dir, delocate_archs):
    """Custom macOS repair: delocate fails with duplicate basenames from multiple
    RPATH entries, so we manually bundle Conan dylibs and fix install names."""

    conan_lib_dirs = find_conan_lib_dirs(["*.dylib"])
    # Build a lookup: basename -> full path (first match wins)
    conan_libs = {}
    for d in conan_lib_dirs:
        for f in os.listdir(d):
            if ".dylib" in f and f not in conan_libs:
                conan_libs[f] = os.path.join(d, f)

    print(f"Found {len(conan_libs)} unique Conan dylibs across {len(conan_lib_dirs)} directories", flush=True)

    with tempfile.TemporaryDirectory() as tmp:
        extract_dir = os.path.join(tmp, "wheel")
        os.makedirs(extract_dir)

        with zipfile.ZipFile(wheel) as zf:
            zf.extractall(extract_dir)

        # Find the package directory containing .so/.dylib files
        pkg_dir = None
        for root, dirs, files in os.walk(extract_dir):
            if any(f.endswith(".so") or ".cpython-" in f for f in files):
                pkg_dir = root
                break

        if not pkg_dir:
            print("No binaries found in wheel, copying as-is", flush=True)
            shutil.copy2(wheel, os.path.join(dest_dir, os.path.basename(wheel)))
            return

        # Recursively find and copy all needed Conan dylibs
        copied = set()
        to_process = [
            os.path.join(pkg_dir, f)
            for f in os.listdir(pkg_dir)
            if f.endswith(".so") or f.endswith(".dylib") or ".cpython-" in f
        ]

        while to_process:
            binary = to_process.pop()
            if binary in copied:
                continue
            copied.add(binary)

            result = subprocess.run(
                ["otool", "-L", binary], capture_output=True, text=True
            )
            for line in result.stdout.strip().split("\n")[1:]:
                line = line.strip()
                if not line:
                    continue
                dep_path = line.split()[0]
                if not dep_path.startswith("@rpath/"):
                    continue
                basename = os.path.basename(dep_path)
                dest_path = os.path.join(pkg_dir, basename)

                if os.path.exists(dest_path):
                    # Already copied or is our own lib
                    if dest_path not in copied:
                        to_process.append(dest_path)
                    continue

                # Find in Conan libs
                if basename in conan_libs:
                    shutil.copy2(conan_libs[basename], dest_path)
                    os.chmod(dest_path, 0o755)
                    print(f"  Bundled: {basename}", flush=True)
                    to_process.append(dest_path)
                else:
                    print(f"  WARNING: Could not find {basename}", flush=True)

        # Fix install names: @rpath/X.dylib -> @loader_path/X.dylib
        all_binaries = [
            os.path.join(pkg_dir, f)
            for f in os.listdir(pkg_dir)
            if f.endswith(".so") or f.endswith(".dylib") or ".cpython-" in f
        ]

        for binary in all_binaries:
            # Fix dependencies
            result = subprocess.run(
                ["otool", "-L", binary], capture_output=True, text=True
            )
            for line in result.stdout.strip().split("\n")[1:]:
                line = line.strip()
                if not line:
                    continue
                dep_path = line.split()[0]
                if dep_path.startswith("@rpath/"):
                    basename = os.path.basename(dep_path)
                    new_path = f"@loader_path/{basename}"
                    subprocess.run(
                        ["install_name_tool", "-change", dep_path, new_path, binary],
                        capture_output=True,
                    )

            # Fix library ID
            result = subprocess.run(
                ["otool", "-D", binary], capture_output=True, text=True
            )
            lines = result.stdout.strip().split("\n")
            if len(lines) >= 2:
                current_id = lines[-1].strip()
                if current_id.startswith("@rpath/") or "/" in current_id:
                    basename = os.path.basename(current_id)
                    subprocess.run(
                        [
                            "install_name_tool",
                            "-id",
                            f"@loader_path/{basename}",
                            binary,
                        ],
                        capture_output=True,
                    )

            # Remove Conan RPATH entries
            result = subprocess.run(
                ["otool", "-l", binary], capture_output=True, text=True
            )
            for match in re.finditer(
                r"path\s+(.+?)\s+\(offset", result.stdout
            ):
                rpath = match.group(1)
                if ".conan2" in rpath or (
                    rpath.startswith("/") and rpath != "@loader_path"
                ):
                    subprocess.run(
                        ["install_name_tool", "-delete_rpath", rpath, binary],
                        capture_output=True,
                    )

        # Repack the wheel
        subprocess.run(
            [sys.executable, "-m", "wheel", "pack", extract_dir, "-d", dest_dir],
            check=True,
        )

    print("macOS wheel repair complete", flush=True)


def repair_windows(wheel, dest_dir):
    dll_dirs = find_conan_lib_dirs(["*.dll"])

    # Also extract the wheel so delvewheel can find our project DLLs
    # (kipl.dll etc.) that are already in the wheel
    with tempfile.TemporaryDirectory() as tmp:
        with zipfile.ZipFile(wheel) as zf:
            zf.extractall(tmp)

        wheel_dll_dirs = set()
        for root, _dirs, files in os.walk(tmp):
            if any(f.endswith((".dll", ".pyd")) for f in files):
                wheel_dll_dirs.add(root)

        all_dirs = dll_dirs | wheel_dll_dirs
        add_path = ";".join(all_dirs) if all_dirs else ""
        print(
            f"delvewheel --add-path: {len(dll_dirs)} Conan + {len(wheel_dll_dirs)} wheel directories",
            flush=True,
        )
        cmd = ["delvewheel", "repair", "-w", dest_dir, wheel]
        if add_path:
            cmd.extend(["--add-path", add_path])
        subprocess.run(cmd, check=True)


if __name__ == "__main__":
    wheel = sys.argv[1]
    dest_dir = sys.argv[2]
    platform = sys.argv[3]

    if platform == "linux":
        repair_linux(wheel, dest_dir)
    elif platform == "macos":
        delocate_archs = sys.argv[4] if len(sys.argv) > 4 else ""
        repair_macos(wheel, dest_dir, delocate_archs)
    elif platform == "windows":
        repair_windows(wheel, dest_dir)
    else:
        print(f"Unknown platform: {platform}", file=sys.stderr)
        sys.exit(1)
