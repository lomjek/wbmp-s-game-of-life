env = Environment()

if ARGUMENTS.get('debug'):
    env.Append(CCFLAGS=['-g'])

env.Append(CPPPATH=['#']) 
env.Program('game_of_life', Glob('src/*.c'))
