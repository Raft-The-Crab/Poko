#!/bin/bash
# Script to generate Doxygen documentation for Poko project

echo "Generating Poko documentation..."
cd "$(dirname "$0")/.."
doxygen Doxyfile

if [ $? -eq 0 ]; then
    echo "Documentation generated successfully in docs/generated/html/"
    echo "Open docs/generated/html/index.html in your browser to view the documentation"
else
    echo "Documentation generation failed"
    exit 1
fi