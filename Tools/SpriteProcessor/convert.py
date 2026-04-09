from pathlib import Path
import sys

from PIL import Image


def convert_to_png(source: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    with Image.open(source) as image:
        image.convert("RGBA").save(destination, "PNG")
    print(f"Converted {source} -> {destination}")


def main() -> int:
    if len(sys.argv) != 3:
        print("Usage: python Tools/SpriteProcessor/convert.py <input> <output>")
        return 1

    source = Path(sys.argv[1])
    destination = Path(sys.argv[2])

    if not source.exists():
        print(f"Input file not found: {source}")
        return 1

    convert_to_png(source, destination)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
