# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import subprocess

subprocess.call('doxygen /JOCKTOS/docs/Doxyfile', shell=True)

# Add Doxygen-generated files to Sphinx
breathe_default_project = 'jocktos-docs'
breathe_projects = {
    'jocktos-docs': "/JOCKTOS/docs/build/xml",
}

latex_engine = 'xelatex'  # or 'pdflatex' if you prefer
latex_build_directory = '/JOCKTOS/docs/_latex'
latex_elements = {
    'papersize': 'letterpaper',
    'pointsize': '10pt',
    'preamble': '',
    'figure_align': 'htbp',
}
# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'jocktos-docs'
copyright = '2024, Nick Schneider, Joshua Goard'
author = 'Nick Schneider, Joshua Goard'
release = 'v0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'breathe'
]

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
