NXEXTRACT v1.12.0
-----------------

LOOP operator:

1. A few math capabilities were added in loop declaration. It's now possible to use
basic arythmetic operators, like:
@( i = $(param) + 1, nxs:/entry1/foo/bar - 1
...
@)

Note: the spaces before and after math operators are mandatory. This is incorrect:
@( i = $(param)+ 1, nxs:/entry1/foo/bar-1

2. Step parameter 
An optionnal third parameter may be used to specify step value, e.g.:
@( i = 0, nxs:/entry1/foo/bar - 1, 4
NXEXTRACT v1.12.0
-----------------

LOOP operator:

1. A few math capabilities were added in loop declaration. It's now possible to use
basic arythmetic operators, like:
@( i = $(param) + 1, nxs:/entry1/foo/bar - 1
...
@)

Note: the spaces before and after math operators are mandatory. This is incorrect:
@( i = $(param)+ 1, nxs:/entry1/foo/bar-1

2. Step parameter 
An optionnal third parameter may be used to specify step value, e.g.:
@( i = 0, nxs:/entry1/foo/bar - 1, 4
