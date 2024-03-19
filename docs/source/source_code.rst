=================================================
Source Code!
=================================================

for gods sake trigger the docs build

Embedded LaTeX Equations
-------------------------------------------------
Here's some LaTeX inside the reStructured-Text:

.. math::

   \frac{ \sum_{t=0}^{N}f(t,k) }{N}

Data Structure Reference
-------------------------------------------------
Here's an auto-gen blurb on the Task Control Block (TCB) data structure:

.. doxygenstruct:: T_TaskControlBlock
   :project: jocktos-docs
   :members:

Enum Reference
-------------------------------------------------
Here's an auto-gen blurb on the task state enumeration:

.. doxygenenum:: E_TaskState
   :project: jocktos-docs

.. admonition:: This was all in the header file!

    The figure and LaTeX can both be found in jocktos/inc/os.h
