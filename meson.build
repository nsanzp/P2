project('Práctica 2 de PAV - detección de actividad vocal', 'c')

exe = 'vad'
src = ['src/main_vad.c', 'src/vad.c', 'src/pav_analysis.c']
lib = ['-lm', '-lsndfile']

inputDO = files('src/vad.docopt')
progDO = files('docopt_c/docopt_c.py')
outputDO = 'vad_docopt.h'

headerDO = custom_target('Construye vad_docopt.h',
output: outputDO, command: [progDO, inputDO, '-o', outputDO])

executable(exe, sources: [src, headerDO], link_args: lib)