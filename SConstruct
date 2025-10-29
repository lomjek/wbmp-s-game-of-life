env = Environment()

target = ARGUMENTS.get('target', 'debug').lower()

env['CCCOMSTR'] = 'Compiling $SOURCE'
env['SHCCCOMSTR'] = 'Compiling Shared Library $SOURCE'

env['LINKCOMSTR'] = 'Linking $TARGET'
env['SHLINKCOMSTR'] = 'Linking Shared Library $TARGET'

if target == 'debug':
    print("Building DEBUG configuration...")
    env.Append(
        CCFLAGS = ['-g', '-O0'],
        CPPDEFINES = ['DEBUG']
    )

elif target == 'release':
    print("Building RELEASE configuration...")
    env.Append(
        CCFLAGS = ['-Oz', '-DNDEBUG'],
        CPPDEFINES = ['RELEASE']
    )

else:
    print(f"Error: Unknown target '{target}'. Use 'debug' or 'release'.")
    Exit(1)


env.Append(
    CCGLAGS = ['-Wextra', '-Wall'],
    CPPPATH=['#'],
    LIBS=['m', 'rt']
) 
env.Program('game_of_life', Glob('src/*.c'))
