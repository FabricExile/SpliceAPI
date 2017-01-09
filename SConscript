#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os, sys, platform, copy

Import(
  'parentEnv',
  'FTL_INCLUDE_DIR',
  'FABRIC_DIR',
  'FABRIC_SPLICE_VERSION',
  'STAGE_DIR',
  'FABRIC_BUILD_TYPE',
  'FABRIC_BUILD_OS',
  'FABRIC_BUILD_ARCH',
  'BOOST_DIR'
  )

# configuration flags
if FABRIC_BUILD_OS == "Windows":
  baseCPPDefines = [
    '_SCL_SECURE_NO_WARNINGS=1',
    '_ITERATOR_DEBUG_LEVEL=0',
    '_WIN32_WINNT=0x0600',
  ]
  baseCPPFlags = [
    '/EHsc', 
    '/wd4624',
  ]
  baseLinkFlags = ['chkstk.obj']
  spliceDebugFlags = {
    'CCFLAGS': baseCPPFlags + ['/Od', '/Z7', '/MTd', '/DEBUG'],
    'CPPDEFINES': baseCPPDefines + ['_ITERATOR_DEBUG_LEVEL=2', '_DEBUG', 'DEBUG'],
    'LINKFLAGS': baseLinkFlags + ['/DEBUG', '/NODEFAULTLIB:LIBCMT'],
  }
  spliceReleaseFlags = {
    'CCFLAGS': baseCPPFlags + ['/Ox', '/MT'],
    'CPPDEFINES': baseCPPDefines + ['NDEBUG'],
    'LINKFLAGS': baseLinkFlags + ['/NODEFAULTLIB:LIBCMTD'],
  }
  if FABRIC_BUILD_ARCH.endswith('64'):
    baseCPPDefines.append( 'WIN64' )
else:
  spliceDebugFlags = {
    'CCFLAGS': ['-fPIC', '-m64', '-g'],
    'LINKFLAGS': ['-m64']
  }
  spliceReleaseFlags = {
    'CCFLAGS': ['-fPIC', '-m64', '-O2'],
    'LINKFLAGS': ['-m64'],
    'CPPDEFINES': ['NDEBUG'],
  }

if FABRIC_BUILD_OS == "Darwin":
  for flags in [spliceDebugFlags, spliceReleaseFlags]:
    flags['CCFLAGS'] += [
      '-fvisibility=hidden',
      '-mmacosx-version-min=10.7',
      '-stdlib=libstdc++',
      '-fno-omit-frame-pointer',
      ]      
    flags['LINKFLAGS'] += [
      '-mmacosx-version-min=10.7',
      '-stdlib=libstdc++',
      ]

Export('spliceDebugFlags', 'spliceReleaseFlags')

if FABRIC_BUILD_TYPE == 'Debug':
  parentEnv.MergeFlags(spliceDebugFlags)
else:
  parentEnv.MergeFlags(spliceReleaseFlags)

baseCapiFlags = {
  'CPPPATH': [os.path.join(FABRIC_DIR, 'include')],
  'CPPDEFINES': [
    'FEC_PROVIDE_STL_BINDINGS',
    '__STDC_LIMIT_MACROS',
    '__STDC_CONSTANT_MACROS',
  ],
  'LIBPATH': [],
  'LIBS': [],
}

FABRIC_CORE_VERSION = FABRIC_SPLICE_VERSION.rpartition('.')[0]

staticCapiFlags = copy.deepcopy(baseCapiFlags)
staticCapiFlags['CPPDEFINES'] += ['FEC_STATIC']
if FABRIC_BUILD_OS == 'Windows':
  staticCapiFlags['LIBS'] += [File(os.path.join(FABRIC_DIR, 'lib', 'FabricCore-'+FABRIC_CORE_VERSION+'_s.lib'))]
else:
  staticCapiFlags['LIBS'] += [File(os.path.join(FABRIC_DIR, 'lib', 'libFabricCore-'+FABRIC_CORE_VERSION+'_s.a'))]
Export('staticCapiFlags')

if FABRIC_BUILD_OS == 'Windows':
  fabricCoreLibName = 'FabricCore-'+FABRIC_CORE_VERSION
else:
  fabricCoreLibName = 'FabricCore'

sharedCapiFlags = copy.deepcopy(baseCapiFlags)
sharedCapiFlags['CPPDEFINES'] += ['FEC_SHARED']
sharedCapiFlags['LIBPATH'] += [os.path.join(FABRIC_DIR, 'lib')]
sharedCapiFlags['LIBS'] += [fabricCoreLibName]
Export('sharedCapiFlags')

apiVersion = FABRIC_SPLICE_VERSION.split('-')[0].split('.')
for i in range(1, len(apiVersion)):
  while len(apiVersion[i]) < 3:
    apiVersion[i] = '0'+apiVersion[i]

parentEnv.Append(CPPDEFINES=['SPLICE_API_VERSION='+''.join(apiVersion)])
parentEnv.Append(CPPDEFINES=['SPLICE_MAJOR_VERSION='+FABRIC_SPLICE_VERSION.split('.')[0]])
parentEnv.Append(CPPDEFINES=['SPLICE_MINOR_VERSION='+FABRIC_SPLICE_VERSION.split('.')[1]])
parentEnv.Append(CPPDEFINES=['SPLICE_REVISION_VERSION='+FABRIC_SPLICE_VERSION.split('.')[2]])

parentEnv.Append(CPPPATH=[FTL_INCLUDE_DIR])

if FABRIC_BUILD_OS == 'Windows':
  parentEnv.Append(LIBS = ['advapi32', 'shell32'])

if BOOST_DIR is None:
  print "BOOST_DIR not found. Please specify BOOST_DIR."
  print "Refer to README.txt for more information."
  sys.exit(1)

if not os.path.exists(BOOST_DIR):
  print "BOOST_DIR path invalid: " + BOOST_DIR
  print "Refer to README.txt for more information."
  sys.exit(1)

boostFlags = {
  'CPPPATH': [os.path.join(BOOST_DIR, 'include')],
  'LIBPATH': [os.path.join(BOOST_DIR, 'lib')],
}
if FABRIC_BUILD_OS == 'Windows':
  if FABRIC_BUILD_TYPE == 'Debug':
    boostFlags['LIBS'] = [
      'libboost_thread-vc120-mt-sgd-1_55.lib',
      'libboost_system-vc120-mt-sgd-1_55.lib',
      'libboost_filesystem-vc120-mt-sgd-1_55.lib'
      ]
  else:
    boostFlags['LIBS'] = [
      'libboost_thread-vc120-mt-s-1_55.lib',
      'libboost_system-vc120-mt-s-1_55.lib',
      'libboost_filesystem-vc120-mt-s-1_55.lib'
      ]
else:
  boostFlags['LIBS'] = ['boost_thread','boost_system','boost_filesystem']
Export('boostFlags')

parentEnv.MergeFlags(boostFlags)

env = parentEnv.Clone()

libNameBase = 'FabricSplice'
majMinVer = os.path.splitext(FABRIC_SPLICE_VERSION)[0]
majMinRevVer = FABRIC_SPLICE_VERSION

if FABRIC_BUILD_OS == 'Windows':
  libNameBase = '-'.join([libNameBase, majMinVer])

staticEnv = env.Clone()
staticEnv.VariantDir(staticEnv.Dir('static'), staticEnv.Dir('.').srcnode())
staticEnv.Append(CPPDEFINES=['FEC_SHARED', 'FECS_STATIC', 'FECS_BUILDING'])
staticEnv.MergeFlags(sharedCapiFlags)

staticLibNameBase = libNameBase+'_s'
if FABRIC_BUILD_OS == 'Windows':
  staticLibName = '.'.join([staticLibNameBase, 'lib'])
if FABRIC_BUILD_OS == 'Linux':
  staticLibName = '.'.join(['lib' + staticLibNameBase, 'a', majMinRevVer])
if FABRIC_BUILD_OS == 'Darwin':
  staticLibName = '.'.join(['lib' + staticLibNameBase, majMinRevVer, 'a'])

staticLibrary = staticEnv.Library(
  staticEnv.File(staticLibName),
  staticEnv.Glob('static/*.cpp')
)

installedStaticLibrary = staticEnv.Install(STAGE_DIR.Dir('lib'), staticLibrary)
if FABRIC_BUILD_OS == 'Linux':
  env.AddPostAction(
    installedStaticLibrary,
    [
      [
        'ln', '-snf',
        '.'.join(['lib' + staticLibNameBase, 'a', majMinRevVer]),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + staticLibNameBase, 'a', majMinVer])),
        ],
      [
        'ln', '-snf',
        '.'.join(['lib' + staticLibNameBase, 'a', majMinVer]),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + staticLibNameBase, 'a'])),
        ]
      ]
    )
if FABRIC_BUILD_OS == 'Darwin':
  env.AddPostAction(
    installedStaticLibrary,
    [
      [
        'ln', '-snf',
        '.'.join(['lib' + staticLibNameBase, majMinRevVer, 'a']),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + staticLibNameBase, majMinVer, 'a'])),
        ],
      [
        'ln', '-snf',
        '.'.join(['lib' + staticLibNameBase, majMinVer, 'a']),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + staticLibNameBase, 'a'])),
        ]
      ]
    )

sharedEnv = env.Clone()
sharedEnv.VariantDir(sharedEnv.Dir('shared'), sharedEnv.Dir('.').srcnode())
sharedEnv.Append(CPPDEFINES=['FEC_SHARED', 'FECS_SHARED', 'FECS_BUILDING'])
sharedEnv.MergeFlags(sharedCapiFlags)

if FABRIC_BUILD_OS == 'Windows':
  sharedLibName = '.'.join([libNameBase, 'dll'])
if FABRIC_BUILD_OS == 'Linux':
  sharedLibName = '.'.join(['lib' + libNameBase, 'so', majMinRevVer])
if FABRIC_BUILD_OS == 'Darwin':
  sharedLibName = '.'.join(['lib' + libNameBase, majMinRevVer, 'dylib'])

if FABRIC_BUILD_OS == 'Linux':
  exportsFile = env.File('Linux.exports').srcnode()
  sharedEnv.Append(SHLINKFLAGS = ['-Wl,--version-script='+str(exportsFile)])
elif FABRIC_BUILD_OS == 'Darwin':
  exportsFile = env.File('Darwin.exports').srcnode()
  sharedEnv.Append(SHLINKFLAGS = ['-Wl,-exported_symbols_list', str(exportsFile)])

sharedLibrary = sharedEnv.SharedLibrary(
  sharedEnv.File(sharedLibName),
  sharedEnv.Glob('shared/*.cpp')
)

if FABRIC_BUILD_OS == 'Windows':
  installedSharedLibrary = sharedEnv.Install(STAGE_DIR.Dir('lib'), sharedLibrary[2:])
else:
  installedSharedLibrary = sharedEnv.Install(STAGE_DIR.Dir('lib'), sharedLibrary)
if FABRIC_BUILD_OS == 'Linux':
  env.AddPostAction(
    installedSharedLibrary,
    [
      [
        'ln', '-snf',
        '.'.join(['lib' + libNameBase, 'so', majMinRevVer]),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + libNameBase, 'so', majMinVer])),
        ],
      [
        'ln', '-snf',
        '.'.join(['lib' + libNameBase, 'so', majMinVer]),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + libNameBase, 'so'])),
        ]
      ]
    )
if FABRIC_BUILD_OS == 'Darwin':
  env.AddPostAction(
    installedSharedLibrary,
    [
      [
        'ln', '-snf',
        '.'.join(['lib' + libNameBase, majMinRevVer, 'dylib']),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + libNameBase, majMinVer, 'dylib'])),
        ],
      [
        'ln', '-snf',
        '.'.join(['lib' + libNameBase, majMinVer, 'dylib']),
        STAGE_DIR.Dir('lib').File('.'.join(['lib' + libNameBase, 'dylib'])),
        ]
      ]
    )

spliceDistHeader = staticEnv.Install(STAGE_DIR.Dir('include'), 'FabricSplice.h')
Export('spliceDistHeader')

installedFiles = [installedStaticLibrary, installedSharedLibrary, spliceDistHeader]

if FABRIC_BUILD_OS == 'Windows':
  installedFiles += sharedEnv.Install(STAGE_DIR.Dir('bin'), sharedLibrary[:2])

spliceFlags = {
  'CPPPATH': [STAGE_DIR],
  'LIBS': [installedStaticLibrary],
  'CPPDEFINES': ['_BOOL', 'FECS_STATIC']
}
Export('spliceFlags')

alias = env.Alias('spliceapi', installedFiles)
spliceData = (alias, installedFiles)
Return('spliceData')
