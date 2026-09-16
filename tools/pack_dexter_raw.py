#!/usr/bin/env python3
"""Compat: redirecciona para pack_chars.py (todos os lutadores)."""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pack_chars import main

if __name__ == "__main__":
    sys.exit(main())
