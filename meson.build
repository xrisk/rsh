project('rsh', 'c')

inc = include_directories('include')

# sources = ['src/builtin.c', 'src/external.c', 'src/history.c', 'src/interpret.c', 'src/ls.c', 'src/main.c', 'src/nightswatch.c', 'src/parse.c', 'src/pinfo.c', 'src/prompt.c'] 

sources = ['src/main.c', 'src/parse.c', 'src/external.c', 'src/interpret.c', 'src/prompt.c', 'src/util.c', 'src/builtin.c', 'src/pinfo.c', 'src/nightswatch.c', 'src/history.c', 'src/ls.c', 'src/env.c']

executable('rsh', sources, include_directories: inc)
