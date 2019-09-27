project('rsh', 'c')

readline = meson.get_compiler('c').find_library('libreadline', dirs: ['/usr/local/opt/readline/lib'], required: false)

if not readline.found()
    readline = meson.get_compiler('c').find_library('libreadline')
endif

inc = include_directories('include')

sources = ['src/main.c', 'src/parse.c', 'src/external.c', 'src/interpret.c', 'src/prompt.c', 'src/util.c', 'src/builtin.c', 'src/pinfo.c', 'src/nightswatch.c', 'src/history.c', 'src/ls.c', 'src/env.c', 'src/kill.c', 'src/cronjob.c']

executable('rsh', sources, include_directories: inc, dependencies: readline)
