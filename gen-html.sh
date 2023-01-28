#!/bin/bash
cd "$(dirname "$0")"
pandoc -s --template template.html --metadata title="GSHAX Tool: Create PSX GameShark Codes That Gain Code Execution Mid-Game
" readme.md -o readme.html
