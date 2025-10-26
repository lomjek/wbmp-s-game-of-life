env = Environment()

target = ARGUMENTS.get('target', 'debug').lower()

if target == 'debug':
    print("Building DEBUG configuration...")
    env.Append(
        CCFLAGS = ['-g', '-O0'],
        CPPDEFINES = ['DEBUG']
    )

elif target == 'release':
    print("Building RELEASE configuration...")
    env.Append(
        CCFLAGS = ['-Oz', '-DNDEBUG', '-Wextra', '-Wall'],
        CPPDEFINES = ['RELEASE']
    )

else:
    print(f"Error: Unknown target '{target}'. Use 'debug' or 'release'.")
    Exit(1)


env.Append(
    CPPPATH=['#'],
    LIBS=['m']
) 
env.Program('game_of_life', Glob('src/*.c'))
